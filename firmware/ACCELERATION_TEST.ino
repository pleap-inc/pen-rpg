#include <Wire.h>
#include "M5Atom.h"
//#include <M5StickC.h>
#include <WiFiClientSecure.h>
#include <ssl_client.h>
#include <ArduinoJson.h>

#define ADXL345_I2C_ADRESS  0x53    //ADXL345 I2C address
#define ADXL345_DEV_ID  0xE5    //ADXL345 Device ID Number
#define ADXL345_SCALE_FACTOR  0.0039  //ADXL345 Scale Factor at ±2g range 
#define ADXL345_DEVID 0x00        //ADXL345 Device ID Address
#define ADXL345_DATAX0  0x32        //ADXL345 X-Axis Data0 Address
#define ADXL345_POWER_CTL 0x2D        //ADXL345Power Management Adress
#define SAMPLING_RATE 50
#define SAMPLING_DATA_SIZE 300

const char* ssid = "Buffalo-G-C7DA";
const char* password = "vjdridmuhhaas";

const char* server = "pen-rpg-api.herokuapp.com";

uint32_t WebPost_LastTime = 0;
const int click_button = 23;

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


void adxl345_set_data(uint8_t ucReg, uint8_t ucData);                   
void adxl345_get_data(uint8_t ucReg, uint8_t *ucData, uint8_t ucSize); 
void adxl345_get_axi(float *pfData);

int counter_x_hold=0;
int counter_x=0;
int counter_y=0;

bool is_rotation;
bool is_click;
bool is_hold;

int total_click = 0;

WiFiClientSecure client;

void setup() {
  Serial.begin(115200);    
  Wire.begin(26, 32);   
  //Wire.begin(32, 33);        
  Wire.setClock(400000L);
  pinMode(click_button, INPUT_PULLUP);
  /*  
  M5.Axp.ScreenBreath(10);    
  M5.Lcd.setRotation(3);      
  M5.Lcd.setTextSize(2);     
  M5.Lcd.fillScreen(BLACK); 
  */
  uint8_t ucData;                  
  adxl345_get_data(ADXL345_DEVID, &ucData, 1); //デバイスＩＤ取得
  if(ucData !=  ADXL345_DEV_ID){
    Serial.println(F("I2C connection fail.Try again."));
  }         
  connectToWiFi();
  //ADXL345を測定モードへ変更  
  adxl345_set_data(ADXL345_POWER_CTL, 0x08);   
  delay(100);
  
  WebPost_LastTime = 200000;
}

void loop() {

  float x_hold_array[SAMPLING_RATE];
  float y_array[SAMPLING_DATA_SIZE];
  float x_array[SAMPLING_DATA_SIZE];

  if ((millis() - WebPost_LastTime) > 10000) {
    postData(is_rotation, total_click, is_hold);  //Post data
    is_rotation = false;
    WebPost_LastTime = millis();  //store current time in WebGet_LastTime
  }
  
  float axiData[3];
  //加速度データ取得
  adxl345_get_axi(axiData);
  //加速度データの表示
  /*
  Serial.print(" X[g]:");Serial.print(axiData[0]);Serial.print(",");
  Serial.print(" Y[g]:");Serial.print(axiData[1]);Serial.print(",");
  Serial.print(" Z[g]:");Serial.print(axiData[2]);
  Serial.println();
  */
  
  y_array[counter_y] = axiData[1];
  x_hold_array[counter_x_hold] = axiData[0];
  x_array[counter_x] = axiData[0];
  
  if(counter_x_hold > SAMPLING_RATE) {
    Serial.println(maxElement(x_hold_array, SAMPLING_RATE));
    if(maxElement(x_hold_array, SAMPLING_RATE) > 0.2) {
      Serial.println("writing");
      is_hold=true;
    } else {
      Serial.println("resting");
      is_hold=false; 
    }
    
    for(int i=0;i<SAMPLING_RATE;i++) {
      x_hold_array[i] = 0;
    }
    counter_x_hold=0;
  }
  
  if(counter_x > SAMPLING_DATA_SIZE && counter_y > SAMPLING_DATA_SIZE) {
    /*
    Serial.print("x: ");
    for(int index=0;index<SAMPLING_DATA_SIZE;index++) {
      Serial.print(x_array[index]);
      Serial.print(" ");
    }
    for(int index=0;index<SAMPLING_DATA_SIZE;index++) {
      Serial.print(y_array[index]);
      Serial.print(" ");
    }
    Serial.println();
    */
    Serial.print("y_max: ");
    Serial.println(maxElement(y_array, SAMPLING_DATA_SIZE));
    for(int index=0;index<SAMPLING_DATA_SIZE;index++) {
      if(x_array[index]==y_array[index]) {
        for(int i=2; i<51; i++) {
          if(x_array[index+i]==y_array[index+i] && maxElement(y_array, SAMPLING_DATA_SIZE) < float(2)) {
            Serial.println("rotation detected!");
            is_rotation = true; 
            break;
          }
        }
      }
    }
    /***Zero clear*****/
    for(int i=0;i<SAMPLING_DATA_SIZE;i++) {
      x_array[i] = 0;
    }
    for(int i=0;i<SAMPLING_DATA_SIZE;i++) {
      y_array[i] = 0;
    }
    /***Zero clear*****/
    counter_x=0;
    counter_y=0;
  }
    
  counter_x_hold++;
  counter_x++;
  counter_y++;

  if(digitalRead(click_button)==true) {
    total_click++;
  }
  delay(10);
}


void adxl345_set_data(uint8_t ucReg, uint8_t ucData){
  Wire.beginTransmission(ADXL345_I2C_ADRESS); 
  Wire.write(ucReg);
  Wire.write(ucData);
  Wire.endTransmission(true); 
}

void adxl345_get_data(uint8_t ucReg, uint8_t *ucData, uint8_t ucSize){
  uint8_t cnt = 0;
  Wire.beginTransmission(ADXL345_I2C_ADRESS); 
  Wire.write(ucReg);    
  Wire.endTransmission(false); 
  Wire.requestFrom(ADXL345_I2C_ADRESS, ucSize, true);
  while(Wire.available()){
    ucData[cnt] = Wire.read();
    cnt++;   
  }
}

float maxElement(const float* array, size_t size) {

  float max = array[0];
  for(size_t i=0; i<size; i++) {
    if(max < array[i]) {
      max = array[i]; 
    }
  }

  return max;
}

float minElement(const float* array, size_t size) {

  float min = array[0];
  for(size_t i=0; i<size; i++) {
    if(min > array[i]) {
      min = array[i]; 
  }
  return min;
  }
}

/*ADXL345 X,Y,Zデータ取得関数*/
void adxl345_get_axi(float *pfData){
  uint8_t ucBuf[6];
  int16_t acc[3];
  adxl345_get_data(ADXL345_DATAX0, ucBuf, 6);
  acc[0] =((int16_t)ucBuf[1] << 8) | ucBuf[0];  
  acc[1] =((int16_t)ucBuf[3] << 8) | ucBuf[2];
  acc[2] =((int16_t)ucBuf[5] << 8) | ucBuf[4];
  pfData[0] = acc[0] * ADXL345_SCALE_FACTOR;
  pfData[1] = acc[1] * ADXL345_SCALE_FACTOR;
  pfData[2] = acc[2] * ADXL345_SCALE_FACTOR;
}

void postData(bool rotation, int total_click, bool hold) {
   DynamicJsonDocument doc(1024);

  doc["rotation"] = rotation;
  doc["click"]   = total_click;
  doc["hold"] = hold;

  String state;
  serializeJson(doc, state);
  Serial.println(state);
  
  client.setCACert(root_ca);
  Serial.println("\nStarting connection to server...");
  delay(100);
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
}
