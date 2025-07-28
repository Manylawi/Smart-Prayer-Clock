# 🕋 Smart Prayer Clock System

This project presents the design and implementation of a **Smart Prayer Clock** combining real-time Islamic functionality, environmental monitoring, and smart technologies. The system is built using **ATmega32**, multiple **7-segment displays**, and peripheral modules such as **Bluetooth**, **PIR**, **RTC**, and **audio output**, forming an integrated solution for both timekeeping and religious observance.

---

## 👨‍🎓 Team Members

- Zeyad Tarek 
- Ahmed El-Manylawi  
- Ahmed Sief
- Ahmed Ayman
- Sarah Hesham 

**Supervised by**: Dr. Hossam Eldin Ali  
**Department**: Intelligent Systems Engineering  
**Faculty**: Faculty of Engineering – Helwan National University  
**Year**: 2025

---

## 📘 Project Overview

The **Smart Prayer Clock System** provides:
- Accurate **Gregorian and Hijri date tracking**
- **Daily Islamic prayer time display**
- **Adhan audio playback**
- **Bluetooth mobile app connectivity**
- **Temperature monitoring**
- **PIR-based power saving**
- **Touch-based control**
- And more — all through an embedded system built from the ground up.

---

## 🔑 Key Features

### 🕓 Time & Date Display
- Displays **current time** in hours and minutes
- Supports **Gregorian** and **Hijri** calendars with offset configuration

### 🙏 Prayer Times & Adhan
- Displays five daily prayer times: Fajr, Dhuhr, Asr, Maghrib, Isha
- **Adhan audio** played automatically using an ISD sound module and speaker

### 🌡️ Temperature Display
- Uses the **DS3231 RTC module's built-in sensor** to show real-time temperature

### 📱 Bluetooth Control (HC-05)
- Mobile app can:
  - Update time & date
  - Configure prayer times
  - Set alarms
  - Toggle **summer/winter time**
  - Offset Hijri calendar
  - Enable/disable **PIR power saving**

### ⏰ Alarm & Touch Button
- Settable alarm via Bluetooth
- **Capacitive touch sensor** to stop Adhan/alarm

### 💤 Power Saving with PIR Sensor
- Detects motion to **activate/deactivate** display
- Conserves energy when no user is present

---

## 🔩 Components Used

| Component                | Description                                      |
|--------------------------|--------------------------------------------------|
| ATmega32                 | Main microcontroller                             |
| 18x 7-Segment Displays   | 1” & 0.8” displays for time, date, temperature   |
| 74HC595N Shift Registers | Control multiple 7-segment displays              |
| DS3231 RTC Module        | Keeps time and provides temperature              |
| ISD Audio Module         | Stores and plays Adhan sound                     |
| PIR Sensor               | Detects motion for power saving                  |
| Bluetooth Module (HC-05) | For mobile app control                           |
| Capacitive Touch Sensor  | For manual user input                            |
| LEDs & Resistors         | Indicators and protection                        |
| Buzzer                   | Alerts and notification sounds                   |

---

## 🧠 System Functionality

### 👨‍💻 ATmega32
- Controls all peripherals
- Manages display, audio, timekeeping, and communication

### 📺 7-Segment + Shift Registers
- 18 displays driven via shift registers for efficient pin usage

### ⏲️ DS3231 RTC
- Maintains time and date even during power loss (with backup battery)
- Outputs accurate temperature readings

### 🔊 ISD Audio Module
- Plays pre-recorded Adhan at the correct prayer time through speaker

### 🔄 PIR Sensor
- Enables smart power-saving based on motion

### 🔗 Bluetooth Integration
- Fully controlled using a custom mobile app via HC-05

### 👆 Touch Sensor
- Used to stop Adhan or alarm manually with a single tap

---

## 🖼️ Circuit Diagram

> Please refer to the attached schematic (Proteus or PDF) in the repository.

---

## 📦 Cost Estimate

**Total cost**: ~1000 EGP (as of 2025)

---

## 🛠️ Tools Used

- **AVR Microcontroller (ATmega32)**  
- **Proteus** for simulation  
- **AVR-GCC / Atmel Studio** for programming  
- **MIT App Inventor / Bluetooth Terminal** for control  

---

## 📬 Contact

**Ahmed El-Manylawi**  
📧 Email: ahmed.elmanylawi@gmail.com  
🔗 [LinkedIn](https://www.linkedin.com/in/ahmed-el-manylawi-67b6162aa)

---

## ⚠️ Legal Notice

> © 2025 Ahmed El-Manylawi and Team.  
> All rights reserved.

This project, its design, documentation, code, and all associated materials are the **intellectual property of the authors**.  
**No part of this work may be copied, reused, published, or modified** without **explicit written permission**.

This documentation is shared for **academic and educational showcase purposes only** and **may not be used** in personal, commercial, or academic projects without authorization.

---

