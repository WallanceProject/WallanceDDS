#!/bin/bash

# Stop WallanceDDS Node (Raspberry)
# Command: ./Stop_Node.sh

# To avoid to type the password, install the RSA Public/Private Key
# Generate RSA Public/Private Key: ssh-keygen -t rsa # ENTER to every field
# Copy Key to target: ssh-copy-id pi@ipaddr

# Find All Node IPs
IPs=$(sudo nmap -n -sn $(ip -o -f inet addr show | awk '{print $4}' | grep '192') | awk '/Nmap scan report for/{printf $5;}/MAC Address:/{print " => "$3;}' | grep "B8:27:EB" | awk '{print $1}')

for ip in $IPs
do
	echo "********** Stop Node $ip **********"
	ssh pi@$ip 'sudo service WallanceDDS_Node stop' >> /dev/null &
done
