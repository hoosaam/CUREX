// Space Medical Monitoring System - JavaScript

class MedicalMonitoringSystem {
    constructor() {
        this.data = {
            bodyTemperature: null,
            jointMobility: null,
            cabinTemperature: null,
            humidity: null,
            flexSensorRaw: null
        };
        
        this.alerts = [];
        this.isConnected = true;
        this.updateInterval = null;
        
        this.init();
    }
    
    init() {
        this.updateTime();
        this.startDataSimulation();
        this.setupEventListeners();
        
        // Update time every second
        setInterval(() => this.updateTime(), 1000);
    }
    
    updateTime() {
        const now = new Date();
        const timeString = now.toLocaleTimeString('ar-SA', {
            hour12: false,
            hour: '2-digit',
            minute: '2-digit',
            second: '2-digit'
        });
        document.getElementById('current-time').textContent = timeString;
    }
    
    startDataSimulation() {
        // Simulate real-time data updates every 2 seconds
        this.updateInterval = setInterval(() => {
            this.generateSimulatedData();
            this.updateDisplay();
            this.checkAlerts();
        }, 2000);
        
        // Initial data load
        this.generateSimulatedData();
        this.updateDisplay();
    }
    
    generateSimulatedData() {
        // Simulate realistic sensor readings with some variation
        const baseTime = Date.now() / 1000;
        
        // Body temperature (36.1 - 37.2 normal range)
        this.data.bodyTemperature = 36.5 + Math.sin(baseTime * 0.1) * 0.3 + (Math.random() - 0.5) * 0.2;
        
        // Joint mobility (percentage)
        this.data.jointMobility = 75 + Math.sin(baseTime * 0.05) * 15 + (Math.random() - 0.5) * 10;
        this.data.jointMobility = Math.max(0, Math.min(100, this.data.jointMobility));
        
        // Cabin temperature (18-25 ideal range)
        this.data.cabinTemperature = 22 + Math.sin(baseTime * 0.02) * 2 + (Math.random() - 0.5) * 1;
        
        // Humidity (30-70 ideal range)
        this.data.humidity = 50 + Math.sin(baseTime * 0.03) * 10 + (Math.random() - 0.5) * 5;
        this.data.humidity = Math.max(0, Math.min(100, this.data.humidity));
        
        // Flex sensor raw value
        this.data.flexSensorRaw = 600 + Math.sin(baseTime * 0.08) * 100 + (Math.random() - 0.5) * 50;
        this.data.flexSensorRaw = Math.max(0, Math.min(4095, this.data.flexSensorRaw));
        
        // Occasionally simulate critical conditions for testing
        if (Math.random() < 0.05) { // 5% chance
            this.simulateCriticalCondition();
        }
    }
    
    simulateCriticalCondition() {
        const conditions = [
            () => { this.data.bodyTemperature = 38.8; }, // High fever
            () => { this.data.bodyTemperature = 35.2; }, // Hypothermia
            () => { this.data.jointMobility = 25; }, // Reduced mobility
            () => { this.data.cabinTemperature = 16; }, // Cold cabin
            () => { this.data.humidity = 85; } // High humidity
        ];
        
        const randomCondition = conditions[Math.floor(Math.random() * conditions.length)];
        randomCondition();
    }
    
    updateDisplay() {
        // Update body temperature
        const bodyTempElement = document.getElementById('body-temp');
        const bodyTempStatus = document.getElementById('body-temp-status');
        
        if (this.data.bodyTemperature !== null) {
            bodyTempElement.textContent = `${this.data.bodyTemperature.toFixed(1)}°C`;
            
            if (this.data.bodyTemperature >= 38.5) {
                bodyTempStatus.textContent = 'حمى شديدة - تنبيه حرج';
                bodyTempStatus.className = 'status status-critical';
                bodyTempElement.parentElement.parentElement.classList.add('pulse-critical');
            } else if (this.data.bodyTemperature <= 35.0) {
                bodyTempStatus.textContent = 'انخفاض حرارة - تنبيه حرج';
                bodyTempStatus.className = 'status status-critical';
                bodyTempElement.parentElement.parentElement.classList.add('pulse-critical');
            } else if (this.data.bodyTemperature < 36.1 || this.data.bodyTemperature > 37.2) {
                bodyTempStatus.textContent = 'درجة حرارة غير طبيعية';
                bodyTempStatus.className = 'status status-warning';
                bodyTempElement.parentElement.parentElement.classList.remove('pulse-critical');
            } else {
                bodyTempStatus.textContent = 'طبيعي';
                bodyTempStatus.className = 'status status-normal';
                bodyTempElement.parentElement.parentElement.classList.remove('pulse-critical');
            }
        }
        
        // Update joint mobility
        const jointMobilityElement = document.getElementById('joint-mobility');
        const mobilityStatus = document.getElementById('mobility-status');
        
        if (this.data.jointMobility !== null) {
            jointMobilityElement.textContent = `${this.data.jointMobility.toFixed(0)}%`;
            
            if (this.data.jointMobility < 30) {
                mobilityStatus.textContent = 'انخفاض شديد في المرونة';
                mobilityStatus.className = 'status status-critical';
            } else if (this.data.jointMobility < 50) {
                mobilityStatus.textContent = 'انخفاض في المرونة';
                mobilityStatus.className = 'status status-warning';
            } else {
                mobilityStatus.textContent = 'طبيعي';
                mobilityStatus.className = 'status status-normal';
            }
        }
        
        // Update cabin temperature
        const cabinTempElement = document.getElementById('cabin-temp');
        const cabinTempStatus = document.getElementById('cabin-temp-status');
        
        if (this.data.cabinTemperature !== null) {
            cabinTempElement.textContent = `${this.data.cabinTemperature.toFixed(1)}°C`;
            
            if (this.data.cabinTemperature < 18 || this.data.cabinTemperature > 25) {
                cabinTempStatus.textContent = 'درجة حرارة غير مثالية';
                cabinTempStatus.className = 'status status-warning';
            } else {
                cabinTempStatus.textContent = 'مثالي';
                cabinTempStatus.className = 'status status-normal';
            }
        }
        
        // Update humidity
        const humidityElement = document.getElementById('humidity');
        const humidityStatus = document.getElementById('humidity-status');
        
        if (this.data.humidity !== null) {
            humidityElement.textContent = `${this.data.humidity.toFixed(0)}%`;
            
            if (this.data.humidity < 30 || this.data.humidity > 70) {
                humidityStatus.textContent = 'رطوبة غير مثالية';
                humidityStatus.className = 'status status-warning';
            } else {
                humidityStatus.textContent = 'مثالي';
                humidityStatus.className = 'status status-normal';
            }
        }
        
        // Update connection status
        const connectionStatus = document.getElementById('connection-status');
        if (this.isConnected) {
            connectionStatus.textContent = 'متصل';
            connectionStatus.className = 'status-normal';
        } else {
            connectionStatus.textContent = 'غير متصل';
            connectionStatus.className = 'status-error';
        }
    }
    
    checkAlerts() {
        const newAlerts = [];
        
        // Check body temperature alerts
        if (this.data.bodyTemperature >= 38.5) {
            newAlerts.push({
                type: 'CRITICAL_FEVER',
                title: 'حمى شديدة',
                message: `درجة حرارة الجسم: ${this.data.bodyTemperature.toFixed(1)}°C - تتطلب تدخل فوري`,
                time: new Date().toLocaleTimeString('ar-SA'),
                priority: 'critical'
            });
        } else if (this.data.bodyTemperature <= 35.0) {
            newAlerts.push({
                type: 'CRITICAL_HYPOTHERMIA',
                title: 'انخفاض حرارة شديد',
                message: `درجة حرارة الجسم: ${this.data.bodyTemperature.toFixed(1)}°C - تتطلب تدخل فوري`,
                time: new Date().toLocaleTimeString('ar-SA'),
                priority: 'critical'
            });
        } else if (this.data.bodyTemperature < 36.1 || this.data.bodyTemperature > 37.2) {
            newAlerts.push({
                type: 'ABNORMAL_BODY_TEMPERATURE',
                title: 'درجة حرارة غير طبيعية',
                message: `درجة حرارة الجسم: ${this.data.bodyTemperature.toFixed(1)}°C - خارج المدى الطبيعي`,
                time: new Date().toLocaleTimeString('ar-SA'),
                priority: 'warning'
            });
        }
        
        // Check joint mobility alerts
        if (this.data.jointMobility < 30) {
            newAlerts.push({
                type: 'REDUCED_JOINT_MOBILITY',
                title: 'انخفاض شديد في مرونة المفاصل',
                message: `مرونة المفاصل: ${this.data.jointMobility.toFixed(0)}% - قد يشير إلى مشاكل في المفاصل`,
                time: new Date().toLocaleTimeString('ar-SA'),
                priority: 'critical'
            });
        } else if (this.data.jointMobility < 50) {
            newAlerts.push({
                type: 'REDUCED_JOINT_MOBILITY',
                title: 'انخفاض في مرونة المفاصل',
                message: `مرونة المفاصل: ${this.data.jointMobility.toFixed(0)}% - مراقبة مستمرة مطلوبة`,
                time: new Date().toLocaleTimeString('ar-SA'),
                priority: 'warning'
            });
        }
        
        // Check environmental alerts
        if (this.data.cabinTemperature < 18 || this.data.cabinTemperature > 25) {
            newAlerts.push({
                type: 'SUBOPTIMAL_CABIN_TEMPERATURE',
                title: 'درجة حرارة المقصورة غير مثالية',
                message: `درجة الحرارة: ${this.data.cabinTemperature.toFixed(1)}°C - خارج المدى المثالي`,
                time: new Date().toLocaleTimeString('ar-SA'),
                priority: 'warning'
            });
        }
        
        if (this.data.humidity < 30 || this.data.humidity > 70) {
            newAlerts.push({
                type: 'SUBOPTIMAL_CABIN_HUMIDITY',
                title: 'رطوبة المقصورة غير مثالية',
                message: `الرطوبة: ${this.data.humidity.toFixed(0)}% - خارج المدى المثالي`,
                time: new Date().toLocaleTimeString('ar-SA'),
                priority: 'warning'
            });
        }
        
        // Update alerts
        this.alerts = newAlerts;
        this.updateAlertsDisplay();
    }
    
    updateAlertsDisplay() {
        const alertsContainer = document.getElementById('alerts-container');
        
        if (this.alerts.length === 0) {
            alertsContainer.innerHTML = `
                <div class="no-alerts">
                    <i class="fas fa-check-circle"></i>
                    <p>لا توجد تنبيهات طبية حالياً</p>
                </div>
            `;
        } else {
            alertsContainer.innerHTML = this.alerts.map(alert => `
                <div class="alert-item ${alert.priority === 'critical' ? 'pulse-critical' : ''}">
                    <i class="fas fa-exclamation-triangle"></i>
                    <div class="alert-content">
                        <div class="alert-title">${alert.title}</div>
                        <div class="alert-message">${alert.message}</div>
                    </div>
                    <div class="alert-time">${alert.time}</div>
                </div>
            `).join('');
        }
    }
    
    setupEventListeners() {
        // Simulate connection issues occasionally
        setInterval(() => {
            if (Math.random() < 0.02) { // 2% chance
                this.isConnected = false;
                setTimeout(() => {
                    this.isConnected = true;
                }, 5000); // Reconnect after 5 seconds
            }
        }, 10000);
        
        // Add click handlers for footer links
        document.querySelectorAll('.footer-links a').forEach(link => {
            link.addEventListener('click', (e) => {
                e.preventDefault();
                this.handleFooterLinkClick(link.textContent.trim());
            });
        });
    }
    
    handleFooterLinkClick(linkText) {
        const messages = {
            'معلومات النظام': 'نظام مراقبة الصحة الفضائية - إصدار 1.0\nتم تطويره لمراقبة العلامات الحيوية لرواد الفضاء',
            'الإعدادات': 'صفحة الإعدادات قيد التطوير',
            'الاتصال بمركز التحكم': 'جاري الاتصال بمركز التحكم الأرضي...'
        };
        
        const message = messages[linkText] || 'هذه الميزة قيد التطوير';
        alert(message);
    }
    
    // Method to simulate receiving real data from ESP32
    receiveDataFromESP32(data) {
        this.data = { ...this.data, ...data };
        this.updateDisplay();
        this.checkAlerts();
    }
    
    // Method to stop the simulation
    stopSimulation() {
        if (this.updateInterval) {
            clearInterval(this.updateInterval);
            this.updateInterval = null;
        }
    }
}

// Initialize the system when the page loads
document.addEventListener('DOMContentLoaded', () => {
    window.medicalSystem = new MedicalMonitoringSystem();
    
    // Add some console logging for debugging
    console.log('Space Medical Monitoring System initialized');
    console.log('System is simulating real-time data from ESP32 sensors');
    
    // Example of how to receive real data from ESP32
    // window.medicalSystem.receiveDataFromESP32({
    //     bodyTemperature: 36.8,
    //     jointMobility: 85,
    //     cabinTemperature: 23.5,
    //     humidity: 45,
    //     flexSensorRaw: 750
    // });
});

// Export for potential use in other scripts
if (typeof module !== 'undefined' && module.exports) {
    module.exports = MedicalMonitoringSystem;
}
