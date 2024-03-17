#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

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

char html[] PROGMEM = R"(
<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="UTF-8" />
        <meta name="viewport" content="width=device-width, initial-scale=1.0" />
        <script src="https://cdn.tailwindcss.com"></script>
        <style>
            table {
                width: 100%;
            }
            td {
                padding: 6px;
            }
            table tr td:nth-child(1) {
                text-align: left;
            }
            table tr td:nth-child(2) {
                text-align: right;
            }
        </style>
    </head>
    <body>
        <div
            class="shadow-[0_20px_50px_rgba(8,_112,_184,_0.7)] w-[320px] mx-auto rounded-xl mt-4"
        >
            <div class="px-8 py-4">
                <div
                    class="font-bold text-xl mb-4 text-[rgba(8,_112,_184,_1)] text-center"
                >
                    Weather Details
                </div>
                <table>
                    <tr>
                        <td>Humidity :</td>
                        <td id="humidity">54 %</td>
                    </tr>
                    <tr>
                        <td>Temperature :</td>
                        <td id="temperature">27 °C</td>
                    </tr>
                    <tr>
                        <td>Air Quality :</td>
                        <td id="air">80 ppm</td>
                    </tr>
                    <tr>
                        <td>AQI Status :</td>
                        <td id="status">Good Air</td>
                    </tr>
                </table>
            </div>
        </div>
        <p id="value" class="mt-12 text-center"></p>
        <script>
            socket = new WebSocket('ws://' + location.host + ':81')
            socket.onopen = (e) => {
                console.log('[socket] socket.onopen')
            }
            socket.onerror = (e) => {
                console.log('[socket] socket.onerror')
            }
            socket.onmessage = (e) => {
                const dataString = e.data
                if (dataString !== 'Connected') {
					const data = JSON.parse(dataString)
					console.log(data)
                    document.getElementById('humidity').innerHTML = data.humidity
                    document.getElementById('temperature').innerHTML = data.temperature
                    document.getElementById('air').innerHTML = data.air
                    document.getElementById('status').innerHTML = data.status
                }
                document.getElementById('value').innerHTML = e.data
            }
        </script>
    </body>
</html>
)";

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
        request -> send(200, "text/html", html);
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