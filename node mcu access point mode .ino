#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


// Create a web server on port 80
ESP8266WebServer server(80);


// Store WiFi credentials
String ssid = "";
String password = "";


// Variables to store the WiFi credentials after user input
const char* AP_ssid = "NodeMCU_AP";
const char* AP_password = "12345678"; // Set a password for the AP


void handleRoot() {
  String html = "<html><body><h1>Enter WiFi Credentials</h1>";
  html += "<form action='/connect' method='POST'>";
  html += "SSID: <input type='text' name='ssid'><br>";
  html += "Password: <input type='password' name='password'><br>";
  html += "<input type='submit' value='Connect'>";
  html += "</form></body></html>";


  server.send(200, "text/html", html);
}


void handleConnect() {
  ssid = server.arg("ssid");
  password = server.arg("password");


  String response = "<html><body><h1>Connecting to WiFi...</h1>";
  response += "<p>SSID: " + ssid + "</p>";
  server.send(200, "text/html", response);


  // Start the connection attempt
  WiFi.begin(ssid.c_str(), password.c_str());
}


void setup() {
  Serial.begin(115200);


  // Start in AP mode
  WiFi.softAP(AP_ssid, AP_password);
  Serial.println("AP mode started. SSID: " + String(AP_ssid));


  // Start the web server
  server.on("/", handleRoot);
  server.on("/connect", handleConnect);
  server.begin();
  Serial.println("Web server started. Access the NodeMCU via its IP.");


  // Print the IP address of the AP
  Serial.println("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}


void loop() {
  server.handleClient();


  // If the device is in station mode and successfully connected, switch mode
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());


    // Stop AP mode
    WiFi.softAPdisconnect(true);
    Serial.println("AP mode disabled. Device is now connected to the WiFi.");
    
    // You can add code here to do other things after WiFi connection


    // Stop the server since you no longer need the AP
    server.stop();
    
    // To avoid repeatedly switching modes, break out of the loop
    while (true);
  }
}