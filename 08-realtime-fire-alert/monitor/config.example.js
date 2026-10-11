// Copy this file to config.js and fill in your own values.
// config.js is gitignored.
window.MONITOR_CONFIG = {
  // WebSocket address of the same broker the ESP32 uses.
  // Example for the public HiveMQ test broker: "wss://broker.hivemq.com:8884/mqtt"
  brokerUrl: "wss://YOUR_BROKER_HOST:8884/mqtt",
  username: "",
  password: "",
  // Must match MQTT_TOPIC_BASE in the firmware config.h
  topicBase: "students/fire-alarm/CHANGE_ME",
};
