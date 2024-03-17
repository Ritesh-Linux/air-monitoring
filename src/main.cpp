#include <header.h>

#define ledPin LED_BUILTIN

void setup() {
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    wifiSetup();
    serverSetup();
    websocketSetup();
}

unsigned long previousMillis;
unsigned long currentMillis;

void loop() {
    webSocket.loop();
    value = analogRead(A0);

    currentMillis = millis();
    if (currentMillis - previousMillis >= 1000) {
        previousMillis = currentMillis;
        digitalWrite(ledPin, !digitalRead(ledPin));
        data = (String) value;
        webSocket.broadcastTXT(data);
    }
    delay(50);
}