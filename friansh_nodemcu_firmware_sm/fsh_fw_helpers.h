class frianshFW_helpers {
  public:
    static bool checkControlId(char* receivedControlId, const char* supposedControlId) {
      char* fullSupposedControlId = (char*) malloc(100 * sizeof(char));
      snprintf(fullSupposedControlId, 100, "friansh/control/%s/%s", frianshfw_fs.getDeviceToken(), supposedControlId);
      if ( strcmp(receivedControlId, fullSupposedControlId) == 0 ) {
        free(fullSupposedControlId);
        return true;
      }
      free(fullSupposedControlId);
      return false;
    }

    static bool controlIsTrue(char* receivedControlValue) {
      if ( strcmp(receivedControlValue, "1") == 0 ) return true;
      return false;
    }

    static float convertBitToHumid(float soilmoisture, int min_val, int max_val, int zero_value) {
      if (soilmoisture < zero_value)
        soilmoisture = 0;
      else if (soilmoisture == 1024)
        soilmoisture = 100;
      else
        soilmoisture = (soilmoisture - min_val) / (max_val - min_val) * 100;
        // soilmoisture = (max_val - soilmoisture)/(max_val - min_val) * 100;

      soilmoisture = float( long(soilmoisture * 10) ) / 10.0;

      if (soilmoisture > 100)
        soilmoisture = 100;
      else if (soilmoisture < 0)
        soilmoisture = 0;

      return soilmoisture;
  }

  static void printRawReading(unsigned int n, float value) {
    Serial.print("  - Soil moisture ");
    Serial.print(n);
    Serial.print(": ");
    Serial.print(value);
    Serial.print(" (");
    Serial.print(value / 1023 * 3.3);
    Serial.println(" V)");
  }

  static void printConverted(unsigned int n, float value) {
    Serial.print("  - Soil moisture ");
    Serial.print(n);
    Serial.print(": ");
    Serial.print(value);
    Serial.println("%");
  }
};
