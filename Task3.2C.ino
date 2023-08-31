/*
 * Sunlight Detection & IFTTT Interaction
 * 
 * This Arduino sketch connects to a WiFi network and reads
 * the intensity of sunlight using an LDR sensor. It then
 * sends HTTP requests to IFTTT's webhook service to trigger
 * events when sunlight starts or ends. The code demonstrates
 * how to detect changes in light intensity and interact with
 * external services based on those changes.
 * 
 * Author: Ekamjot Singh
 */
#include <WiFiNINA.h>  // Include the WiFiNINA library for WiFi communication

char ssid[] = "Ekam";      // Set your WiFi network SSID (name)
char pass[] = "ekam12345"; // Set your WiFi network password
const int ldrPin = A0;                                                                                       
WiFiClient client;         // Create a WiFiClient object for communication
char HOST_NAME[] = "maker.ifttt.com";                                                                        // Set the host name for IFTTT
String PATH_NAME_SUNLIGHT = "/trigger/Sunlight-start/with/key/FwQ6apytKQOTRZMJfe-qeDaydaq_kX_uYhpfFY1RsY";   // Set the path for sunlight event
String PATH_NAME_NO_SUNLIGHT = "/trigger/Sunlight-end/with/key/FwQ6apytKQOTRZMJfe-qeDaydaq_kX_uYhpfFY1RsY";  // Set the path for no sunlight event

bool previousSunlightState = false;  // Flag to store the previous sunlight state (assumed dark initially)

void setup() {  

  // Initialize WiFi connection
  WiFi.begin(ssid, pass);

  Serial.begin(9600);  // Start serial communication
  while (!Serial)
    ;

  // Connect to web server on port 80:
  if (client.connect(HOST_NAME, 80)) {  // Attempt to connect to the IFTTT server
    // if connected:
    Serial.println("Connected to server");
  } else {  // if not connected:
    Serial.println("Connection failed");
  }
}

void loop() {
  int ldrValue = analogRead(ldrPin);  // Read analog value from LDR sensor

  // Assuming 0-1023 scale

  Serial.println(ldrValue);  // Print the raw LDR value to the serial monitor

  bool currentSunlightState = ldrValue < 100;  // Determine if it's currently sunlight based on LDR value threshold

  if (currentSunlightState != previousSunlightState) {
    previousSunlightState = currentSunlightState;

    if (currentSunlightState) {
      // Transitioned to sunlight, send HTTP request for sunlight:
      sendHTTPRequest(PATH_NAME_SUNLIGHT);  // Call function to send HTTP request for sunlight event
    } else {
      // Transitioned to no sunlight, send HTTP request for no sunlight:
      sendHTTPRequest(PATH_NAME_NO_SUNLIGHT);  // Call function to send HTTP request for no sunlight event
    }
  }
  delay(1000);  // Delay for 1 second before the next iteration
}

void sendHTTPRequest(String path) {
  if (!client.connected()) {              // Check if not connected to the server
    if (client.connect(HOST_NAME, 80)) {  // Attempt to reconnect
      Serial.println("Reconnected to server");
    } else {
      Serial.println("Connection retry failed");
      return;  // Exit the function if reconnection fails
    }
  }
  if (client.connected()) {
    client.println("GET " + path + " HTTP/1.1");   // Send HTTP GET request
    client.println("Host: " + String(HOST_NAME));  // Include host information
    client.println("Connection: close");           // Close the connection after the request
    client.println();                              // End HTTP header
 
    while (client.connected() && !client.available()) {
      // Waiting for response from server
    }

    while (client.available()) {
      char c = client.read();  // Read response character by character
      Serial.write(c);         // Print response to serial monitor
    }

    Serial.println();  // Print a blank line
    Serial.println("Disconnected");
  } else {
    Serial.println("Client not connected.");
  }
}
