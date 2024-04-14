
#include <ESP8266WiFi.h>
#include <EEPROM.h>

const char *ssid = "LightAP";       // Access Point SSID
const char *password = "12345678";  // Access Point password

WiFiServer server(80);
String header;

const int ledPin = 2;  // GPIO2 pin

const bool LIGHT_ON = LOW;  ////Here light ON is = LOW
const bool LIGHT_OFF = HIGH;
bool lastState = LIGHT_ON;
int setting = 0;  // To store the default LED state preference
int servingPage = 0;

void setup() {
  pinMode(ledPin, OUTPUT);

  EEPROM.begin(512);
  // Read the preference from EEPROM on boot
  int savedState = EEPROM.read(0);
  setting = EEPROM.read(1);
  EEPROM.end();
  if (setting == 1) {
    digitalWrite(ledPin, LIGHT_OFF);
    lastState = LIGHT_OFF;
  } else if (setting == 2 && savedState == LIGHT_OFF) {
    digitalWrite(ledPin, LIGHT_OFF);
    lastState = LIGHT_OFF;
    delay(100);
    EEPROM.begin(512);
    EEPROM.write(1, 2);
    EEPROM.commit();
    EEPROM.end();
  } else if (setting == 3) {
    EEPROM.begin(512);    
    if (savedState == 0) {
      digitalWrite(ledPin, LIGHT_OFF);
      lastState = LIGHT_OFF;
      EEPROM.write(0, 1);

    } else {
      EEPROM.write(0, 0);
    }
    EEPROM.write(1, 3);  /// refresh
    delay(100);
    EEPROM.commit();
    EEPROM.end();
  }

  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.print("Laststate");
  Serial.println(lastState);
  Serial.print("setting");
  Serial.println(setting);

  Serial.println("Configuring Access Point...");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();
  Serial.println("Server started");
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  Serial.println("New client connected");

  String header = "";
  String path = "";

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      Serial.write(c);  // Print client data to Serial Monitor
      header += c;

      if (c == '\n') {
        // Check if it's the first line of the HTTP request containing the path
        if (header.startsWith("GET ")) {
          // Extract the path from the header
          path = header.substring(4, header.indexOf(" ", 5));
        }

        if (path.equals("/LED=ON")) {
          // Handle turning on the LED
          lastState = LIGHT_ON;
          digitalWrite(ledPin, lastState);
          if (setting == 2 || setting == 3) {
            EEPROM.begin(512);
            EEPROM.write(0, LIGHT_ON);
            EEPROM.commit();
            EEPROM.end();
          }
          servingPage = 0;
        } else if (path.equals("/LED=OFF")) {
          // Handle turning off the LED
          lastState = LIGHT_OFF;
          digitalWrite(ledPin, lastState);
          if (setting == 2 || setting == 3) {
            EEPROM.begin(512);
            EEPROM.write(0, LIGHT_OFF);
            EEPROM.commit();
            EEPROM.end();
          }
          servingPage = 0;
        } else if (path.equals("/STATE=ON")) {
          // Handle state ON
          setting = 0;
          EEPROM.begin(512);
          EEPROM.write(1, 0);
          EEPROM.commit();
          EEPROM.end();
          servingPage = 1;
        } else if (path.equals("/STATE=OFF")) {
          // Handle state OFF
          setting = 1;
          EEPROM.begin(512);
          EEPROM.write(1, 1);
          EEPROM.commit();
          EEPROM.end();
          servingPage = 1;
        } else if (path.equals("/STATE=LS")) {
          // Handle state LS
          setting = 2;
          EEPROM.begin(512);
          EEPROM.write(1, 2);
          EEPROM.write(0, lastState);
          EEPROM.commit();
          EEPROM.end();
          servingPage = 1;
        } else if (path.equals("/STATE=SC")) {
          // Handle state LS
          setting = 3;
          EEPROM.begin(512);
          EEPROM.write(1, 3);
          EEPROM.write(0, lastState);
          EEPROM.commit();
          EEPROM.end();
          servingPage = 1;
        } else if (path.equals("/") || path.equals("")) {
          // Handle the root path, send the home page
          servingPage = 0;
        } else if (path.equals("/Settings")) {
          servingPage = 1;
        } else servingPage = 9;

        if (servingPage == 0)
          sendHomepage(client);
        else if (servingPage == 1)
          sendSettingsPage(client);
        else send404Error(client);

        delay(1);
        Serial.println("Client disconnected");
        header = "";
        break;  // Exit the while loop after sending the response
      }
    }
  }
  Serial.println("Client disconnected");
}



void sendHomepage(WiFiClient &client) {
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: text/html\r\n";
  response += "Connection: close\r\n";
  response += "\r\n";
  response += "<!DOCTYPE HTML>\r\n";
  response += "<html>\r\n";
  response += "<body style='font-size: 48px; ";  // Larger font size

  // Change background color based on the ledState for eye comfort
  response += (lastState == LIGHT_ON ? "background-color: white;" : "background-color: gray;");

  response += "'>\r\n";

  response += "<h1 style='font-size: 60px;'>TCs Light Control</h1>\r\n";  // Larger header font
  response += "\r\n";
  response += "<p><a href=\"/LED=ON\">Turn ON Light</a></p>\r\n";
  response += "<p><a href=\"/LED=OFF\">Turn OFF Light</a></p>\r\n";
  response += "\r\n";
  response += "<p>Current state: ";
  response += (lastState == LIGHT_ON ? "ON" : "OFF");
  response += "</p>\r\n";
  response += "<p><a href=\"/Settings\">Settings</a></p>\r\n";
  response += "</body>\r\n";
  response += "</html>\r\n";

  client.print(response);
}


void sendSettingsPage(WiFiClient &client) {
  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: text/html\r\n";
  response += "Connection: close\r\n";
  response += "\r\n";
  response += "<!DOCTYPE HTML>\r\n";
  response += "<html>\r\n";
  response += "<body style='font-size: 32px;'>\r\n";             // Larger font size
  response += "<h1 style='font-size: 48px;'>Settings</h1>\r\n";  // Larger header font
  response += "<p>Change the state of the light during power ON</p>\r\n";

  response += "<p><strong>Options</strong></p>\r\n";
  response += "<p><a href=\"/STATE=ON\">Light ON (Recommended)</a></p>\r\n";
  response += "<p><a href=\"/STATE=OFF\">Light OFF</a></p>\r\n";
  response += "<p><a href=\"/STATE=LS\">Last State</a></p>\r\n";
  response += "<p><a href=\"/STATE=SC\">Power toggle controlled</a></p>\r\n";
  response += "<p>Current setting: ";
  switch (setting) {
    case 0: response += "Light ON"; break;
    case 1: response += "Light OFF"; break;
    case 2: response += "Last State"; break;
    default: response += "Power toggle controlled"; break;
  }
  response += "\r\n";
  response += "<p><a href=\"/\">Back</a></p>\r\n";
  response += "</body>\r\n";
  response += "</html>\r\n";

  client.print(response);
}

void send404Error(WiFiClient &client) {
  String response = "HTTP/1.1 404 Not Found\r\n";
  response += "Content-Type: text/html\r\n";
  response += "Connection: close\r\n";
  response += "\r\n";
  response += "<!DOCTYPE HTML>\r\n";
  response += "<html>\r\n";
  response += "<body style='font-size: 32px; background-color: #ffcccc;'>\r\n";
  response += "<h1 style='color: #990000;'>Oops! 404 Not Found</h1>\r\n";
  response += "<p>Sorry, the page you are looking for isn't available!</p>\r\n";
  response += "<p>Contact Thilina(Developer) using tcshared@outlook.com if the issue persist</p>\r\n";
  response += "<p><a href='/'>Back to Home</a></p>\r\n";
  response += "\r\n";
  response += "</body>\r\n";
  response += "</html>\r\n";

  client.print(response);
}
