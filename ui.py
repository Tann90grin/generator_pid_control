import tkinter as tk
from tkinter import ttk, messagebox
import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import threading
import time

class ArduinoPIDGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Arduino PID Temperature Control")

        self.port_label = ttk.Label(root, text="Select COM Port:")
        self.port_label.pack()
        self.port_combo = ttk.Combobox(root, values=self.get_com_ports(), state="readonly")
        self.port_combo.pack()

        self.baud_label = ttk.Label(root, text="Select Baud Rate:")
        self.baud_label.pack()
        self.baud_combo = ttk.Combobox(root, values=["9600", "19200", "38400", "57600", "115200"], state="readonly")
        self.baud_combo.set(str(115200))
        self.baud_combo.pack()

        self.connect_button = ttk.Button(root, text="Connect", command=self.connect_serial)
        self.connect_button.pack(pady=5)

        self.disconnect_button = ttk.Button(root, text="Disconnect", command=self.disconnect_serial)
        self.disconnect_button.pack()
        self.disconnect_button["state"] = "disabled" 

        self.temp_label = ttk.Label(root, text="Current Temp: -- °C", font=("Arial", 14))
        self.temp_label.pack(pady=10)

        self.setpoint_label = ttk.Label(root, text="Set Temp:")
        self.setpoint_label.pack()
        self.setpoint_entry = ttk.Entry(root)
        self.setpoint_entry.pack()

        self.set_button = ttk.Button(root, text="Send Setpoint", command=self.send_setpoint)
        self.set_button.pack(pady=5)

        self.serial_conn = None
        self.running = False
        self.read_thread = None

    def get_com_ports(self):
        ports = serial.tools.list_ports.comports()
        return [port.device for port in ports]

    def connect_serial(self):
        selected_port = self.port_combo.get()
        if not selected_port:
            messagebox.showwarning("Warning", "Please select a COM port.")
            return
        try:
            baud = int(self.baud_combo.get())
            self.serial_conn = serial.Serial(selected_port, baud, timeout=1)
            self.running = True
            self.read_thread = threading.Thread(target=self.read_serial, daemon=True)
            self.read_thread.start()
            self.disconnect_button["state"] = "normal"
            self.connect_button["state"] = "disabled"
            messagebox.showinfo("Connected", f"Connected to {selected_port} at {baud} baud.")
        except serial.SerialException as e:
            messagebox.showerror("Connection Error", f"Could not open port {selected_port}:\n{e}")


    def disconnect_serial(self):
        self.running = False
        if self.serial_conn and self.serial_conn.is_open:
            try:
                self.serial_conn.close()
                self.disconnect_button["state"] = "disabled"
                self.connect_button["state"] = "normal"
                print("Serial connection closed.")
            except Exception as e:
                print(f"Error while closing port: {e}")

    def send_setpoint(self):
        temp = self.setpoint_entry.get()
        try:
            float(temp)
            self.send_command(f"SET_TEMP:{temp}")
        except ValueError:
            messagebox.showerror("Input Error", "Invalid temperature setpoint.")

    def send_command(self, cmd):
        if self.serial_conn and self.serial_conn.is_open:
            self.serial_conn.write((cmd + "\n").encode())

    def read_serial(self):
        while self.running:
            try:
                if self.serial_conn and self.serial_conn.in_waiting:
                    line = self.serial_conn.readline().decode().strip()
                    if line.startswith("TEMP:"):
                        temp_value = line.split(":")[1]
                        self.temp_label.config(text=f"Current Temp: {temp_value} °C")
            except Exception as e:
                print(f"Read error: {e}")
            time.sleep(0.5)

    def on_close(self):
        self.running = False
        if self.serial_conn:
            self.serial_conn.close()
        self.root.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    gui = ArduinoPIDGUI(root)
    root.protocol("WM_DELETE_WINDOW", gui.on_close)
    root.mainloop()
