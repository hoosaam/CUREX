# Space Medical Monitoring System

## Project Overview

This project is a comprehensive medical monitoring system designed for astronauts in space missions. The harsh conditions in space, such as complete isolation, physical and psychological exhaustion, and lack of gravity, significantly affect astronaut health. With limited medical tools available in space, this system provides continuous health monitoring to help astronauts and ground medical teams track vital signs and detect potential health issues early.

The system is particularly relevant for monitoring conditions that have similar symptoms to rheumatoid arthritis, which can be exacerbated by space conditions and continuous painkiller use that affects kidneys and other body parts.

## Hardware Components

### 1. LM35 Temperature Sensor - Body Temperature Monitoring
- **Purpose**: Continuous monitoring of body temperature
- **Pin**: A1
- **Principle**: Converts temperature to voltage (10mV per °C)
- **Range**: -55°C to +150°C
- **Medical Significance**: Detects fever, inflammation, or abnormal body temperature

### 2. DHT11 Sensor - Environmental Monitoring
- **Purpose**: Monitor cabin temperature and humidity
- **Pin**: Digital 2
- **Principle**: Measures ambient temperature and relative humidity
- **Range**: 0-50°C for temperature, 20-90% for humidity
- **Medical Significance**: Environmental factors affecting astronaut comfort and health

### 3. Flex Sensor - Joint Mobility Assessment
- **Purpose**: Monitor joint flexibility and movement patterns
- **Pin**: A0
- **Principle**: Variable resistance based on bending/flexion
- **Medical Significance**: Early detection of joint stiffness or mobility issues similar to rheumatoid arthritis symptoms

### 4. ESP32 Microcontroller
- **Purpose**: Main processing unit with WiFi/Bluetooth capabilities
- **Features**: 
  - WiFi connectivity for data transmission to ground control
  - Bluetooth for local communication
  - Sufficient processing power for real-time monitoring
  - Low power consumption for space applications

### 5. Mobile Application Interface
- **Purpose**: Real-time data visualization for medical professionals
- **Features**:
  - Live monitoring dashboard
  - Historical data analysis
  - Alert system for abnormal readings
  - Remote consultation capabilities

## Code Explanation

### Required Libraries
```cpp
#include "DHT.h"           // DHT11 sensor library
#include <WiFi.h>          // ESP32 WiFi library
#include <HTTPClient.h>    // HTTP client for data transmission
#include <ArduinoJson.h>   // JSON handling for API communication
```

### Pin Definitions and Variables
```cpp
#define DHTPIN 2           // DHT11 sensor pin
#define DHTTYPE DHT11      // Sensor type
#define flexPin A0         // Flex sensor pin
#define Lm35_Pin A1        // LM35 temperature sensor pin

// WiFi credentials
const char* ssid = "SPACE_STATION_WIFI";
const char* password = "secure_password";

// Medical monitoring thresholds
const float NORMAL_BODY_TEMP_MIN = 36.1;
const float NORMAL_BODY_TEMP_MAX = 37.2;
const float FLEX_THRESHOLD = 500; // Threshold for joint mobility assessment
```

### Setup Function
```cpp
void setup() { 
  Serial.begin(115200);    // Initialize serial communication
  dht.begin();             // Initialize DHT11 sensor
  WiFi.begin(ssid, password); // Connect to WiFi
  
  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
  Serial.println("Medical monitoring system initialized");
}
```

### Main Loop Function

#### 1. Sensor Data Reading
```cpp
float humidity = dht.readHumidity();           // Read cabin humidity
float ambient_temp = dht.readTemperature();    // Read cabin temperature
float flex_reading = analogRead(flexPin);      // Read joint flexibility
int body_temp_raw = analogRead(Lm35_Pin);      // Read body temperature
```

#### 2. Data Processing and Conversion
```cpp
// Convert LM35 reading to actual temperature
float voltage = body_temp_raw * (3.3 / 4095.0);  // ESP32 uses 3.3V and 12-bit ADC
float body_temperature = voltage * 100;           // LM35: 10mV per °C

// Process flex sensor data for joint mobility assessment
float joint_mobility = map(flex_reading, 0, 4095, 0, 100); // Convert to percentage
```

#### 3. Medical Alert System
```cpp
// Check for abnormal body temperature
if (body_temperature < NORMAL_BODY_TEMP_MIN || body_temperature > NORMAL_BODY_TEMP_MAX) {
  sendMedicalAlert("ABNORMAL_BODY_TEMPERATURE", body_temperature);
}

// Check for joint mobility issues
if (flex_reading < FLEX_THRESHOLD) {
  sendMedicalAlert("REDUCED_JOINT_MOBILITY", flex_reading);
}
```

#### 4. Data Transmission
- Real-time data transmission to ground control via WiFi
- JSON formatted data for easy processing
- Historical data logging for trend analysis
- Emergency alert system for critical readings

## System Operation

### 1. Continuous Monitoring
- **Data Collection**: System reads data from all three sensors every 2 seconds
- **Real-time Processing**: Converts raw sensor readings to meaningful medical data
- **Alert Generation**: Automatically generates alerts for abnormal readings
- **Data Transmission**: Sends all data to ground control via WiFi

### 2. Medical Decision Making
- **Body Temperature Analysis**: Monitors for fever, hypothermia, or inflammation
- **Joint Mobility Assessment**: Tracks flexibility changes that may indicate joint issues
- **Environmental Monitoring**: Ensures optimal cabin conditions for astronaut health
- **Trend Analysis**: Identifies patterns that may indicate developing health issues

### 3. Emergency Response System
- **Critical Alerts**: Immediate notification for life-threatening conditions
- **Progressive Warnings**: Early detection of potential health deterioration
- **Historical Tracking**: Long-term monitoring for chronic condition development
- **Remote Consultation**: Enables ground-based medical team intervention

## Medical Applications

### Primary Use Cases
- **Astronaut Health Monitoring**: Continuous vital sign tracking during space missions
- **Rheumatoid Arthritis Detection**: Early identification of joint mobility issues
- **Painkiller Impact Assessment**: Monitoring kidney and organ function
- **Environmental Health**: Ensuring optimal living conditions in space

### Clinical Benefits
- **Early Intervention**: Detect health issues before they become critical
- **Remote Monitoring**: Enable ground-based medical supervision
- **Data-Driven Decisions**: Provide objective data for medical assessments
- **Preventive Care**: Identify risk factors before symptoms manifest

## Mobile Application Features

### Real-time Dashboard
- Live vital sign monitoring
- Environmental condition display
- Alert notifications
- Historical trend graphs

### Medical Professional Interface
- Patient data visualization
- Alert management system
- Remote consultation tools
- Data export capabilities

### Emergency Response
- Critical alert notifications
- Automated emergency protocols
- Communication with ground control
- Medical intervention guidance

## Technical Requirements

### Hardware Components
- ESP32 Development Board
- LM35 Temperature Sensor
- DHT11 Humidity/Temperature Sensor
- Flex Sensor (Joint Mobility)
- WiFi connectivity module
- Power management system
- Data storage capabilities

### Software Requirements
- Arduino IDE with ESP32 support
- WiFi connectivity libraries
- JSON processing libraries
- HTTP client libraries
- Mobile application development framework
- Database for historical data storage

### Space Mission Considerations
- Radiation-hardened components
- Low power consumption design
- Reliable communication protocols
- Backup systems for critical functions
- Compact and lightweight design
- Temperature and pressure tolerance

## Future Enhancements

### Advanced Monitoring
- Heart rate and blood pressure sensors
- Oxygen saturation monitoring
- Sleep pattern analysis
- Stress level assessment

### AI Integration
- Machine learning for pattern recognition
- Predictive health analytics
- Automated diagnosis assistance
- Personalized health recommendations

### Extended Applications
- Long-duration space missions
- Mars exploration missions
- Space station health monitoring
- Commercial space travel
