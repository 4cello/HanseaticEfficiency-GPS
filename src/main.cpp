#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include "secrets.h"

#define RX_PIN D2
#define TX_PIN D3
#define MAX_CLIENTS 5

const char * ssid = WIFI_SSID;
const char * wifi_password = WIFI_PASSWORD;

SoftwareSerial ss(RX_PIN, TX_PIN);
TinyGPSPlus gps;

WiFiServer server(23);
WiFiClient serverClients[MAX_CLIENTS];

void setup() {
    Serial.begin(115200);
    Serial.print("\nConnecting to "); Serial.print(ssid);

    ss.begin(9600);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, wifi_password);
    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED && i++ < 20) {
      Serial.print(".");
      delay(500);
    }
    Serial.println("");
    if (i == 21) {
      Serial.print("Could not connect to"); Serial.println(ssid);
      while (1) delay(500);
    }
    server.begin();
    server.setNoDelay(true);
    Serial.print(WiFi.localIP());
}

void checkClients() {
  //check if there are any new clients
  uint8_t i;
  if (server.hasClient()) {
    for (i = 0; i < MAX_CLIENTS; i++) {
      //find free/disconnected spot
      if (!serverClients[i] || !serverClients[i].connected()) {
        if (serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        serverClients[i].write("Connected\n");
        continue;
      }
    }
    //no free/disconnected spot so reject
    WiFiClient serverClient = server.available();
    serverClient.stop();
  }
}

void loop() {
  checkClients();
  uint8_t i;
  char read = 0;
  while (ss.available() > 0) {
    read = (char) ss.read();
    gps.encode(read);

    Serial.print(read);
    for (i = 0; i < MAX_CLIENTS; i++) {
      if (serverClients[i] && serverClients[i].connected()) {
        serverClients[i].write(read);
        delay(1);
      }
    }
    if (gps.location.isUpdated()) {
      Serial.print("LAT=");  Serial.println(gps.location.lat(), 6);
      Serial.print("LONG="); Serial.println(gps.location.lng(), 6);
      Serial.print("ALT=");  Serial.println(gps.altitude.meters());
    }
  }
}