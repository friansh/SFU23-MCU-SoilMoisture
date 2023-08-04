class frianshFW_setup {
  public:
    void begin() {
      char* AP_SSID = new char[100];
      sprintf(AP_SSID, "fsh-%sv%s", HOTSPOT_NAME, FIRMWARE_VERSION);
      
      Serial.println("  Configuring access point...");
      WiFi.softAP(AP_SSID, "");
      IPAddress myIP = WiFi.softAPIP();

      server = new ESP8266WebServer(80);
      server->on("/", [&] {
        server->send(200, "text/html", configHTML);
      });

      server->on("/save", [&] {
          if (server->method() != HTTP_POST) server->send(405, "text/plain", "Method Not Allowed");
          else {
            const char*   cloud_host        = server->arg(0).c_str();
            unsigned int  cloud_https_port  = atoi(server->arg(1).c_str());
            unsigned int  cloud_mqtts_port  = atoi(server->arg(2).c_str());
            const char*   wifi_ssid         = server->arg(3).c_str();
            const char*   wifi_password     = server->arg(4).c_str();
            const char*   device_token      = server->arg(5).c_str();

            Serial.println(F("-----------------------------------------------"));
            Serial.println(F("|               New Configuration             |"));
            Serial.println(F("-----------------------------------------------"));
            Serial.printf   (" _Cloud host        : %s\n", cloud_host);
            Serial.printf   (" _Cloud https port  : %d\n", cloud_https_port);
            Serial.printf   (" _Cloud mqtts port  : %d\n", cloud_mqtts_port);
            Serial.printf   (" _WiFi SSID         : %s\n", wifi_ssid);
            Serial.printf   (" _WiFi Password     : %s\n", wifi_password);
            Serial.printf   (" _Device token      : %s\n", device_token);
            Serial.println(F("-----------------------------------------------"));

            bool saveConfig = frianshfw_fs.saveConfig(
              cloud_host,
              cloud_https_port,
              cloud_mqtts_port,
              wifi_ssid,
              wifi_password,
              device_token
              );

            if (saveConfig) {
              server->send(200, "text/plain", "The configuration has been saved, restarting board in 5 secs...");
              delay(5000);
              ESP.restart();
            }
              else server->send(500, "text/plain", "Error saving the config!");
          }
      });
      
      server->begin();
      Serial.println("  HTTP server started");
      Serial.printf ("  Please connect to the WiFi SSID \"%s\"\n", AP_SSID);
      Serial.print  ("  and then go to http://");
      Serial.print  (myIP);
      Serial.println(" on your browser.");
    }

    void run() {
      server->handleClient();
      if (millis() - lastBlink >= 125UL) {
        digitalWrite(LED_BUILTIN, !ledState);
        ledState = !ledState;
        lastBlink = millis();
      }
    }
    
  private:
    ESP8266WebServer *server;
    bool ledState = false;
    unsigned long lastBlink = 0;
};
