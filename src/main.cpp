#include <header.h>
#include <DHTesp.h>
#include <MQ135.h>

#define ledPin LED_BUILTIN
#define BUZZERPIN D3
#define MQ135PIN A0
#define DHTPIN D1

MQ135 mq135_sensor(MQ135PIN);
DHTesp dht;

void setup() {
    Serial.begin(115200);
    dht.setup(DHTPIN, DHTesp::DHT11);
    delay(1000);
    pinMode(ledPin, OUTPUT);
    pinMode(BUZZERPIN, OUTPUT);
    wifiSetup();
    serverSetup();
    websocketSetup();
    mdnsSetup();
}

unsigned long previousMillis;
unsigned long currentMillis;

void loop() {
    MDNS.update();
    webSocket.loop();
    currentMillis = millis();
    if (currentMillis - previousMillis >= 1000) { previousMillis = currentMillis;
        digitalWrite(ledPin, !digitalRead(ledPin));
        humidity = dht.getHumidity();
        temperature = dht.getTemperature();
        if (isnan(humidity) || isnan(temperature)) {
            Serial.println("Failed to read from DHT sensor!");
            return;
        }
        air = mq135_sensor.getPPM();
        airStatus = (air <= 1000) ? "Good Air" : ((air > 1000 && air <= 2000) ? "Bad Air" : "Danger!");
        digitalWrite(BUZZERPIN, air > 2000);
        flush();
        json["humidity"] = (String)humidity + " %";
        json["temperature"] = (String) temperature + " °C";
        json["air"] = (String) air + " ppm";
        json["status"] = airStatus;
        serializeJson(json, jsonString);
//        Serial.println(jsonString);
        webSocket.broadcastTXT(jsonString);
    }
    delay(50);
}