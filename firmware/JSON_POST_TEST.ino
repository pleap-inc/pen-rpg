#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

const char* ssid = "Buffalo-G-C7DA";
const char* password = "vjdridmuhhaas";

const char* server = "pen-rpg-api.herokuapp.com";

const char *root_ca = \
  "-----BEGIN CERTIFICATE-----\n"\
  "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"\
  "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"\
  "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"\
  "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"\
  "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"\
  "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"\
  "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"\
  "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"\
  "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"\
  "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"\
  "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"\
  "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"\
  "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"\
  "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"\
  "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"\
  "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"\
  "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"\
  "rqXRfboQnoZsG4q5WTP468SQvvG5\n"\
  "-----END CERTIFICATE-----\n";


WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  connectToServer();
}

void loop() {

  
}

void connectToWiFi() {
  WiFi.disconnect();
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  for (int i = 0; WiFi.status() != WL_CONNECTED; i++)
  {
    if (i % 10 == 0) {
      WiFi.begin(ssid, password);
    }
    Serial.print(".");
    delay(1000); // wait a second for connection
  }
  Serial.print("Connected to ");
  Serial.println(ssid);

  delay(2000);
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
}


void connectToServer() {
  DynamicJsonDocument doc(1024);

  doc["rotation"] = false;
  doc["click"]   = 100;
  doc["hold"] = false;

  String state;
  serializeJson(doc, state);
  Serial.println(state);
  
  client.setCACert(root_ca);
  Serial.println("\nStarting connection to server...");


  
  if (!client.connect(server, 443)) {
    Serial.println("Failed to connect");
  } else {
    Serial.println("Connected to server!");
    // Make a HTTP request:
    client.print("POST https://pen-rpg-api.herokuapp.com/api/v1/umsDRdmqyRWr6bvYsA1c6BgirgH2/device");
    client.println(" HTTP/1.1");
    client.println("Host: pen-rpg-api.herokuapp.com");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(state.length());
    client.println();
    client.println(state);
    delay(10);
  }
}

/*

void postData() {
  client.setCACert(root_ca);
  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 443)) {
    Serial.println("Failed to connect");
  } else {
    Serial.println("Connected to server!");
    // Make a HTTP request:
    client.print("POST https://morning-stream-75071.herokuapp.com/api/v1/");
    client.print(WiFi.macAddress());
    client.print("/test/");
    client.println(" HTTP/1.0");
    client.println("Host: morning-stream-75071.herokuapp.com");
    client.println("Content-Type: application/json");
    client.print("Content-Length:");
    client.println(domain_name.length());
    client.println("Connection: close");
    client.println();
    client.println(domain_name);  //send domain name
    Serial.print("successfully posted ");
    Serial.println(domain_name);
  }
}


  */
