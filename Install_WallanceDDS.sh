#!/bin/bash

# Install WallanceDDS
# Command: ./Install_WallanceDDS.sh



# -----------------
# LINUX ENVIRONMENT
# -----------------

# Update Ubuntu
sudo apt-get update

# Install modules
sudo apt-get install -y net-tools \
openssh-server \
wget \
nmap \
zip

# Create Wallance Project Directory
mkdir -p $HOME/WallanceProject



# ------------
# SUBLIME TEXT
# ------------

sudo apt-get install -y gnupg
sudo apt-get install -y libgtk2.0-0
sudo wget -qO - https://download.sublimetext.com/sublimehq-pub.gpg | sudo apt-key add -
sudo echo "deb https://download.sublimetext.com/ apt/stable/" | sudo tee /etc/apt/sources.list.d/sublime-text.list
sudo apt-get update
sudo apt-get install -y sublime-text



# --------------
# OPENDDS-3.13.2
# --------------

# Install tools
sudo apt-get install -y curl \
g++ \
make \
perl-base \
perl-modules \
gcc-arm-linux-gnueabihf \
g++-arm-linux-gnueabihf \
gcc-arm-linux-gnueabi \
g++-arm-linux-gnueabi



# Download OpenDDS-3.13.2
cd $HOME/WallanceProject
wget https://github.com/objectcomputing/OpenDDS/releases/download/DDS-3.13.2/OpenDDS-3.13.2.tar.gz
tar xzvf OpenDDS-3.13.2.tar.gz
rm OpenDDS-3.13.2.tar.gz

# Cross Compile OpenDDS for ARM architecture
cd $HOME/WallanceProject/OpenDDS-3.13.2
./configure --target=linux-cross --target-compiler=arm-linux-gnueabihf-g++ --verbose --no-tests
make -j4

# Compile OpenDDS for x86_64 architecture
./configure --verbose --no-tests
make -j4



# -------
# SQLITE3
# -------

# Install SQLite3 - Command line (for IDE only)
cd $HOME/WallanceProject
sudo apt-get install -y sqlite3

# Install SQLite3 - C/C++ Libraries
export DEBIAN_FRONTEND=noninteractive
sudo apt-get install -y tclsh
wget https://www.sqlite.org/src/zip/sqlite.zip
unzip sqlite.zip -d SQLite3
mv SQLite3/sqlite/* SQLite3/
rm -r SQLite3/sqlite
rm sqlite.zip

# SQLite3 Compilation Option
export CFLAGS='-DSQLITE_ENABLE_UPDATE_DELETE_LIMIT=1'

# Compile SQLite3 for ARM architecture
cd $HOME/WallanceProject/SQLite3
./configure --host=arm-linux CC=arm-linux-gnueabihf-gcc
make clean && make



# -----------------
# GRAFANA INTERFACE
# -----------------

# Install Grafana Interface - Install MySQL
cd $HOME/WallanceProject
sudo apt-get install -y mysql-server
sudo service mysql start
sudo mysql -e "CREATE USER 'grafanaReader'@'localhost'"
sudo mysql -e "GRANT ALL PRIVILEGES ON *.* TO 'grafanaReader'@'localhost';"
sudo mysql -e "FLUSH PRIVILEGES;"

# Install Grafana Interface - Install Web Browser (Firefox)
sudo apt-get install -y firefox

# Install Grafana Interface - Install Grafana (Default Login/Password: admin/admin)
wget https://dl.grafana.com/oss/release/grafana_5.4.3_amd64.deb
sudo dpkg -i grafana_5.4.3_amd64.deb
rm grafana_5.4.3_amd64.deb



# -----------
# WALLANCEDDS
# -----------

# Download WallanceDDS
cd $HOME/WallanceProject
rm -R -f $HOME/WallanceProject/WallanceDDS
wget https://github.com/WallanceProject/WallanceDDS/archive/master.zip -O WallanceDDS.zip

# Unzip WallanceDDS
unzip WallanceDDS.zip
mv WallanceDDS-master WallanceDDS
rm WallanceDDS.zip

# Enable Execution Permission of all Scripts
find $HOME/WallanceProject/WallanceDDS/ -type f -iname "*.sh" -exec chmod +x {} \;

# Add Wallance Repository into OpenDDS Compiler
rm -f OpenDDS-3.13.2/DDS_no_tests.mwc
rm -f OpenDDS-3.13.2/build/target/DDS_TAOv2.mwc
echo -e "workspace{\\n\\t$HOME/WallanceProject/WallanceDDS/OpenDDS/\\n}" >> OpenDDS-3.13.2/build/target/DDS_TAOv2.mwc
echo -e "workspace{\\n\\t$HOME/WallanceProject/WallanceDDS/Grafana/\\n}" >> OpenDDS-3.13.2/DDS_no_tests.mwc

# Install Grafana Interface
sudo cp $HOME/WallanceProject/WallanceDDS/Grafana/WallanceDDS_DataSource.yaml /etc/grafana/provisioning/datasources/
sudo cp $HOME/WallanceProject/WallanceDDS/Grafana/WallanceDDS_Dashboard.js /usr/share/grafana/public/dashboards/

# Install Grafana Interface Buying Request Generator
sudo cp $HOME/WallanceProject/WallanceDDS/Grafana/Grafana_Interface.desktop /usr/share/applications/
sudo apt-get install -y desktop-file-utils
sudo update-desktop-database

# Compile WallanceDDS
cd $HOME/WallanceProject/WallanceDDS
./Compile_WallanceDDS.sh