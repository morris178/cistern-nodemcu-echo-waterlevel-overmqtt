#define WIFI_SSID "<SSID>"       //enter SSID and pass without <>
#define WIFI_PASS "<Pass>"

#define MQTT_SERVER_IP "192.168.X.X"
#define MQTT_SERVER_PORT 1883

#define MQTT_CLIENT_NAME "cistern-sensor"
#define MQTT_SERVER_USER_ID ""      // leave quotes empty, if no authentication is required
#define MQTT_SERVER_PASS ""

#define MQTT_TOPIC_NAME_LEVEL_PERCENT "cistern/level/percent"
#define MQTT_TOPIC_NAME_LEVEL_LITERS "cistern/level/liters"
#define MQTT_TOPIC_NAME_LEVEL_CM "cistern/level/cm"

#define SENSOR_DISTANCE_TO_MAX_VOLUME 21        // in cm - minimum is 21

#define TIME_PERIOD_BETWEEN_READINGS 60   // in seconds

#define CISTERN_HEIGHT 150      // in cm
#define CISTERN_LENGTH 225      // in cm




/** ===============ADVANCED SETTINGS==================== **/

#define TRIGGER_PULSE_WIDTH 15      // in microseconds - adjust this setting if your sensor doesn't send the pulse

#define READING_TIMEOUT 60      // in milliseconds - max is 60ms - post-pulse time period after which the MCU gives up waiting for a return signal
