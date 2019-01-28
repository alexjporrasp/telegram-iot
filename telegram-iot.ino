#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "zumbador.h"
#include "home_wifi.h"

// Update these with values suitable for your network.
const char* mqtt_server = "172.20.10.5"; // ip address of the mqtt server

WiFiClient espClient;
PubSubClient client(espClient);

// Constant defining the GPIO outputs
#define EXTERNAL_LED 0
#define MOTOR_RIGHT 4
#define MOTOR_LEFT 5

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // ssid and password are defined in 'home_wifi.h'

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

// Every time an MQTT message arrives this function is called
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message Arrived");

  if ((char)payload[0] == '1') {
    analogWrite(EXTERNAL_LED, 255);   // LED with low intensity
  } else if ((char)payload[0] == '2') {
    analogWrite(EXTERNAL_LED, 512);   // LED with medium intensity
  } else if ((char)payload[0] == '3') {
    digitalWrite(EXTERNAL_LED, HIGH);   // LED with high intensity
  } else if ((char)payload[0] == '5') {
    play_music(); // Reproduce music
  } 
  else if ((char)payload[0] == '7'){ // Turn motor direction one
    digitalWrite(MOTOR_LEFT, LOW);
    digitalWrite(MOTOR_RIGHT, HIGH);  
  } else if ((char)payload[0] == '6'){ // Turn motor opposite direction
    digitalWrite(MOTOR_RIGHT, LOW);
    digitalWrite(MOTOR_LEFT, HIGH);   
  } else { // Turn everything off
    digitalWrite(EXTERNAL_LED, LOW);
    digitalWrite(MOTOR_RIGHT, LOW);
    digitalWrite(MOTOR_LEFT, LOW);
    play_alarm();
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
      // client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("/led_on"); // '/led_on' is the name of the topic that handles the system
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(EXTERNAL_LED, OUTPUT);     // Initialize pins as output
  pinMode(MOTOR_RIGHT, OUTPUT);
  pinMode(MOTOR_LEFT, OUTPUT);
  
 // pinMode(zumbador, OUTPUT);
  
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.subscribe("/led_on");
  client.setCallback(callback);
}

void loop() {
  // Commented code is for debugging purposes
  // digitalWrite(MOTOR_RIGHT, LOW);
  // int analog_input = analogRead(A0);
  
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();

  // long now = millis();
}
