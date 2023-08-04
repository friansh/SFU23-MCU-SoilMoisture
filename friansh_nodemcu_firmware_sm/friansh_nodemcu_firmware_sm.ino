#define FIRMWARE_VERSION  "0.1.0"
#define BOARD             "ESP8266"
#define HOTSPOT_NAME      "SoilMoist"

#include "fsh_fw_lib.h"
frianshFW frianshfw;

int max_value[3] = {0, 0, 0};     // Kondisi tanah basah
int min_value[3] = {1023, 1023, 1023};     // Kondisi tanah kering
int zero_value[3] = {50, 50, 50};       // Kondisi sensor tidak tertancap

void action(char* topic, char* receivedData) {

}

frianshFW_scheduler taskOne;

void subscriptionList() {

}

void setup() {
  Serial.begin(115200);
  delay(100);
  frianshfw.begin();

  // Pin Assignments
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_Z, INPUT);
}

void loop() {
  frianshfw.run();
  taskOne.run(SENSOR_READ_INTERVAL, [] {
    // Read soil moisture 1
    digitalWrite(MUX_S0, LOW); digitalWrite(MUX_S1, LOW);
    float SoilMoisture1Value = analogRead(MUX_Z);
    delay(100);

    // Read soil moisture 2
    digitalWrite(MUX_S0, HIGH); digitalWrite(MUX_S1, LOW);
    float SoilMoisture2Value = analogRead(MUX_Z);
    delay(100);

    // Read soil moisture 3
    digitalWrite(MUX_S0, !HIGH); digitalWrite(MUX_S1, HIGH);
    float SoilMoisture3Value = analogRead(MUX_Z);
    delay(100);
    
    Serial.println("  Soil moisture raw readings: ");

    frianshFW_helpers::printRawReading(1, SoilMoisture1Value);
    frianshFW_helpers::printRawReading(2, SoilMoisture2Value);
    frianshFW_helpers::printRawReading(3, SoilMoisture3Value);

    SoilMoisture1Value = frianshFW_helpers::convertBitToHumid(SoilMoisture1Value, min_value[0], max_value[0], zero_value[0]);
    SoilMoisture2Value = frianshFW_helpers::convertBitToHumid(SoilMoisture2Value, min_value[1], max_value[1], zero_value[1]);
    SoilMoisture3Value = frianshFW_helpers::convertBitToHumid(SoilMoisture3Value, min_value[2], max_value[2], zero_value[2]);

    Serial.println("  Soil moisture in percent: ");
    
    frianshFW_helpers::printConverted(1, SoilMoisture1Value);
    frianshFW_helpers::printConverted(2, SoilMoisture2Value);
    frianshFW_helpers::printConverted(3, SoilMoisture3Value);
        
    // Publish data
    frianshfw.publishFloat(soilMoistureDatasetLocation1, SoilMoisture1Value);
    frianshfw.publishFloat(soilMoistureDatasetLocation2, SoilMoisture2Value);
    frianshfw.publishFloat(soilMoistureDatasetLocation3, SoilMoisture3Value);
  });
}