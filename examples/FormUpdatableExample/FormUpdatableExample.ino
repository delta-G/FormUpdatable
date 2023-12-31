/*

FormUpdatableExample.ino  --  Update values in running code using HTML forms
     Copyright (C) 2023  David C.

     This program is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.

     */



#include "WiFiS3.h"

#include "arduino_secrets.h"
#include "FormUpdatable.h"

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS;
WiFiServer server(2080);
IPAddress ipAddress(192, 168, 1, 81);

int intValue = 0;
int oldIntValue = 0;
float floatValue = 0.00;
float oldFloatValue = 0.00;
int blinkInterval = 500;


FormUpdatableValue iuValue(intValue, "value");
FormUpdatableValue fuValue(floatValue, "float");
FormUpdatableValue blValue(blinkInterval, "interval");

// Examples of overriding templates to get different functionality
//
// template<> 
// void FormUpdatableValue<int>::parseValue(char* ptr){
//   Serial.println("Called New Function");
//   value = atoi(ptr);
// }

// template<> 
// void FormUpdatableValue<float>::printValue(WiFiClient *client){
//   client->print(value, 5);
// }

void setup() {
  //Initialize serial and wait for port to open:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\nFormUpdatableExample.ino\n\n");
  startWiFi();
  server.begin();
  printWiFiStatus();
}


void loop() {
  handleClient();
  if (intValue != oldIntValue) {
    Serial.println("******************VALUE*******************");
    Serial.print("Value : ");
    Serial.println(intValue);
    oldIntValue = intValue;
  }
  if (floatValue != oldFloatValue) {
    Serial.println("******************FLOAT*******************");
    Serial.print("fValue : ");
    Serial.println(floatValue);
    oldFloatValue = floatValue;
  }
  static unsigned long pm = millis();
  unsigned long cm = millis();
  if(cm - pm >= blinkInterval){
    static uint8_t state = LOW;
    state = 1 - state;
    digitalWrite(LED_BUILTIN, state);
    pm = cm;
  }
}


void handleClient(){

  WiFiClient client = server.available();

  if (client) {
    char currentLine[64] = {0};
    uint8_t idx = 0;
    while (client.connected()) {
      delayMicroseconds(10);
      if (client.available()) {
        char c = client.read();
        // Serial.write(c);
        if (c == '\n') {

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (idx == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<p>Use the forms below to set values</p>");
            client.print("<br>");

            // Show a list of forms 
            FormUpdatable::listForms(&client);

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            // char buf[101];
            // currentLine.toCharArray(buf, 100);
            FormUpdatable::parse(currentLine);
            currentLine[0] = 0;
            idx = 0;
          }
        } else if (c != '\r') {
          if (idx < 63) {
            currentLine[idx] = c;
            currentLine[++idx] = 0;
          }
        }
      }
    }
    // close the connection:
    client.stop();
  }
}


void startWiFi() {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  if (WiFi.firmwareVersion() < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  WiFi.config(ipAddress);

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);  // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
