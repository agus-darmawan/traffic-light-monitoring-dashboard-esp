#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "HO-SATKOMINDO";         
const char* password = "s@tkom1ndo"; 

const char* serverName = "API/api/v1/statuses";

String device_id = "55";

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) { 
    HTTPClient http;

    http.begin(serverName); 
    http.addHeader("Content-Type", "application/json"); 

    // Create the JSON payload
    String httpRequestData = "{\"device_id\":\"" + device_id + "\"}";
    Serial.print("Sending POST request: ");
    Serial.println(httpRequestData);

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    // Check the response code
    if (httpResponseCode > 0) {
      String response = http.getString(); // Get the response to the request
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Response: ");
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(60000); // Send a request every 60 seconds
}
