# 🧊 RFID Smart Refrigerator Access & Cooling Control System (ESP32)

An intelligent **RFID-based refrigerator system** built using ESP32.  
The system provides **secure access using RFID cards** and allows **manual cooling control using a potentiometer**.

---

## 📌 Features

- 🔐 RFID-based secure access system  
- 🚪 Automatic door lock/unlock using relay  
- 📡 UID-based card authentication  
- 🌬️ Adjustable cooling using potentiometer  
- 🌀 Dual fan control with PWM  
- 📟 LCD display for real-time feedback  
---

## 🔌 Pin Configuration

| Component        | Pin |
|-----------------|-----|
| RFID SDA (SS)   | 4   |
| RFID SCK        | 18  |
| RFID MISO       | 19  |
| RFID MOSI       | 23  |
| RFID RST        | 5   |
| Relay           | 2   |
| Fan1 PWM        | 27  |
| Fan2 PWM        | 26  |
| Fan1 IN1        | 25  |
| Fan1 IN2        | 33  |
| Fan2 IN1        | 32  |
| Fan2 IN2        | 14  |
| Potentiometer   | 34  |
| LCD SDA         | 21  |
| LCD SCL         | 22  |

---

## 🔐 RFID Access System

- Only authorized cards can unlock the refrigerator  
- System reads RFID UID and compares with stored IDs  

### ✅ Access Granted
- Relay unlocks the door  
- LCD displays **Access Granted**

### ❌ Access Denied
- Door remains locked  
- LCD displays **Access Denied**

---

## 🌬️ Cooling Control System

- Potentiometer reads analog values (0–4095)  
- Values mapped to PWM (0–255)  
- Controls speed of both cooling fans  

---

## ⚙️ System Workflow

1. System starts and shows **System Ready**  
2. User scans RFID card  
3. If authorized → Door unlocks  
4. If not → Access denied  
5. Fan speed controlled by potentiometer  

---

## 🔒 Authorized Cards

```cpp
byte authorizedUIDs[][4] = {
  {0x62, 0x50, 0x2C, 0x3C},
  {0x53, 0x24, 0x20, 0x14}
};
---

## 🏢 Developed By

**Mohamed Amiin**

---

## 📜 License

Open-source for educational use.
