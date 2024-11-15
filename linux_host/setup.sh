#!/bin/bash

# Define the environment and requirements
ENV_DIR="venv"
REQUIREMENTS_FILE="requirements.txt"

# Ensure Python 3 is installed
if ! command -v python3 &> /dev/null; then
    echo "Python 3 is not installed. Please install it before running this script."
    exit 1
fi

# Create virtual environment
echo "Creating virtual environment..."
python3 -m venv $ENV_DIR || { echo "Failed to create virtual environment."; exit 1; }

# Activate and install dependencies
source $ENV_DIR/bin/activate
if [ -f "$REQUIREMENTS_FILE" ]; then
    echo "Installing dependencies from $REQUIREMENTS_FILE..."
    pip install -r $REQUIREMENTS_FILE || { echo "Failed to install dependencies."; exit 1; }
else
    echo "Error: $REQUIREMENTS_FILE not found."
    exit 1
fi

# Create systemd service file
SERVICE_FILE="/etc/systemd/system/vfd_display.service"
echo "Creating systemd service file at $SERVICE_FILE..."
echo "[Unit]
Description=Run main.py script to display system info on VFD display
After=network.target

[Service]
Type=simple
WorkingDirectory=$(pwd)
ExecStart=$(pwd)/$ENV_DIR/bin/python $(pwd)/main.py
Restart=always
User=$(whoami)
Environment=PYTHONUNBUFFERED=1

[Install]
WantedBy=multi-user.target" | sudo tee $SERVICE_FILE > /dev/null || { echo "Failed to create service file."; exit 1; }

# Reload systemd, enable and start the service
echo "Reloading systemd, enabling and starting the service..."
sudo systemctl daemon-reload || { echo "Failed to reload systemd."; exit 1; }
sudo systemctl enable vfd_display.service || { echo "Failed to enable the service."; exit 1; }
sudo systemctl start vfd_display.service || { echo "Failed to start the service."; exit 1; }

echo "Setup and service installation completed successfully!"

# sudo systemctl enable vfd_display.service
# sudo systemctl start vfd_display.service
# sudo systemctl status vfd_display.service
