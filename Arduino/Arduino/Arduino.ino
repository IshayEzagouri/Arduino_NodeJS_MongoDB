#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Your SSID here";
const char* password = "Your Password here";

struct States {
  int state1;
  int state2;
  int state3;
  int state4;
};

int relay = 4;
#define light_sensor A0
int LED = 12;

States get_data() {
  HTTPClient http;
  WiFiClient client;

  http.begin(client, "http://104.28.60.68:8000/data");
  int httpResponseCode = http.GET();
  States timedelay;

  if (httpResponseCode == HTTP_CODE_OK) {
    String response = http.getString();
    Serial.println("Response received:");
    Serial.println(response);

    String state1, state2, state3, state4;
 
    int startIndex = response.indexOf("\"state1\":\"") + 10;
    int endIndex = response.indexOf("\"", startIndex);
    state1 = response.substring(startIndex, endIndex);

    startIndex = response.indexOf("\"state2\":\"") + 10;
    endIndex = response.indexOf("\"", startIndex);
    state2 = response.substring(startIndex, endIndex);

    startIndex = response.indexOf("\"state3\":\"") + 10;
    endIndex = response.indexOf("\"", startIndex);
    state3 = response.substring(startIndex, endIndex);

    startIndex = response.indexOf("\"state4\":\"") + 10;
    endIndex = response.indexOf("\"", startIndex);
    state4 = response.substring(startIndex, endIndex);

    Serial.println("State 1: " + state1);
    Serial.println("State 2: " + state2);
    Serial.println("State 3: " + state3);
    Serial.println("State 4: " + state4);

    int state1Val = state1.toInt();
    int state2Val = state2.toInt();
    int state3Val = state3.toInt();
    int state4Val = state4.toInt();

    timedelay.state1 = state1Val;
    timedelay.state2 = state2Val;
    timedelay.state3 = state3Val;
    timedelay.state4 = state4Val;
  } else {
    Serial.print("Error: ");
    Serial.println(httpResponseCode);
    timedelay.state1 = 2;
    timedelay.state2 = 2;
    timedelay.state3 = 2;
    timedelay.state4 = 2;
  }

  http.end();
  return timedelay;
}

void setup() {
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  pinMode(LED, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

void loop() {
  States result = get_data();
  Serial.print("Value 1: ");
  Serial.println(result.state1);
  Serial.print("Value 2: ");
  Serial.println(result.state2);
  Serial.print("Value 3: ");
  Serial.println(result.state3);
  Serial.print("Value 4: ");
  Serial.println(result.state4);

  unsigned long currentTime = millis();

  unsigned long relayEndTime = currentTime + (result.state1 * 1000);
  while (millis() < relayEndTime) {
    Serial.println("Relay_time");
    digitalWrite(relay, HIGH);
  }

  unsigned long lightEndTime = millis() + (result.state2 * 1000);
  int count = 0;
  int sum = 0;
  int avg = 0;
  while (millis() < lightEndTime) {
    int sensorValue = analogRead(light_sensor);           
    int mappedValue = map(sensorValue, 0, 1023, 0, 255);  
    sum += mappedValue;
    count++;
    delay(1000);  
  }
  avg = sum / count;
  Serial.println(avg);

  unsigned long LEDEndTime = millis() + (result.state3 * 1000);
  while (millis() < LEDEndTime) {
    analogWrite(LED, avg);
    Serial.println("Turn on LED");
  }
  analogWrite(LED, 0);
  Serial.println("LED OFF");

  Serial.println("suspended");
  delay(result.state4 * 1000);
}
