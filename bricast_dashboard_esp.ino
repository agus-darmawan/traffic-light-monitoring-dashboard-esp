#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

const char* ssid = "SSID";         // Initial Wi-Fi SSID
const char* password = "PASSWORD"; // Initial Wi-Fi password

String device_id = "your_device_id"; // Initial device ID
int delayInterval = 60000; // Initial delay interval in milliseconds

WebServer server(80);

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

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/", handleRoot);
  server.on("/submit", handleSubmit);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  // POST data periodically
  static unsigned long lastPostTime = 0;
  if (millis() - lastPostTime > delayInterval) {
    postData();
    lastPostTime = millis();
  }
}

void handleRoot() {
  String html = "<!DOCTYPE html>\
<html lang=\"en\">\
<head>\
  <meta charset=\"UTF-8\">\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
  <title>Configuration</title>\
  <style>\
    body {\
      background-color: #f0f0f0;\
      font-family: Arial, sans-serif;\
      display: flex;\
      justify-content: center;\
      align-items: center;\
      height: 100vh;\
    }\
    .form-container {\
      background-color: white;\
      padding: 20px;\
      border-radius: 5px;\
      box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);\
    }\
    .form-container h1 {\
      text-align: center;\
      margin-bottom: 20px;\
    }\
    .form-container label {\
      display: block;\
      margin-bottom: 5px;\
      font-weight: bold;\
    }\
    .form-container input[type='text'], .form-container input[type='password'] {\
      width: 100%;\
      padding: 8px;\
      margin-bottom: 10px;\
      border: 1px solid #ccc;\
      border-radius: 3px;\
    }\
    .form-container input[type='submit'] {\
      width: 100%;\
      padding: 10px;\
      background-color: #007BFF;\
      color: white;\
      border: none;\
      border-radius: 3px;\
      cursor: pointer;\
      font-size: 16px;\
    }\
    .form-container input[type='submit']:hover {\
      background-color: #0056b3;\
    }\
  </style>\
</head>\
<body>\
  <div class=\"form-container\">\
    <h1>Device Configuration</h1>\
    <form action=\"/submit\" method=\"post\">\
      <label for=\"ssid\">WiFi SSID:</label>\
      <input type=\"text\" id=\"ssid\" name=\"ssid\" required>\
      <label for=\"password\">WiFi Password:</label>\
      <input type=\"password\" id=\"password\" name=\"password\" required>\
      <label for=\"device_id\">Device ID:</label>\
      <input type=\"text\" id=\"device_id\" name=\"device_id\" required>\
      <label for=\"delay\">Delay (ms):</label>\
      <input type=\"text\" id=\"delay\" name=\"delay\" required>\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
  </div>\
</body>\
</html>";
  server.send(200, "text/html", html);
}

void handleSubmit() {
  if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("device_id") && server.hasArg("delay")) {
    ssid = server.arg("ssid").c_str();
    password = server.arg("password").c_str();
    device_id = server.arg("device_id");
    delayInterval = server.arg("delay").toInt();

    // Reconnect to WiFi with new credentials
    WiFi.begin(ssid, password);
    Serial.print("Reconnecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println("Reconnected to WiFi");

    // Print the new IP address
    Serial.print("New IP Address: ");
    Serial.println(WiFi.localIP());

    server.send(200, "text/html", "Settings updated. Please refresh the page.");
  } else {
    server.send(400, "text/html", "Invalid Input");
  }
}

void postData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://10.10.21.14:3333/api/v1/statuses");
    http.addHeader("Content-Type", "application/json");

    String httpRequestData = "{\"device_id\":\"" + device_id + "\"}";
    Serial.print("Sending POST request: ");
    Serial.println(httpRequestData);

    int httpResponseCode = http.POST(httpRequestData);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Response: ");
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
