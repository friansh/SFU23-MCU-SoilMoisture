/*
 *    frianshFW Configuration File.
 */

#define CONFIG_RESET_BUTTON_PRESS_TIME            10000UL
#define CONFIG_FIRMWARE_UPDATE_CHECK_INTERVAL     600000UL
#define CONFIG_DEVICE_HEARTBEAT_INTERVAL          300000UL

#define SENSOR_READ_INTERVAL                      2000UL

#define CONFIG_RESET_PIN    14  // D5 (must be pulled up)

#define MUX_S0    15   // D8
#define MUX_S1    13   // D7

#define MUX_Z     A0

const char* soilMoistureDatasetLocation1 = "63845df6e303f4563a77463a";
const char* soilMoistureDatasetLocation2 = "63845e26e303f4563a77463b";
const char* soilMoistureDatasetLocation3 = "63845e3de303f4563a77463c";  
