#!/bin/bash

# Launch WallanceDDS Node (Raspberry)
# Command: ./Run_Node.sh



# Init IP Address
MyAddr=$(hostname -I | cut -d ' ' -f1)
sed -i "s/DCPSDefaultAddress=.*/DCPSDefaultAddress=$MyAddr/g" OpenDDS/rtps.ini
sed -i "s/local_address=.*./local_address=$MyAddr:12345/g" OpenDDS/rtps.ini

# Init Network Interface
MyInterface=$(ifconfig | grep -B1 "$MyAddr" | awk '$1!="inet" {print $1}' | cut -f1 -d ':')
sed -i "s/MulticastInterface=.*./MulticastInterface=$MyInterface/g" OpenDDS/rtps.ini
sed -i "s/multicast_interface=.*./multicast_interface=$MyInterface/g" OpenDDS/rtps.ini

# Remove Previous Consensus
rm -f Consensus/*.db

# Set Shared Libraries
export LD_LIBRARY_PATH="/usr/local/lib"

# Display Node's ID
echo "--------------------------------"
echo "YOUR NODE IS $(echo $MyAddr | cut -d '.' -f4) ($MyAddr)"
echo "--------------------------------"

# Launch OpenDDS process
cd OpenDDS
./DDS.bin $(echo $MyAddr | cut -d '.' -f4) -DCPSConfigFile rtps.ini