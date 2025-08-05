from flask import Flask, render_template, request, redirect, url_for
import serial 
from serial import SerialException
import threading
import time
from datetime import datetime

app = Flask(__name__)
LOG_FILE = "logs.txt"
USERNAME = "admin"
PASSWORD = "admin123"

# Serial connection with Arduino
try:
    ser = serial.Serial('COM3', 9600, timeout=1)  # Replace with your COM port
except serial.SerialException:
    print("⚠️ Arduino not connected. Continuing without serial...")
    ser = None


# Background thread to read logs
def read_serial():
    while True:
        if ser.in_waiting:
            line = ser.readline().decode().strip()
            if "ACCESS_GRANTED" in line:
                method = line.split(":")[1]
                with open(LOG_FILE, "a") as f:
                    f.write(f"{datetime.now()}, {method}\n")
        time.sleep(0.5)

threading.Thread(target=read_serial, daemon=True).start()

@app.route("/", methods=["GET", "POST"])
def login():
    if request.method == "POST":
        if request.form["username"] == USERNAME and request.form["password"] == PASSWORD:
            return redirect(url_for("dashboard"))
    return '''
        <form method="post">
            <input name="username" placeholder="Username">
            <input name="password" type="password" placeholder="Password">
            <button type="submit">Login</button>
        </form>
    '''

@app.route("/dashboard")
def dashboard():
    try:
        with open(LOG_FILE, "r") as f:
            logs = f.readlines()
    except FileNotFoundError:
        logs = []
    logs = [log.strip().split(", ") for log in logs]
    return render_template("dashboard.html", logs=logs)

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)
