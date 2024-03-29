#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

#include "htmlPage.h"

AsyncWebServer server(80);
WebSocketsServer webSocket(81);

const char *ssid = "realme8266";
const char *pass = "0123456789";

int humidity, temperature, air;
String airStatus;

JsonDocument json;
String jsonString;

void flush() {
    json.clear();
    jsonString = "";
}

void wifiSetup() {
    WiFi.begin(ssid, pass);
    Serial.print("\nConnecting to wifi ");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(500);
    }
    Serial.println("\nConnected to wifi " + (String) ssid);
    Serial.println("IP Address: " + WiFi.localIP().toString());
}

void mdnsSetup() {
    Serial.println(
        !MDNS.begin("esp")
        ? "Error setting up mDNS responder!"
        : "mDNS responder started"
    );
}

void serverSetup() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request -> send(200, "text/html", htmlPage);
    });
    server.on("/api/monitoring", HTTP_GET, [](AsyncWebServerRequest *request){
        request -> send(200, "application/json", jsonString);
    });
    server.onNotFound([](AsyncWebServerRequest *request) {
        request -> send(404, "text/plain", "Not found");
    });
    server.begin();
}

void websocketSetup() {
    webSocket.begin();
    webSocket.onEvent([](uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
        switch (type) {
            case WStype_DISCONNECTED:
                Serial.printf("[%u] Disconnected!\n", num);
                break;
            case WStype_CONNECTED: {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
                webSocket.sendTXT(num, "Connected");
            } break;
            case WStype_TEXT:
                Serial.printf("[%u] Received text: %s\n", num, payload);
                webSocket.sendTXT(num, payload);
                break;
            default: break;
        }
    });
}