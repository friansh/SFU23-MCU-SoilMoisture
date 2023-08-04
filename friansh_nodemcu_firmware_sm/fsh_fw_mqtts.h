//BearSSL::X509List inicert(cert);
//BearSSL::PublicKey inipubkeymqtt(mqttPubKey);

class frianshFW_MQTTS {
  public:
    void begin() {
//      espClient.setTrustAnchors(&inicert);
//      espClient.allowSelfSignedCerts();
//      espClient.setKnownKey(&inipubkeymqtt);
//      espClient.setFingerprint(mqttCertFingerprint);

//      configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
//    
//      Serial.print("Waiting for NTP time sync: ");
//      time_t now = time(nullptr);
//      while (now < 8 * 3600 * 2) {
//        delay(500);
//        Serial.print(".");
//        now = time(nullptr);
//      }
//      Serial.println("");
//      struct tm timeinfo;
//      gmtime_r(&now, &timeinfo);
//      Serial.print("Current time: ");
//      Serial.print(asctime(&timeinfo));
//  
//      espClient.setX509Time(now);
      
      client.setClient(espClient);
      client.setServer(frianshfw_fs.getCloudHost(), frianshfw_fs.getCloudMqttsPort());
      client.setCallback([&] (char* topic, byte* payload, unsigned int length) {
        Serial.printf("  Received mqtt data from [%s] length: %d | ", topic, length);
        char *msg_buffer = (char*) malloc(500 * sizeof(char));
       
        for (int i = 0; i < length; i++) {
          Serial.printf("%d ", payload[i]);
          msg_buffer[i] = payload[i];
        }
        msg_buffer[length] = '\0';

        Serial.printf(" %s\n", msg_buffer);
        action(topic, msg_buffer);
        free(msg_buffer);
      });
      mqttConnect();
    }

//    void setCallback( void (*action)() ) {
//      callbackFunction = (*action);
//    }
    
    bool mqttConnect() {
        Serial.print("  Attempting MQTT connection...");
        String clientId = "friansh-";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
          Serial.println(" connected.");

          // !!!!!!!!!!!!!!!!!!!!
          // NANTI GANTI!! 
              subscriptionList();
          // NANTI GANTI!!
          // ^^^^^^^^^^^^^^^^^^^^
      
          return true;
        } else {
          Serial.printf(" failed, rc=%d try again in 5 seconds.\n", client.state());
//          char* lastSSLError = new char[100];
//          espClient.getLastSSLError(lastSSLError, 100);
//          Serial.printf ("  - SSL error\t: %s\n", lastSSLError);
        }
        return false;
    }

    void run() {
      currentMillis = millis();
      
      if (!client.connected()) {
        if ( currentMillis - lastMQTTReconnect >= 10000UL ) {
          lastMQTTReconnect = millis();
          if ( !mqttConnect() ) return;
        }
      }
      
      client.loop();

      if ( currentMillis - lastHeartbeatSent >= CONFIG_DEVICE_HEARTBEAT_INTERVAL ) {
        heartbeat();
        lastHeartbeatSent = millis();
      }
    }

    bool publishFloat(const char* dataset_id, float value) {
      if ( client.connected() ) {
        char *topic = (char*) malloc(150 * sizeof(char));
        snprintf(topic, 150, "friansh/float_data/%s/%s", frianshfw_fs.getDeviceToken(), dataset_id);
        char *valueInChar = (char*) malloc(10 * sizeof(char));
        snprintf(valueInChar, 10, "%.2f", value);
        Serial.printf("  Publishing float value to [%s] with data: %s\n", topic, valueInChar);
        client.publish(topic, valueInChar);
        free(valueInChar);
        free(topic);
        return true;
      }
      return false;
    }

    bool subscribe(char* topic) {
//      if ( client.connected() ) {
        Serial.printf("  Subscribing to topic [%s]...\n", topic);
        client.subscribe(topic);
        return true;
//      }
//      Serial.printf("  Failed to subscribing topic [%s]!\n", topic);
//      return false;
    }

  private:
    const char* mqtt_username = "default";
    const char* mqtt_password = "default";
    const char* heartbeat_topic = "friansh/heartbeat";

//    static void (*callbackFunction)();
    
    unsigned long currentMillis = 0;
    unsigned long lastHeartbeatSent = 0;
    unsigned long lastMQTTReconnect = 0;

    
//    BearSSL::WiFiClientSecure espClient;
    WiFiClient espClient;
    PubSubClient client;

    void heartbeat() {
      if ( client.connected() ) {
        char *token = (char*) malloc(100 * sizeof(char));
        snprintf(token, 100, "{\"device_token\":\"%s\",\"firmware_version\":\"%s\"}", frianshfw_fs.getDeviceToken(), FIRMWARE_VERSION);
        Serial.println("  Sending heartbeat...");
        client.publish(strdup(heartbeat_topic), token);
        free(token);
      }
    }
    
};
