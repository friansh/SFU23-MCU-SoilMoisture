class frianshFW_HTTPS {
  public:
    bool firmwareUpdateCheck() {
      Serial.println("  Checking for firmware update...");

      char *versionUri = (char*) malloc(200 * sizeof(char));
      snprintf(versionUri, 200, "http://%s:%d/firmware/%s/version", frianshfw_fs.getCloudHost(), frianshfw_fs.getCloudHttpsPort(), frianshfw_fs.getDeviceToken());
        
      if (http.begin(client, versionUri)) {
        free(versionUri);
        int httpCode = http.GET();
        if (httpCode > 0) {
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            char* updateStatus = strdup(http.getString().c_str());

            StaticJsonDocument<256> doc;
            DeserializationError error = deserializeJson(doc, updateStatus);
            if (error) {
              Serial.println("  Failed to parse update status!");
              return false;
            }

            const char* fwVersion = doc["version"];
            const char* board = doc["board"];
            const char* update_notes = doc["update_notes"];
            
            Serial.println("  Gathered firmware status from the update server.");
            Serial.printf ("  - Version\t\t: %s\n", fwVersion);
            Serial.printf ("  - Board\t\t: %s\n", board);
            Serial.printf ("  - Update notes\t: %s\n", update_notes);

            if ( strcmp(board, BOARD) == 0 && strcmp(fwVersion, FIRMWARE_VERSION) != 0 ) {
              Serial.println();
              Serial.println(F("  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
              Serial.println(F("  !!! This board firmware version mismatch with the server !!!"));
              Serial.println(F("  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
              return true;
            }
          }
        } else {
          Serial.println("  Failed to fetch data from the update server!");
          Serial.printf ("  - HTTP error\t: %s\n", http.errorToString(httpCode).c_str());
//          char* lastSSLError = (char*) malloc(100 * sizeof(char));
//          client.getLastSSLError(lastSSLError, 100);
//          Serial.printf ("  - SSL error\t: %s\n", lastSSLError);
//          free(lastSSLError);
        }
        http.end();
      } else {
        free(versionUri);
        Serial.println("  Unable to connect to the update server!");
      }
      return false;
    }

    void firmwareUpdate() {
      ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW);

      ESPhttpUpdate.onStart([] {
        Serial.println(F("-----------------------------------------------"));
        Serial.println(F("|                Firmware Update              |"));
        Serial.println(F("-----------------------------------------------"));
        Serial.println(F("  HTTP update process started..."));
      });
      ESPhttpUpdate.onEnd([] {
        Serial.println(F("  HTTP update process finished."));
      });
      ESPhttpUpdate.onProgress([](int cur, int total) {
        Serial.printf_P(PSTR("  HTTP update process at %d of %d bytes...\n"), cur, total);
      });
      ESPhttpUpdate.onError([](int err) {
        Serial.printf_P(PSTR("  UPDATE:  HTTP update fatal error code %d\n"), err);
      });

      Serial.println("  Downloading update...");

      char *downloadUri = (char*) malloc(200 * sizeof(char));
      snprintf(downloadUri, 200, "http://%s:%d/firmware/%s/download", frianshfw_fs.getCloudHost(), frianshfw_fs.getCloudHttpsPort(), frianshfw_fs.getDeviceToken());
      t_httpUpdate_return ret = ESPhttpUpdate.update(client, downloadUri);
      free(downloadUri);
      
      switch (ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("  Checking for update failed. %s\n", ESPhttpUpdate.getLastErrorString().c_str());
          break;
  
        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("  No update detected.");
          break;
  
        case HTTP_UPDATE_OK:
          Serial.println("  HTTP_UPDATE_OK");
          break;
      }
    }
    
  private:
//    BearSSL::WiFiClientSecure client;
//    BearSSL::PublicKey *key;
    WiFiClient client;
    HTTPClient http;
};
