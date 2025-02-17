#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>


// Create a web server on port 80
ESP8266WebServer server(80);


// Store WiFi credentials
String ssid = "";
String password = "";


// Variables to store the WiFi credentials after user input
const char* AP_ssid = "NodeMCU_AP_1";
const char* AP_password = "12345678"; // Set a password for the AP


// Servo setup
Servo myServo;
int servoPin = D4; // Pin connected to the servo motor


// Serve the initial HTML page for WiFi credentials
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>WiFi Setup</title>
    </head>
    <body>
      <h1>Enter WiFi Credentials</h1>
      <form action="/connect" method="POST">
        SSID: <input type="text" name="ssid"><br>
        Password: <input type="password" name="password"><br>
        <input type="submit" value="Connect">
      </form>
    </body>
    </html>
  )rawliteral";


  server.send(200, "text/html", html);
}


// Handle WiFi credentials and attempt connection
void handleConnect() {
  ssid = server.arg("ssid");
  password = server.arg("password");


  String response = "<html><body><h1>Attempting to Connect...</h1>";
  response += "<p>SSID: " + ssid + "</p>";
  server.send(200, "text/html", response);


  // Start the connection attempt
  WiFi.begin(ssid.c_str(), password.c_str());
}


// Serve the HTML page for servo control
void handleServoPage() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>Servo Control</title>
      <style>
        body {
          font-family: Arial, sans-serif;
          text-align: center;
          padding: 20px;
        }
        .slider {
          width: 300px;
        }
      </style>
    </head>
    <body>
      <h1>Servo Motor Control</h1>
      <p>Adjust the slider to set the servo angle.</p>
      <input type="range" min="0" max="180" value="90" class="slider" id="angleSlider" oninput="updateAngle()">
      <p>Current Angle: <span id="angleValue">90</span>°</p>
      <button onclick="sendAngle()">Set Angle</button>


      <script>
        function updateAngle() {
          const slider = document.getElementById('angleSlider');
          const angleValue = document.getElementById('angleValue');
          angleValue.textContent = slider.value;
        }


        function sendAngle() {
          const slider = document.getElementById('angleSlider');
          const angle = slider.value;
          const xhr = new XMLHttpRequest();
          xhr.open('GET', '/servo?angle=' + angle, true);
          xhr.send();
          alert('Angle set to ' + angle + '°');
        }
      </script>
    </body>
    </html>
  )rawliteral";


  server.send(200, "text/html", html);
}


// Handle the servo movement
void handleServoMove() {
  if (!server.hasArg("angle")) {
    server.send(400, "text/plain", "Angle not provided!");
    return;
  }


  int angle = server.arg("angle").toInt();
  if (angle < 0 || angle > 180) {
    server.send(400, "text/plain", "Invalid angle. Please provide an angle between 0 and 180.");
    return;
  }


  myServo.write(angle);
  String response = "<html><body><h1>Servo Moved</h1>";
  response += "<p>Servo moved to " + String(angle) + " degrees.</p>";
  server.send(200, "text/html", response);
}


void setup() {
  Serial.begin(115200);


  // Servo setup
  myServo.attach(servoPin);


  // Start in AP mode
  WiFi.softAP(AP_ssid, AP_password);
  Serial.println("AP mode started. SSID: " + String(AP_ssid));


  // Start the web server
  server.on("/", handleRoot);           // Serve the WiFi setup page
  server.on("/connect", handleConnect); // Handle WiFi connection
  server.on("/servo", handleServoPage); // Serve the servo control page
  server.on("/servo", HTTP_GET, handleServoMove); // Handle servo angle requests
  server.begin();
  Serial.println("Web server started. Access the NodeMCU via its IP.");


  // Print the IP address of the AP
  Serial.println("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}


void loop() {
  server.handleClient();


  // If the device is in station mode and successfully connected
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
}





