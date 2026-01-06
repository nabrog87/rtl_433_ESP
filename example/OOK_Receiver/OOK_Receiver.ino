  #include <WiFi.h>
  #include <PubSubClient.h>
  #include "secrets.h"
  #include <ArduinoJson.h>
  #include <ArduinoLog.h>
  #include <rtl_433_ESP.h>

  #ifndef RF_MODULE_FREQUENCY
    #define RF_MODULE_FREQUENCY 868.35
  #endif

  #define JSON_MSG_BUFFER 512
  char messageBuffer[JSON_MSG_BUFFER];

  rtl_433_ESP rf;

  WiFiClient espClient;
  PubSubClient mqttClient(espClient);
  char lastMsg[512];

  int count = 0;


  volatile bool mqttPending = false;
  char lastTopic[64];

  unsigned long lastMqttTry = 0; 


  void reconnectMQTT() {
    if (mqttClient.connected()) return;
    if (millis() - lastMqttTry < 2000) return;
    lastMqttTry = millis();

    Serial.print("MQTT connecting to ");
    Serial.print(MQTT_HOST);
    Serial.print(":");
    Serial.println(MQTT_PORT);

bool ok = mqttClient.connect("rtl433-vevor", MQTT_USER, MQTT_PASS,
                             MQTT_TOPIC_STAT, 0, true, "offline");
if (ok) mqttClient.publish(MQTT_TOPIC_STAT, "online", true);

 mqttClient.publish("rtl_433/vevor/boot", "1", true);
 
 mqttClient.publish(MQTT_TOPIC_STAT, "online", true); // retained
  }

  void rtl_433_Callback(char * message) {
    StaticJsonDocument<768> doc;
    if (deserializeJson(doc, message)) return;

    const char* model = doc["model"] | "";
    const char* protocol = doc["protocol"] | "";

    bool isBad = (!strcmp(model, "undecoded signal") || !strcmp(protocol, "signal parsing failed"));
    const char* topic = isBad ? MQTT_TOPIC_RAW : MQTT_TOPIC_OK;

    strncpy(lastMsg, message, sizeof(lastMsg) - 1);
    lastMsg[sizeof(lastMsg) - 1] = '\0';
    strncpy(lastTopic, topic, sizeof(lastTopic) - 1);
    lastTopic[sizeof(lastTopic) - 1] = '\0';

    mqttPending = true;
  }

  void logJson(JsonDocument jsondata) {
  #if defined(ESP8266) || defined(ESP32) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
    char JSONmessageBuffer[measureJson(jsondata) + 1];
    serializeJson(jsondata, JSONmessageBuffer, measureJson(jsondata) + 1);
  #else
    char JSONmessageBuffer[JSON_MSG_BUFFER];
    serializeJson(jsondata, JSONmessageBuffer, JSON_MSG_BUFFER);
  #endif
  #if defined(setBitrate) || defined(setFreqDev) || defined(setRxBW)
    Log.setShowLevel(false);
    Log.notice(F("."));
    Log.setShowLevel(true);
  #else
    Log.notice(F("Received message : %s" CR), JSONmessageBuffer);
  #endif
  }

  void setup() {
    Serial.begin(115200);
    delay(1000);

  #ifndef LOG_LEVEL
    #define LOG_LEVEL LOG_LEVEL_NOTICE
  #endif
    Log.begin(LOG_LEVEL, &Serial);

    // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
  }
    //MQTT
mqttClient.setServer(MQTT_HOST, MQTT_PORT);
mqttClient.setBufferSize(1024);
reconnectMQTT();


    // RF
    Log.notice(F("****** setup ******" CR));
    rf.initReceiver(RF_MODULE_RECEIVER_GPIO, RF_MODULE_FREQUENCY);
    rf.setCallback(rtl_433_Callback, messageBuffer, JSON_MSG_BUFFER);
    rf.enableReceiver();
    Log.notice(F("****** setup complete ******" CR));
    rf.getModuleStatus();
  }

  unsigned long uptime() {
    static unsigned long lastUptime = 0;
    static unsigned long uptimeAdd = 0;
    unsigned long uptime = millis() / 1000 + uptimeAdd;
    if (uptime < lastUptime) {
      uptime += 4294967;
      uptimeAdd += 4294967;
    }
    lastUptime = uptime;
    return uptime;
  }

  int next = uptime() + 30;

  #if defined(setBitrate) || defined(setFreqDev) || defined(setRxBW)

  #  ifdef setBitrate
  #    define TEST    "setBitrate" // 17.24 was suggested
  #    define STEP    2
  #    define stepMin 1
  #    define stepMax 300

  // #    define STEP    1
  // #    define stepMin 133
  // #    define stepMax 138
  #  elif defined(setFreqDev) // 40 kHz was suggested
  #    define TEST    "setFrequencyDeviation"
  #    define STEP    1
  #    define stepMin 5
  #    define stepMax 200
  #  elif defined(setRxBW)
  #    define TEST "setRxBandwidth"

  #    if defined(RF_SX1276) || defined(RF_SX1278)
  #      define STEP    5
  #      define stepMin 5
  #      define stepMax 250
  #    else
  #      define STEP    5
  #      define stepMin 58
  #      define stepMax 812
  // #      define STEP    0.01
  // #      define stepMin 202.00
  // #      define stepMax 205.00
  #    endif
  #  endif
  float step = stepMin;
  #endif

  void loop() {
    rf.loop();

    if (WiFi.status() == WL_CONNECTED) {
      reconnectMQTT();
      mqttClient.loop();
    
    if (mqttPending && mqttClient.connected()) {
    mqttPending = false;
    bool ok = mqttClient.publish(lastTopic, lastMsg);
    if (!ok) {
      Serial.println("MQTT publish FAILED (buffer too small or disconnected)");
    }
  }
    }
  #if defined(setBitrate) || defined(setFreqDev) || defined(setRxBW)
    char stepPrint[8];
    if (uptime() > next) {
      next = uptime() + 120; // 60 seconds
      dtostrf(step, 7, 2, stepPrint);
      Log.notice(F(CR "Finished %s: %s, count: %d" CR), TEST, stepPrint, count);
      step += STEP;
      if (step > stepMax) {
        step = stepMin;
      }
      dtostrf(step, 7, 2, stepPrint);
      Log.notice(F("Starting %s with %s" CR), TEST, stepPrint);
      count = 0;

      int16_t state = 0;
  #  ifdef setBitrate
      state = rf.setBitRate(step);
      RADIOLIB_STATE(state, TEST);
  #  elif defined(setFreqDev)
      state = rf.setFrequencyDeviation(step);
      RADIOLIB_STATE(state, TEST);
  #  elif defined(setRxBW)
      state = rf.setRxBandwidth(step);
      if ((state) != RADIOLIB_ERR_NONE) {
        Log.notice(F(CR "Setting  %s: to %s, failed" CR), TEST, stepPrint);
        next = uptime() - 1;
      }
  #  endif

      rf.receiveDirect();
      // rf.getModuleStatus();
    }
  #endif
  }
