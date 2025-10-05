#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <SPIFFS.h>

// WiFi credentials
const char* ssid = "SPACE_STATION_WIFI";
const char* password = "secure_password";

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
WebServer server(80);

// Global variables for sensor data
float humidity = 0;
float ambient_temp = 0;
float flex_reading = 0;
float body_temperature = 0;
float joint_mobility = 0;
unsigned long lastUpdate = 0;

void setup() { 
  Serial.begin(115200);
  dht.begin();
  
  // Initialize SPIFFS for file system
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
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
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Setup web server routes
  setupWebServer();
  
  Serial.println("Space Medical Monitoring System with Web Server Initialized");
}

void loop() {
  // Handle web server requests
  server.handleClient();
  
  // Update sensor data every 2 seconds
  if (millis() - lastUpdate >= 2000) {
    readSensorData();
    checkMedicalAlerts();
    lastUpdate = millis();
  }
  
  delay(100);
}

void setupWebServer() {
  // Serve static files
  server.on("/", HTTP_GET, handleRoot);
  server.on("/styles.css", HTTP_GET, handleCSS);
  server.on("/script.js", HTTP_GET, handleJS);
  
  // API endpoints
  server.on("/api/data", HTTP_GET, handleGetData);
  server.on("/api/alerts", HTTP_GET, handleGetAlerts);
  server.on("/api/status", HTTP_GET, handleGetStatus);
  
  // Start server
  server.begin();
  Serial.println("Web server started");
}

void handleRoot() {
  String html = R"(
<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>نظام مراقبة الصحة الفضائية - Space Medical Monitoring</title>
    <link rel="stylesheet" href="/styles.css">
    <link href="https://fonts.googleapis.com/css2?family=Cairo:wght@300;400;600;700&display=swap" rel="stylesheet">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css">
</head>
<body>
    <div class="container">
        <!-- Header -->
        <header class="header">
            <div class="header-content">
                <div class="logo">
                    <i class="fas fa-rocket"></i>
                    <h1>نظام مراقبة الصحة الفضائية</h1>
                </div>
                <div class="mission-info">
                    <span class="mission-id">المهمة: MISSION_2024</span>
                    <span class="astronaut-id">رائد الفضاء: ASTRONAUT_001</span>
                </div>
            </div>
        </header>

        <!-- Status Bar -->
        <div class="status-bar">
            <div class="status-item">
                <i class="fas fa-wifi"></i>
                <span id="connection-status">متصل</span>
            </div>
            <div class="status-item">
                <i class="fas fa-clock"></i>
                <span id="current-time">--:--:--</span>
            </div>
            <div class="status-item">
                <i class="fas fa-shield-alt"></i>
                <span id="system-status">نظام تشغيلي</span>
            </div>
        </div>

        <!-- Main Dashboard -->
        <main class="dashboard">
            <!-- Vital Signs Section -->
            <section class="vital-signs">
                <h2><i class="fas fa-heartbeat"></i> العلامات الحيوية</h2>
                <div class="cards-grid">
                    <div class="card temperature-card">
                        <div class="card-header">
                            <i class="fas fa-thermometer-half"></i>
                            <h3>درجة حرارة الجسم</h3>
                        </div>
                        <div class="card-content">
                            <div class="value" id="body-temp">--°C</div>
                            <div class="status" id="body-temp-status">جاري القياس...</div>
                        </div>
                        <div class="card-footer">
                            <span class="normal-range">المدى الطبيعي: 36.1°C - 37.2°C</span>
                        </div>
                    </div>

                    <div class="card mobility-card">
                        <div class="card-header">
                            <i class="fas fa-hand-paper"></i>
                            <h3>مرونة المفاصل</h3>
                        </div>
                        <div class="card-content">
                            <div class="value" id="joint-mobility">--%</div>
                            <div class="status" id="mobility-status">جاري القياس...</div>
                        </div>
                        <div class="card-footer">
                            <span class="normal-range">المدى الطبيعي: > 500</span>
                        </div>
                    </div>
                </div>
            </section>

            <!-- Environmental Conditions -->
            <section class="environmental">
                <h2><i class="fas fa-globe"></i> الظروف البيئية</h2>
                <div class="cards-grid">
                    <div class="card cabin-temp-card">
                        <div class="card-header">
                            <i class="fas fa-thermometer-quarter"></i>
                            <h3>درجة حرارة المقصورة</h3>
                        </div>
                        <div class="card-content">
                            <div class="value" id="cabin-temp">--°C</div>
                            <div class="status" id="cabin-temp-status">جاري القياس...</div>
                        </div>
                        <div class="card-footer">
                            <span class="normal-range">المدى المثالي: 18°C - 25°C</span>
                        </div>
                    </div>

                    <div class="card humidity-card">
                        <div class="card-header">
                            <i class="fas fa-tint"></i>
                            <h3>الرطوبة</h3>
                        </div>
                        <div class="card-content">
                            <div class="value" id="humidity">--%</div>
                            <div class="status" id="humidity-status">جاري القياس...</div>
                        </div>
                        <div class="card-footer">
                            <span class="normal-range">المدى المثالي: 30% - 70%</span>
                        </div>
                    </div>
                </div>
            </section>

            <!-- Alerts Section -->
            <section class="alerts-section">
                <h2><i class="fas fa-exclamation-triangle"></i> التنبيهات الطبية</h2>
                <div class="alerts-container" id="alerts-container">
                    <div class="no-alerts">
                        <i class="fas fa-check-circle"></i>
                        <p>لا توجد تنبيهات طبية حالياً</p>
                    </div>
                </div>
            </section>
        </main>

        <!-- Footer -->
        <footer class="footer">
            <div class="footer-content">
                <p>&copy; 2024 نظام مراقبة الصحة الفضائية - Space Medical Monitoring System</p>
                <div class="footer-links">
                    <a href="#" onclick="showSystemInfo()"><i class="fas fa-info-circle"></i> معلومات النظام</a>
                    <a href="#" onclick="showSettings()"><i class="fas fa-cog"></i> الإعدادات</a>
                    <a href="#" onclick="contactGroundControl()"><i class="fas fa-phone"></i> الاتصال بمركز التحكم</a>
                </div>
            </div>
        </footer>
    </div>

    <script>
        // Real-time data fetching from ESP32
        let isConnected = true;
        
        function updateTime() {
            const now = new Date();
            const timeString = now.toLocaleTimeString('ar-SA', {
                hour12: false,
                hour: '2-digit',
                minute: '2-digit',
                second: '2-digit'
            });
            document.getElementById('current-time').textContent = timeString;
        }
        
        function fetchData() {
            fetch('/api/data')
                .then(response => response.json())
                .then(data => {
                    updateDisplay(data);
                    isConnected = true;
                    document.getElementById('connection-status').textContent = 'متصل';
                    document.getElementById('connection-status').className = 'status-normal';
                })
                .catch(error => {
                    console.error('Error fetching data:', error);
                    isConnected = false;
                    document.getElementById('connection-status').textContent = 'غير متصل';
                    document.getElementById('connection-status').className = 'status-error';
                });
        }
        
        function updateDisplay(data) {
            // Update body temperature
            document.getElementById('body-temp').textContent = data.body_temperature.toFixed(1) + '°C';
            updateTemperatureStatus(data.body_temperature, 'body-temp-status');
            
            // Update joint mobility
            document.getElementById('joint-mobility').textContent = data.joint_mobility.toFixed(0) + '%';
            updateMobilityStatus(data.joint_mobility, 'mobility-status');
            
            // Update cabin temperature
            document.getElementById('cabin-temp').textContent = data.cabin_temperature.toFixed(1) + '°C';
            updateCabinTempStatus(data.cabin_temperature, 'cabin-temp-status');
            
            // Update humidity
            document.getElementById('humidity').textContent = data.humidity.toFixed(0) + '%';
            updateHumidityStatus(data.humidity, 'humidity-status');
        }
        
        function updateTemperatureStatus(temp, elementId) {
            const element = document.getElementById(elementId);
            if (temp >= 38.5) {
                element.textContent = 'حمى شديدة - تنبيه حرج';
                element.className = 'status status-critical';
            } else if (temp <= 35.0) {
                element.textContent = 'انخفاض حرارة - تنبيه حرج';
                element.className = 'status status-critical';
            } else if (temp < 36.1 || temp > 37.2) {
                element.textContent = 'درجة حرارة غير طبيعية';
                element.className = 'status status-warning';
            } else {
                element.textContent = 'طبيعي';
                element.className = 'status status-normal';
            }
        }
        
        function updateMobilityStatus(mobility, elementId) {
            const element = document.getElementById(elementId);
            if (mobility < 30) {
                element.textContent = 'انخفاض شديد في المرونة';
                element.className = 'status status-critical';
            } else if (mobility < 50) {
                element.textContent = 'انخفاض في المرونة';
                element.className = 'status status-warning';
            } else {
                element.textContent = 'طبيعي';
                element.className = 'status status-normal';
            }
        }
        
        function updateCabinTempStatus(temp, elementId) {
            const element = document.getElementById(elementId);
            if (temp < 18 || temp > 25) {
                element.textContent = 'درجة حرارة غير مثالية';
                element.className = 'status status-warning';
            } else {
                element.textContent = 'مثالي';
                element.className = 'status status-normal';
            }
        }
        
        function updateHumidityStatus(humidity, elementId) {
            const element = document.getElementById(elementId);
            if (humidity < 30 || humidity > 70) {
                element.textContent = 'رطوبة غير مثالية';
                element.className = 'status status-warning';
            } else {
                element.textContent = 'مثالي';
                element.className = 'status status-normal';
            }
        }
        
        function showSystemInfo() {
            alert('نظام مراقبة الصحة الفضائية - إصدار 1.0\\nتم تطويره لمراقبة العلامات الحيوية لرواد الفضاء');
        }
        
        function showSettings() {
            alert('صفحة الإعدادات قيد التطوير');
        }
        
        function contactGroundControl() {
            alert('جاري الاتصال بمركز التحكم الأرضي...');
        }
        
        // Initialize
        setInterval(updateTime, 1000);
        setInterval(fetchData, 2000);
        fetchData(); // Initial load
    </script>
</body>
</html>
  )";
  
  server.send(200, "text/html", html);
}

void handleCSS() {
  // Serve CSS file (you can copy the content from styles.css)
  server.send(200, "text/css", getCSSContent());
}

void handleJS() {
  // Serve JavaScript file
  server.send(200, "application/javascript", getJSContent());
}

void handleGetData() {
  DynamicJsonDocument doc(1024);
  doc["body_temperature"] = body_temperature;
  doc["joint_mobility"] = joint_mobility;
  doc["cabin_temperature"] = ambient_temp;
  doc["humidity"] = humidity;
  doc["flex_sensor_raw"] = flex_reading;
  doc["timestamp"] = millis();
  doc["system_status"] = "OPERATIONAL";
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  server.send(200, "application/json", jsonString);
}

void handleGetAlerts() {
  DynamicJsonDocument doc(512);
  JsonArray alerts = doc.createNestedArray("alerts");
  
  // Check for alerts and add them to the array
  if (body_temperature >= CRITICAL_TEMP_HIGH) {
    JsonObject alert = alerts.createNestedObject();
    alert["type"] = "CRITICAL_FEVER";
    alert["message"] = "حمى شديدة - تتطلب تدخل فوري";
    alert["priority"] = "critical";
  }
  
  if (body_temperature <= CRITICAL_TEMP_LOW) {
    JsonObject alert = alerts.createNestedObject();
    alert["type"] = "CRITICAL_HYPOTHERMIA";
    alert["message"] = "انخفاض حرارة شديد - تتطلب تدخل فوري";
    alert["priority"] = "critical";
  }
  
  if (flex_reading < FLEX_THRESHOLD) {
    JsonObject alert = alerts.createNestedObject();
    alert["type"] = "REDUCED_JOINT_MOBILITY";
    alert["message"] = "انخفاض في مرونة المفاصل";
    alert["priority"] = "warning";
  }
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  server.send(200, "application/json", jsonString);
}

void handleGetStatus() {
  DynamicJsonDocument doc(256);
  doc["wifi_status"] = WiFi.status() == WL_CONNECTED ? "connected" : "disconnected";
  doc["system_status"] = "operational";
  doc["uptime"] = millis();
  doc["free_heap"] = ESP.getFreeHeap();
  
  String jsonString;
  serializeJson(doc, jsonString);
  
  server.send(200, "application/json", jsonString);
}

void readSensorData() {
  // Read sensor data
  humidity = dht.readHumidity();
  ambient_temp = dht.readTemperature();
  flex_reading = analogRead(flexPin);
  int body_temp_raw = analogRead(Lm35_Pin);
  
  // Convert LM35 reading to actual temperature
  float voltage = body_temp_raw * (3.3 / 4095.0);
  body_temperature = voltage * 100;
  
  // Process flex sensor data
  joint_mobility = map(flex_reading, 0, 4095, 0, 100);
  
  // Check for sensor reading errors
  if (isnan(humidity) || isnan(ambient_temp)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return;
  }
  
  // Display data on serial monitor
  displaySensorData();
}

void checkMedicalAlerts() {
  // Critical temperature alerts
  if (body_temperature >= CRITICAL_TEMP_HIGH) {
    sendMedicalAlert("CRITICAL_FEVER", body_temperature);
  } else if (body_temperature <= CRITICAL_TEMP_LOW) {
    sendMedicalAlert("CRITICAL_HYPOTHERMIA", body_temperature);
  } else if (body_temperature < NORMAL_BODY_TEMP_MIN || body_temperature > NORMAL_BODY_TEMP_MAX) {
    sendMedicalAlert("ABNORMAL_BODY_TEMPERATURE", body_temperature);
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

void displaySensorData() {
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
  Serial.print(body_temperature);
  Serial.println(" °C");
  
  Serial.print("Flex Sensor Raw: ");
  Serial.println(flex_reading);
  Serial.println("=====================================");
}

void sendMedicalAlert(String alert_type, float value) {
  Serial.print("MEDICAL ALERT: ");
  Serial.print(alert_type);
  Serial.print(" - Value: ");
  Serial.println(value);
}

String getCSSContent() {
  // Return the CSS content (you can copy from styles.css)
  return R"(
/* Space Medical Monitoring System - CSS Styles */
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: 'Cairo', sans-serif;
    background: linear-gradient(135deg, #0c0c0c 0%, #1a1a2e 50%, #16213e 100%);
    color: #ffffff;
    min-height: 100vh;
    overflow-x: hidden;
}

/* Add your CSS content here - truncated for brevity */
.container {
    max-width: 1400px;
    margin: 0 auto;
    padding: 0 20px;
}

/* ... rest of CSS ... */
  )";
}

String getJSContent() {
  // Return minimal JavaScript content
  return R"(
// Minimal JavaScript for ESP32 integration
console.log('Space Medical Monitoring System - ESP32 Version');
  )";
}
