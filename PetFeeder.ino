#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>


 
const char* ssid = "WIFI"; // Enter your WiFi name
const char* password =  "SENHA"; // Enter WiFi password
const char* mqttServer = "hairdresser.cloudmqtt.com";
const int mqttPort = 15483;
const char* mqttUser = "oiksomki";
const char* mqttPassword = "3th-yoyHsNdY";

boolean SensorEstado ; 
boolean ultSensorEstado = HIGH;
int SENSOR = 4; //Sensor PIR conectado no NodeMCU pin D2

Servo servo;

WiFiClient espClient;
PubSubClient client(espClient);
 
void setup () {
 
  WiFi.begin(ssid, password);
  Serial.begin(9600);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando no Wifi..");
  }
  Serial.println("Conectado no Wifi com sucesso!");


 
  pinMode(SENSOR, INPUT); 
 
  pinMode(LED_BUILTIN, OUTPUT);
 
  delay(30000);
  servo.attach(5); //Servo motor conectado no NodeMCU pin D1
  servo.write(0);

  delay(2000);

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
      Serial.println("Conectando no MQTT...");
   
      if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
   
        Serial.println("conectado");  
   
      } else {
   
        Serial.print("Erro");
        Serial.print(client.state());
        delay(2000);
   
      }
    }
   
}


void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Mensagem do topico: ");
  Serial.println(topic);
 
  Serial.print("Mensagem:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  if (payload[0] == 'E'){
        Serial.println("nchendo o pote!");
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
        digitalWrite(LED_BUILTIN, HIGH);
        servo.write(90);
    
        delay(300);
    
        servo.write(0);
    
        delay(300);
    }

   
     
  Serial.println();
  Serial.println("-----------------------");
 
}
 
void loop()
{
  
  SensorEstado = digitalRead(SENSOR);  //HIGH quando o sensor for detectado, se não LOW
 
  if (SensorEstado != ultSensorEstado)  
  {
    Serial.println("Sensor Detectado");
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    servo.write(90);

    delay(300);

    servo.write(0);

    delay(300);
    
    if (WiFi.status() == WL_CONNECTED)  //Verifica a conexão do wifi
    {
      
      client.publish("esp/test", "Sensor detectado! Enchendo o pote..."); 
      client.subscribe("esp/test");
 
    }
    ultSensorEstado = SensorEstado;
  }
  client.loop();
}
