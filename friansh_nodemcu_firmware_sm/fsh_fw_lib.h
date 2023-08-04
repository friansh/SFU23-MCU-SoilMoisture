#include <ESP8266WiFi.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266WebServer.h> 
#include <WiFiClientSecureBearSSL.h>

#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>
#include <PubSubClient.h>
#include <time.h>

#include "fsh_fw_config.h"
#include "fsh_fw_variables.h"
#include "fsh_fw_scheduler.h"

#include "fsh_fw_fs.h"
frianshFW_FS frianshfw_fs;

#include "fsh_fw_helpers.h"

void action(char* topic, char* receivedData);

#include "fsh_fw_setup.h"
frianshFW_setup frianshfw_setup;

#include "fsh_fw_https.h"
frianshFW_HTTPS frianshfw_https;

void subscriptionList();

#include "fsh_fw_mqtts.h"
frianshFW_MQTTS frianshfw_mqtts;

class frianshFW {
  public:
    void begin() {
      printBanner();

      pinMode(CONFIG_RESET_PIN, INPUT_PULLUP);
      pinMode(LED_BUILTIN, OUTPUT);
      
      frianshfw_fs.mount();
      
      if ( digitalRead(CONFIG_RESET_PIN) == LOW ) {
        Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
        Serial.println(F("!! Entering the reset state in 10 seconds... !!"));
        Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));

        unsigned long resetButtonPressed = 0;
        while( digitalRead(CONFIG_RESET_PIN) == LOW ){
          resetButtonPressed = millis();
          if ( resetButtonPressed >= CONFIG_RESET_BUTTON_PRESS_TIME ) {
            setup();
            return;
          }
          delay(1);
        }
        
        Serial.println(F("  Device reset cancelled!"));
      }

      if ( !frianshfw_fs.loadConfig() ) restartESP();
      printStatus();
      
//      char* HOSTNAME = (char*) malloc(100* sizeof(char));
//      snprintf(HOSTNAME, 100, "fsh-%sv%s", HOTSPOT_NAME, FIRMWARE_VERSION);
//      WiFi.hostname(strdup(HOSTNAME));
      
      connectWiFi();
      if ( frianshfw_https.firmwareUpdateCheck() ) frianshfw_https.firmwareUpdate();
      randomSeed(micros());
      frianshfw_mqtts.begin();
    }

    void setup() {
      frianshfw_setup.begin();
      SETUP_STATE = true;
    }

    void run() {
      if ( SETUP_STATE ) {
        frianshfw_setup.run();
        return;  
      }
      
      if (WiFi.status() != WL_CONNECTED) {
        connectWiFi();
        return;
      }
      
      unsigned long currentMillis = millis();
      if ( currentMillis - blinkSmoothLastRun > 2 ) {
        blinkSmooth();
        blinkSmoothLastRun = currentMillis;
      }
      
      if ( currentMillis - firmwareUpdateCheckLastRun > CONFIG_FIRMWARE_UPDATE_CHECK_INTERVAL ) {
        if ( frianshfw_https.firmwareUpdateCheck() ) frianshfw_https.firmwareUpdate();
        firmwareUpdateCheckLastRun = currentMillis;
      }

      frianshfw_mqtts.run();
    }

    bool publishFloat(const char *topic, float data) {
      return frianshfw_mqtts.publishFloat(topic, data);
    }

    void subscribe(void (*subscriptionList)()) {
      if ( SETUP_STATE ) return;
      subscribeFunction = (*subscriptionList);
      (*subscribeFunction)();
    }

    bool addControl(const char* control_id) {
      if ( SETUP_STATE ) return false;
      char *topic = (char*) malloc(150 * sizeof(char));
      snprintf(topic, 100, "friansh/control/%s/%s", frianshfw_fs.getDeviceToken(), control_id);
      if ( frianshfw_mqtts.subscribe(topic) ) {
        free(topic);
        return true;
      }
      free(topic);
      return false;
    }

//    void onMessage(void (*action)()) {
//      frianshfw_mqtts.setCallback( &(*action) );
//    }
  private:
    bool brightness_increment = true;
    unsigned int brightness = 0;

    unsigned long blinkSmoothLastRun = 0;
    unsigned long firmwareUpdateCheckLastRun = 0;

    void (*subscribeFunction)();

    bool SETUP_STATE = false;
    
    void printBanner() {
      Serial.print("\n\n\n\n\n");
      Serial.println(F("-----------------------------------------------"));
      Serial.println(F("         __friansh NodeMCU Firmware__          "));
      Serial.println(F("-----------------------------------------------"));
    }
    
    void printStatus() {
      Serial.println(F("-----------------------------------------------"));
      Serial.println(F("|              Board Configuration            |"));
      Serial.println(F("-----------------------------------------------"));
      Serial.println();
      Serial.print(F ("  Firmware version   : "));
      Serial.println(F(FIRMWARE_VERSION));
      Serial.printf  ("  Cloud host         : %s\n", frianshfw_fs.getCloudHost());
      Serial.printf  ("  Cloud HTTPS port   : %d\n", frianshfw_fs.getCloudHttpsPort());
      Serial.printf  ("  Cloud MQTT port    : %d\n", frianshfw_fs.getCloudMqttsPort());
      Serial.printf  ("  WiFi SSID          : %s\n", frianshfw_fs.getWifiSsid());
      Serial.printf  ("  WiFi password      : %s\n", frianshfw_fs.getWifiPassword());
      Serial.printf  ("  Device token       : %s\n", frianshfw_fs.getDeviceToken());
      Serial.printf  ("  Chip MAC address   : %s\n", WiFi.macAddress().c_str());
      Serial.printf  ("  Chip ID            : %d\n", ESP.getChipId());
      Serial.printf  ("  Chip frequency     : %d MHz\n", ESP.getCpuFreqMHz());
      Serial.printf  ("  Chip core version  : %s\n", ESP.getCoreVersion());
      Serial.printf  ("  Device hostname    : %s\n", WiFi.hostname().c_str());
      Serial.println();
      Serial.println("-----------------------------------------------");
      Serial.println("|  Copyright Fikri Rida Pebriansyah (C) 2021  |");
      Serial.println("-----------------------------------------------");
      Serial.println();
    }

    void connectWiFi() {
      const char *ssid = frianshfw_fs.getWifiSsid();
      const char *pass = frianshfw_fs.getWifiPassword();

      WiFi.disconnect();
      delay(1000);
      Serial.print("  WiFi connecting...");
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, pass);

      bool LED_STATE = false;
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        digitalWrite(LED_BUILTIN, !LED_STATE);
        LED_STATE = !LED_STATE;
      }

      delay(1000);
      Serial.print(" connected. IP address: ");
      Serial.println(WiFi.localIP());
    }
    
    void blinkSmooth() {
      if (brightness_increment) brightness++;
      else brightness--;
    
      if ( brightness == 255 ) brightness_increment = false;
      else if ( brightness == 0 ) brightness_increment = true;
      
      analogWrite(LED_BUILTIN, brightness);  
    }

    static void restartESP() {
      Serial.println("  Restarting in 10 secs...");
      delay(10000);
      ESP.restart();
    }
};
