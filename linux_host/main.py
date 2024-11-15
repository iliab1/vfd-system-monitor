import serial
import serial.tools.list_ports
import time
import psutil
import gpustat

BAUD_RATE = 9600

def find_serial_port():
    """Automatically find the serial port."""
    ports = serial.tools.list_ports.comports()
    for port in ports:
        # Look for port description or device name that matches your board
        # port.description.startswith("Espressif_USB")
        # "ttyACM" in port.device or "ttyUSB" in port.device
        if "Espressif_USB" in port.description or "ttyACM" in port.device or "ttyUSB" in port.device:
            print(f"Detected serial port: {port.device}")
            return port.device
    raise serial.SerialException("No suitable serial port found. Please check your device connection.")

def get_system_info():
    """Fetch system information."""
    cpu_usage = psutil.cpu_percent(interval=1)
    cpu_temp = next((sensor.current for sensor in psutil.sensors_temperatures().get("asusec", []) if sensor.label == "CPU Package"), None)
    gpu_usage = gpustat.GPUStatCollection.new_query()[0].utilization
    gpu_temp = gpustat.GPUStatCollection.new_query()[0].temperature
    memory_usage = psutil.virtual_memory().percent
    swap_usage = psutil.swap_memory().percent

    return (
        f"CPU:{f'{cpu_usage:.0f}%':<4}{f'{cpu_temp:.0f}C':<4}"
        f"MEM:{f'{memory_usage:.0f}%':<4}"
        f"GPU:{f'{gpu_usage}%':<4}"
        f"{f'{gpu_temp}C':<4}"
        f"SWP:{f'{swap_usage:.0f}%':<4}"
    )

def send_to_arduino(data, port):
    """Send a string to Arduino via Serial."""
    try:
        with serial.Serial(port, BAUD_RATE, timeout=1) as ser:
            time.sleep(0.5)
            ser.write(f"{data}\n".encode())
    except serial.SerialException as e:
        print(f"Error communicating with Arduino: {e}")

if __name__ == "__main__":
    # Find the serial port
    try:
        serial_port = find_serial_port()
    except serial.SerialException as e:
        print(e)
        exit(1)

    # Main loop
    while True:
        send_to_arduino(get_system_info(), serial_port)