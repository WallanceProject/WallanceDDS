#!/bin/bash

# Launch WallanceDDS Grafana Interface (PC)
# Command: ./Run_Grafana_Interface.sh



# Init IP Address
MyAddr=$(hostname -I | cut -d ' ' -f1)
sed -i "s/DCPSDefaultAddress=.*/DCPSDefaultAddress=$MyAddr/g" Grafana/rtps.ini
sed -i "s/local_address=.*./local_address=$MyAddr:12345/g" Grafana/rtps.ini

# Init Network Interface
MyInterface=$(ifconfig | grep -B1 "$MyAddr" | awk '$1!="inet" {print $1}' | cut -f1 -d ':')
sed -i "s/MulticastInterface=.*./MulticastInterface=$MyInterface/g" Grafana/rtps.ini
sed -i "s/multicast_interface=.*./multicast_interface=$MyInterface/g" Grafana/rtps.ini

# Init Grafana Server
if [ $(service grafana-server status | grep Active | awk '{print $2}') == "inactive" ]
then
	service grafana-server start
fi

# Init MYSQL
if [ $(service mysql status | grep Active | awk '{print $2}') == "inactive" ]
then
	service mysql start
fi

# Set Shared Libraries
source ../OpenDDS-3.13.2/setenv.sh

# Display Node's ID
echo "-------------------------"
echo "YOU ARE GRAFANA INTERFACE"
echo "-------------------------"

# Start Grafana Interface (Firefox)
/usr/bin/firefox -new-window http://localhost:3000/dashboard/script/WallanceDDS_Dashboard.js &

# Start WallanceDDS Node (Raspberry)
./Start_Node.sh

# Start Grafana Interface process
cd Grafana
./Grafana_Interface.bin -DCPSConfigFile rtps.ini

# Stop Grafana Interface (Firefox)
killall -q firefox

# Stop WallanceDDS Node (Raspberry)
cd ../
./Stop_Node.sh
