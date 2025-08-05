#  Biometric Door Lock System (Arduino + Raspberry Pi)

A secure multi-user door locking system using Arduino and Raspberry Pi with fingerprint sensor and passkey-based access. The Raspberry Pi hosts a web interface for access logs and admin controls.

---

##  Hardware Requirements

| Component                | Description                               |
|--------------------------|-------------------------------------------|
| Arduino Uno/Mega         | Microcontroller to control locking system |
| Raspberry Pi (3/4)       | For web interface and logging             |
| R305 Fingerprint Sensor  | Biometric authentication                  |
| 4x4 Matrix Keypad        | Passkey input                             |
| 16x2 LCD Display (I2C)   | UI for user interaction                   |
| SG90 Servo Motor         | Physical lock/unlock mechanism            |
| Breadboard + Jumper Wires| Wiring connections                        |
| 5V Power Supply          | To power Arduino and peripherals          |
| USB Cable                | To connect Arduino to Raspberry Pi        |

---

##  Features

-  Unlock with fingerprint or keypad-based passkey
-  Fingerprint enrollment with unique ID storage
-  Persistent user access using EEPROM
-  Web-based dashboard hosted on Raspberry Pi
-  Real-time access logs with timestamps
-  Admin login protection

---

##  Setup Instructions

### 1. Arduino Setup

- Upload the `arduino/door_lock.ino` code using Arduino IDE.
- Ensure the following pin connections:

  | Arduino Pin | Connected To         |
  |-------------|----------------------|
  | D2, D3      | R305 Fingerprint (TX, RX) |
  | A0-A3       | Keypad row pins       |
  | D4-D7       | Keypad column pins    |
  | D9          | Servo Motor (signal)  |
  | SDA, SCL    | I2C LCD module        |

- Open Serial Monitor @9600 baud rate to debug.

---

### 2. Raspberry Pi Setup

- Clone this repo:

bash:
git clone https://github.com/<your-username>/biometric-door-lock.git
cd biometric-door-lock/flask_server

Install Python dependencies:
bash:
sudo apt update
sudo apt install python3-pip
pip3 install flask pyserial

Connect Arduino via USB (check with ls /dev/ttyUSB*).

Edit app.py and update:
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
(change to your detected port if needed)

Run the Flask app:
python3 app.py
Visit http://<raspberry-pi-ip>:5000 on your browser.

default crdentials:
| Username | Password |
| admin    | admin123 |

