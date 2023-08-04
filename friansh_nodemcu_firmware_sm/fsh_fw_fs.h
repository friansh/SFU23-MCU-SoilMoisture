class frianshFW_FS {
  public:
    void mount() {
      Serial.print("  Mounting file system...");
      if (!LittleFS.begin()) { Serial.println("  failed"); return; }
      Serial.println("  mounted");
    }
    
    bool loadConfig() {
      Serial.print("  Loading configuration from file...");
      File configFile = LittleFS.open("/config.json", "r");
      if (!configFile) {
        formatFS();
        return false;
      }
    
      size_t size = configFile.size();
      if (size > 1024) {
        Serial.println("  failed, the config file size is too large!");
        return false;
      }

      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
    
      // We don't use String here because ArduinoJson library requires the input
      // buffer to be mutable. If you don't use ArduinoJson, you may as well
      // use configFile.readString instead.
      configFile.readBytes(buf.get(), size);

      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, buf.get());
      if (error) {
        Serial.println("  failed to parse config file!");
        return false;
      }
      
      CLOUD_HOST = strdup(doc["cloud_host"]);
      WIFI_SSID = strdup(doc["wifi_ssid"]);
      WIFI_PASSWORD = strdup(doc["wifi_password"]);
      DEVICE_TOKEN = strdup(doc["device_token"]);

      CLOUD_HTTPS_PORT = doc["cloud_https_port"];
      CLOUD_MQTTS_PORT = doc["cloud_mqtts_port"];
      
      Serial.println(" loaded.");
      return true;
    }

    bool saveConfig(const char *cloud_host, unsigned int cloud_https_port, unsigned int cloud_mqtts_port, const char *wifi_ssid, const char *wifi_password, const char *device_token) {
      StaticJsonDocument<512> doc;
      doc["cloud_host"] = cloud_host;
      doc["cloud_https_port"] = cloud_https_port;
      doc["cloud_mqtts_port"] = cloud_mqtts_port;
      doc["wifi_ssid"] = wifi_ssid;
      doc["wifi_password"] = wifi_password;
      doc["device_token"] = device_token;
    
      File configFile = LittleFS.open("/config.json", "w");
      if (!configFile) {
        Serial.println("  Failed to open config file for writing");
        return false;
      }
      
      serializeJson(doc, configFile);
      Serial.println("  Configuration saved.");
      return true;
    }

    void formatFS() {
      Serial.println("  Failed to open config file!");
      Serial.println("  Formatting the file system...");
      LittleFS.format();
      Serial.println("  File system formatted.");
      Serial.println("  Saving the default configuration file...");
      if ( saveConfig("default.server", 443, 8883, "default", "default", "default") )
        Serial.println("  The default configuration file has been saved.");
      else
        Serial.println("  Failed to save the default configuration.");
      Serial.println("  Restarting in 10 secs...");
      delay(10000);
      ESP.restart();
    }
    
    char *getCloudHost() { return CLOUD_HOST; }
    char *getWifiSsid() { return WIFI_SSID; }
    char *getWifiPassword() { return WIFI_PASSWORD; }
    char *getDeviceToken() { return DEVICE_TOKEN; }

    unsigned int getCloudHttpsPort() {
      return CLOUD_HTTPS_PORT;
    }

    unsigned int getCloudMqttsPort() {
      return CLOUD_MQTTS_PORT;
    }    
  private:
    char *CLOUD_HOST = (char*) malloc(100 * sizeof(char));
    char *WIFI_SSID = (char*) malloc(100 * sizeof(char));
    char *WIFI_PASSWORD = (char*) malloc(100 * sizeof(char));
    char *DEVICE_TOKEN = (char*) malloc(100 * sizeof(char));
    unsigned int CLOUD_HTTPS_PORT = 443;
    unsigned int CLOUD_MQTTS_PORT = 8883;
};
