#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "SPACE_STATION_WIFI";
const char* password = "secure_password";

// Server configuration
const char* serverURL = "http://ground-control-api.com/medical-data";

// Pin definitions
#define DHTPIN 2       
#define DHTTYPE DHT11  
#define flexPin A0
#define Lm35_Pin A1

// Medical monitoring thresholds
const float NORMAL_BODY_TEMP_MIN = 36.1;
const float NORMAL_BODY_TEMP_MAX = 37.2;
const float FLEX_THRESHOLD = 500;
const float CRITICAL_TEMP_HIGH = 38.5;
const float CRITICAL_TEMP_LOW = 35.0;

DHT dht(DHTPIN, DHTTYPE);

void setup() { 
  Serial.begin(115200);
  dht.begin();
  
  // Initialize WiFi connection
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  
  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Space Medical Monitoring System Initialized");
}

void loop() {
  // Read sensor data
  float humidity = dht.readHumidity();
  float ambient_temp = dht.readTemperature();
  float flex_reading = analogRead(flexPin);
  int body_temp_raw = analogRead(Lm35_Pin);
  
  // Convert LM35 reading to actual temperature (ESP32 uses 3.3V and 12-bit ADC)
  float voltage = body_temp_raw * (3.3 / 4095.0);
  float body_temperature = voltage * 100;  // LM35: 10mV per °C
  
  // Process flex sensor data for joint mobility assessment
  float joint_mobility = map(flex_reading, 0, 4095, 0, 100);

  // Check for sensor reading errors
  if (isnan(humidity) || isnan(ambient_temp)) {
    Serial.println("Failed to read from DHT11 sensor!");
    sendErrorAlert("DHT11_SENSOR_ERROR");
    delay(2000);
    return;
  }
  
  // Medical alert system
  checkMedicalAlerts(body_temperature, flex_reading, humidity, ambient_temp);
  
  // Display data on serial monitor
  displaySensorData(humidity, ambient_temp, flex_reading, body_temperature, joint_mobility);
  
  // Send data to ground control
  sendMedicalData(humidity, ambient_temp, flex_reading, body_temperature, joint_mobility);
  
  delay(2000);
}

void checkMedicalAlerts(float body_temp, float flex_reading, float humidity, float ambient_temp) {
  // Critical temperature alerts
  if (body_temp >= CRITICAL_TEMP_HIGH) {
    sendMedicalAlert("CRITICAL_FEVER", body_temp);
  } else if (body_temp <= CRITICAL_TEMP_LOW) {
    sendMedicalAlert("CRITICAL_HYPOTHERMIA", body_temp);
  } else if (body_temp < NORMAL_BODY_TEMP_MIN || body_temp > NORMAL_BODY_TEMP_MAX) {
    sendMedicalAlert("ABNORMAL_BODY_TEMPERATURE", body_temp);
  }
  
  // Joint mobility assessment
  if (flex_reading < FLEX_THRESHOLD) {
    sendMedicalAlert("REDUCED_JOINT_MOBILITY", flex_reading);
  }
  
  // Environmental alerts
  if (ambient_temp < 18 || ambient_temp > 25) {
    sendMedicalAlert("SUBOPTIMAL_CABIN_TEMPERATURE", ambient_temp);
  }
  
  if (humidity < 30 || humidity > 70) {
    sendMedicalAlert("SUBOPTIMAL_CABIN_HUMIDITY", humidity);
  }
}

void displaySensorData(float humidity, float ambient_temp, float flex_reading, float body_temp, float joint_mobility) {
  Serial.println("=== SPACE MEDICAL MONITORING DATA ===");
  Serial.print("Cabin Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  
  Serial.print("Cabin Temperature: ");
  Serial.print(ambient_temp);
  Serial.println(" °C");
  
  Serial.print("Joint Mobility: ");
  Serial.print(joint_mobility);
  Serial.println(" %");
  
  Serial.print("Body Temperature: ");
  Serial.print(body_temp);
  Serial.println(" °C");
  
  Serial.print("Flex Sensor Raw: ");
  Serial.println(flex_reading);
  Serial.println("=====================================");
}

void sendMedicalData(float humidity, float ambient_temp, float flex_reading, float body_temp, float joint_mobility) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");
    
    // Create JSON payload
    DynamicJsonDocument doc(1024);
    doc["timestamp"] = millis();
    doc["astronaut_id"] = "ASTRONAUT_001";
    doc["mission_id"] = "MISSION_2024";
    doc["cabin_humidity"] = humidity;
    doc["cabin_temperature"] = ambient_temp;
    doc["body_temperature"] = body_temp;
    doc["joint_mobility"] = joint_mobility;
    doc["flex_sensor_raw"] = flex_reading;
    doc["system_status"] = "OPERATIONAL";
    
    String jsonString;
    serializeJson(doc, jsonString);
    
    int httpResponseCode = http.POST(jsonString);
    
    if (httpResponseCode > 0) {
      Serial.println("Data sent successfully to ground control");
    } else {
      Serial.println("Error sending data to ground control");
    }
    
    http.end();
  } else {
    Serial.println("WiFi not connected - data not sent");
  }
}

void sendMedicalAlert(String alert_type, float value) {
  Serial.print("MEDICAL ALERT: ");
  Serial.print(alert_type);
  Serial.print(" - Value: ");
  Serial.println(value);
  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");
    
    DynamicJsonDocument doc(512);
    doc["alert_type"] = alert_type;
    doc["value"] = value;
    doc["timestamp"] = millis();
    doc["astronaut_id"] = "ASTRONAUT_001";
    doc["priority"] = "HIGH";
    
    String jsonString;
    serializeJson(doc, jsonString);
    
    http.POST(jsonString);
    http.end();
  }
}

void sendErrorAlert(String error_type) {
  Serial.print("SYSTEM ERROR: ");
  Serial.println(error_type);
  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");
    
    DynamicJsonDocument doc(256);
    doc["error_type"] = error_type;
    doc["timestamp"] = millis();
    doc["system_status"] = "ERROR";
    
    String jsonString;
    serializeJson(doc, jsonString);
    
    http.POST(jsonString);
    http.end();
  }
}
