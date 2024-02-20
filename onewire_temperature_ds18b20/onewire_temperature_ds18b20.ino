#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>                
#include <DallasTemperature.h>
 
OneWire ourWire(D3);                //Se establece el pin 2  como bus OneWire
 
DallasTemperature sensors(&ourWire); //Se declara una variable u objeto para nuestro sensor
// Update these with values suitable for your network.

const char* ssid = "TP-JAVI";
const char* password = "xavier1234";
const char* mqtt_server = "192.168.0.250";
const char* TOPIC = "Tanque1/canal/temperature/sensor1";
WiFiClient espClient;
PubSubClient client(espClient);


unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

int sensorValue = 0;  // variable to store the value coming from the sensor


void setup_wifi() ;
void callback(char* topic, byte* payload, unsigned int length) ;
void reconnect() ;
void setup() {
  Serial.begin(115200);
  sensors.begin();   //Se inicia el sensor
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
 
void loop() {
    if (!client.connected()) {
    reconnect();
  }
  client.loop();
  sensors.requestTemperatures();   //Se envía el comando para leer la temperatura
  float temp= sensors.getTempCByIndex(0); //Se obtiene la temperatura en ºC
/*
  Serial.print("Temperatura= ");
  Serial.print(temp);
  Serial.println(" C");
*/
  unsigned long now = millis();
  float volTOPICe = 0.0;
  if (now - lastMsg > 500) {
    
    lastMsg = now;

    snprintf (msg, MSG_BUFFER_SIZE, "%.2f", temp);
    //Serial.print("Publish message: ");
    //Serial.println(msg);
    client.publish(TOPIC, msg);
  }
  delay(100);                     
  }


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void callback(char* topic, byte* payload, unsigned int length) {
  if (strcmp(TOPIC, topic) == 0) {
    digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));  // Turn the LED on (Note that LOW is the volTOPICe level
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(TOPIC, "Teperature sensor");
      // ... and resubscribe
      client.subscribe(TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
