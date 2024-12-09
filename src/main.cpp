#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define MAX_COUNT 10

const char *ssid = "ASUS_D0";
const char *password = "GeogeBool1815.";

int count = 0;
bool isHigh = true;
int threshold = 1024;

void init_threshold()
{
    for (int i = 0; i < 20; i++) {
        threshold = min(threshold, analogRead(A0));
        delay(50);
    }
}

void setup()
{

    Serial.begin(115200);
    WiFi.begin(ssid, password);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // WiFi 연결 대기
    do {
        init_threshold();
        Serial.println("Connecting to WiFi...");
    } while (WiFi.status() != WL_CONNECTED);

    Serial.println("Connected to WiFi");
    digitalWrite(LED_BUILTIN, HIGH);
    pinMode(LED_BUILTIN_AUX, OUTPUT);
    digitalWrite(LED_BUILTIN_AUX, HIGH);
    threshold += 100;
    Serial.println(threshold);
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED) {
        int bright = analogRead(A0);
        Serial.println((isHigh ? "HIGH " : "LOW  ") + String(bright));
        if (isHigh) {
            if (bright > threshold) {
                count++;
                if (count == MAX_COUNT)
                {
                    count = 0;
                    isHigh = false;
                    digitalWrite(LED_BUILTIN_AUX, LOW);
                }
            } else {
                count = 0;
            }
        }
        else {
            if (bright <= threshold - 10) {
                isHigh = true;
                digitalWrite(LED_BUILTIN_AUX, HIGH);
                WiFiClient client;
                HTTPClient http;
                http.begin(client, "http://192.168.50.128:8000/brew/000001/data");
                http.addHeader("Content-Type", "application/json");
                int httpResponseCode = http.POST("");
                if (httpResponseCode > 0) {
                    String response = http.getString(); 
                    Serial.println("HTTP Response Code: " + String(httpResponseCode));
                    Serial.println("Response: " + response);
                }
                else {
                    Serial.println("Error on sending POST: " + String(httpResponseCode));
                }
                http.end();
            }
        }
    }
    else {
        Serial.println("WiFi Disconnected");
    }
    delay(10);
}
