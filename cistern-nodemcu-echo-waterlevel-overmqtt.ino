#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "config.h"
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include "helper.h"
#include <vector>

using namespace std;

#define SERIAL_BAUDRATE                 115200

const double pi = 3.14159265359;
const double r = CISTERN_HEIGHT / 2.0;

double maxVolume = 0.0;


WiFiClient wiFiClient;
PubSubClient client(wiFiClient);

byte clientStatus, prevClientStatus = 99;


// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    
    setupOTA();
}

void setupOTA(){
    ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}

void setMaxVolume(){
  maxVolume = getVolume(getSurface(CISTERN_HEIGHT));
}

void setup() {

    pinMode(TRIGGER_PIN, OUTPUT); // Sets the trigPin as an Output
    pinMode(ECHO_PIN, INPUT); // Sets the echoPin as an Input

    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    // Wifi
    wifiSetup();
    
    // MQTT
    client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
    digitalWrite(TRIGGER_PIN, LOW);
    delay(2);

    setMaxVolume();
}


void ensureMQTTConnection (){
  if (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect

    if (MQTT_SERVER_USER_ID != "" && MQTT_SERVER_PASS != ""){
      client.connect(MQTT_CLIENT_NAME, MQTT_SERVER_USER_ID, MQTT_SERVER_PASS);
      }
    else (client.connect(MQTT_CLIENT_NAME));
    
    if(client.connected()){
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

int getDistanceFromSensor(){
  long duration;
  int distance;
    
    
   // Sets the TRIGGER_PIN on HIGH state for 15 micro seconds
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(TRIGGER_PULSE_WIDTH);
    digitalWrite(TRIGGER_PIN, LOW);
    
    // Reads the ECHO_PIN, returns the sound wave travel time in microseconds
    duration = pulseIn(ECHO_PIN, HIGH,READING_TIMEOUT*1000);
    
    // Calculating the distance
    distance= duration*0.034/2 + SENSOR_OFFSET;


    return distance;
   }
   
vector<int> getWaterlevel(){
  size_t size = SAMPLE_SIZE;
  vector<int> readings(size);
  
  for(int i = 0; i<size;i++){
    int waterlevel = getDistanceFromSensor();
    while (waterlevel  < SENSOR_DISTANCE_TO_MAX_VOLUME || waterlevel > CISTERN_HEIGHT + SENSOR_DISTANCE_TO_MAX_VOLUME +2 ){
      Serial.println("Wrong Reading... repeating...");
      delay(300);
      waterlevel = getDistanceFromSensor();
    }
    readings[i]= waterlevel - SENSOR_DISTANCE_TO_MAX_VOLUME;
    delay(TIME_BETWEEN_READINGS);
  }
  return readings;
}


void loop() {   
    ensureMQTTConnection();
    static unsigned long last = millis();
    if (millis() - last >= IDLE_TIME *1000) {
        last = millis();
        int waterlevel = filteredResult(getWaterlevel());
        char cdist[16];
        itoa(waterlevel, cdist, 10);
        client.publish(MQTT_TOPIC_NAME_LEVEL_CM, cdist);
    
        int volume = getVolume(getSurface(waterlevel));
     
        int level = getLevel(volume, maxVolume);
        char cFul[16];
        itoa(level, cFul, 10);
        client.publish(MQTT_TOPIC_NAME_LEVEL_LITERS, cFul);
    
        double percent = levelPercent(level, maxVolume);
        char cPro[16];
        itoa((int)percent, cPro, 10);
        client.publish(MQTT_TOPIC_NAME_LEVEL_PERCENT, cPro);
    }
    client.loop();
    ArduinoOTA.handle();
    delay(1000);
}
