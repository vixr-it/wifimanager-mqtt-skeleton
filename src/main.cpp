#include <Arduino.h>
#include <WiFiManager.h>
#include <MQTT.h>

#define MSG_BUFFER_SIZE	(256)

MQTTClient client;
WiFiManager wm;
WiFiClient net;
unsigned long lastMillis = 0;
char msg[MSG_BUFFER_SIZE];
char mmsg[MSG_BUFFER_SIZE];

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "public", "public")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/hello");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

void setup() {

    WiFi.mode(WIFI_STA);
    Serial.begin(115200);
  

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    //wm.resetSettings();

    bool res;
    res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
    } 
    else {
        Serial.println("connected...yeey :)");
        client.begin("192.168.1.8", net);
        client.onMessage(messageReceived);
        connect();
    }
}

void loop() {

client.loop();

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 10000) {
    lastMillis = millis();
    float objt = 23.5;
    String message = String("weather,location=garden temperature="+String(objt));
    message.toCharArray(msg, message.length());
    Serial.println(msg);
    client.publish("sensors", msg);

    int32 moisture = 2562;
    message = String("moisture,location=garden level="+String(moisture));
    message.toCharArray(mmsg, message.length());
    Serial.println(mmsg);
    client.publish("sensors", msg);
    

    
  }
}