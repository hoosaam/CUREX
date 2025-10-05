# دليل ربط نظام مراقبة الصحة الفضائية

## كيفية ربط HTML/CSS/JavaScript مع Arduino ESP32

### الطريقة الأولى: خادم ويب مدمج في ESP32 (الأسهل)

#### 1. تحميل المكتبات المطلوبة
```cpp
#include <WebServer.h>
#include <SPIFFS.h>
```

#### 2. إعداد الخادم
- ESP32 يعمل كخادم ويب
- يخدم ملفات HTML/CSS/JS مباشرة
- يوفر API endpoints للبيانات

#### 3. خطوات التطبيق:

**أ) رفع الكود الجديد:**
1. افتح Arduino IDE
2. ارفع ملف `esp32_webserver.ino` إلى ESP32
3. تأكد من توصيل المستشعرات بشكل صحيح

**ب) الاتصال بالشبكة:**
1. غير إعدادات WiFi في الكود:
```cpp
const char* ssid = "اسم_الشبكة_الخاصة_بك";
const char* password = "كلمة_المرور";
```

**ج) الوصول للواجهة:**
1. افتح المتصفح
2. اذهب إلى IP address الخاص بـ ESP32
3. ستظهر واجهة المراقبة الطبية

### الطريقة الثانية: استخدام SPIFFS (الأكثر تطوراً)

#### 1. إعداد SPIFFS:
```bash
# تثبيت SPIFFS Data Upload tool
# في Arduino IDE: Tools > Manage Libraries > SPIFFS
```

#### 2. إنشاء مجلد data:
```
project_folder/
├── data/
│   ├── index.html
│   ├── styles.css
│   └── script.js
└── esp32_webserver.ino
```

#### 3. رفع الملفات:
- Tools > ESP32 Sketch Data Upload
- سيتم رفع الملفات إلى ESP32

### الطريقة الثالثة: خادم منفصل + API

#### 1. ESP32 كـ API Server فقط:
```cpp
// ESP32 يرسل البيانات عبر JSON API
server.on("/api/data", HTTP_GET, handleGetData);
server.on("/api/alerts", HTTP_GET, handleGetAlerts);
```

#### 2. HTML/CSS/JS على خادم منفصل:
- يمكن استضافتها على أي خادم ويب
- تستدعي API الخاص بـ ESP32

## API Endpoints المتاحة

### 1. `/api/data` - بيانات المستشعرات
```json
{
  "body_temperature": 36.8,
  "joint_mobility": 85.2,
  "cabin_temperature": 23.5,
  "humidity": 45.0,
  "flex_sensor_raw": 750,
  "timestamp": 1234567890,
  "system_status": "OPERATIONAL"
}
```

### 2. `/api/alerts` - التنبيهات الطبية
```json
{
  "alerts": [
    {
      "type": "CRITICAL_FEVER",
      "message": "حمى شديدة - تتطلب تدخل فوري",
      "priority": "critical"
    }
  ]
}
```

### 3. `/api/status` - حالة النظام
```json
{
  "wifi_status": "connected",
  "system_status": "operational",
  "uptime": 1234567890,
  "free_heap": 150000
}
```

## خطوات التطبيق العملي

### الخطوة 1: إعداد ESP32
```cpp
// في setup()
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
}
Serial.println("WiFi connected!");
Serial.println(WiFi.localIP());
```

### الخطوة 2: إعداد الخادم
```cpp
void setupWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/data", HTTP_GET, handleGetData);
  server.begin();
}
```

### الخطوة 3: تحديث البيانات
```cpp
void loop() {
  server.handleClient();
  
  if (millis() - lastUpdate >= 2000) {
    readSensorData();
    lastUpdate = millis();
  }
}
```

### الخطوة 4: JavaScript في HTML
```javascript
function fetchData() {
  fetch('/api/data')
    .then(response => response.json())
    .then(data => {
      updateDisplay(data);
    });
}

setInterval(fetchData, 2000);
```

## استكشاف الأخطاء

### مشكلة: لا يمكن الوصول للواجهة
**الحل:**
1. تأكد من IP address الصحيح
2. تحقق من اتصال WiFi
3. تأكد من تشغيل الخادم

### مشكلة: البيانات لا تظهر
**الحل:**
1. تحقق من API endpoints
2. افتح Developer Tools في المتصفح
3. تحقق من Console للأخطاء

### مشكلة: المستشعرات لا تعمل
**الحل:**
1. تحقق من التوصيلات
2. تأكد من pin numbers
3. تحقق من Serial Monitor

## تحسينات إضافية

### 1. WebSocket للبيانات المباشرة
```cpp
#include <WebSocketsServer.h>
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  // Handle WebSocket events
}
```

### 2. قاعدة بيانات محلية
```cpp
#include <Preferences.h>
Preferences preferences;

void saveData() {
  preferences.putFloat("body_temp", body_temperature);
  preferences.putFloat("humidity", humidity);
}
```

### 3. إشعارات Push
```cpp
// إرسال إشعارات للهاتف عند التنبيهات الحرجة
void sendPushNotification(String message) {
  // Implementation for push notifications
}
```

## الأمان

### 1. تشفير البيانات
```cpp
#include <WiFiClientSecure.h>
WiFiClientSecure client;
client.setCACert(ca_cert);
```

### 2. Authentication
```cpp
bool authenticateRequest() {
  String auth = server.header("Authorization");
  return auth == "Bearer your_secret_token";
}
```

### 3. Rate Limiting
```cpp
unsigned long lastRequest = 0;
if (millis() - lastRequest < 1000) {
  server.send(429, "text/plain", "Too Many Requests");
  return;
}
```

## الأداء

### 1. تحسين الذاكرة
```cpp
// استخدام String بدلاً من String
char buffer[256];
sprintf(buffer, "Temperature: %.1f°C", body_temperature);
```

### 2. ضغط البيانات
```cpp
#include <ArduinoJson.h>
// استخدام JSON compact
serializeJson(doc, jsonString);
```

### 3. Caching
```cpp
// Cache البيانات لمدة ثانية واحدة
if (millis() - lastCacheUpdate > 1000) {
  updateCache();
  lastCacheUpdate = millis();
}
```

## الخلاصة

الطريقة الأسهل والأسرع هي استخدام **الطريقة الأولى** (خادم ويب مدمج) لأنها:
- لا تحتاج خادم منفصل
- سهلة التطبيق
- تعمل مباشرة مع ESP32
- مناسبة للمشاريع الصغيرة والمتوسطة

بعد تطبيق هذه الخطوات، ستحصل على نظام مراقبة طبية كامل يعمل عبر الإنترنت! 🚀
