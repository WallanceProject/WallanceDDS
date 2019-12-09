#!/bin/bash

# Compile & Export WallanceDDS
# Command: ./Compile_WallanceDDS.sh



# ------------------
# CONSENSUS DATABASE
# ------------------
rm -f Consensus/*.db



# ---------------------------------
# OPENDDS (DDS & GRAFANA INTERFACE)
# ---------------------------------

# SQLite3 Compilation Option
export CFLAGS='-DSQLITE_ENABLE_UPDATE_DELETE_LIMIT=1'

# Clean Projects
rm -f -R OpenDDS/*modelesC* OpenDDS/*modelesS* OpenDDS/*Type* OpenDDS/GNU* OpenDDS/.o* OpenDDS/.d* OpenDDS/*.bin
rm -f -R Grafana/*modelesC* Grafana/*modelesS* Grafana/*Type* Grafana/GNU* Grafana/.o* Grafana/.d* Grafana/*.bin
cp OpenDDS/DDS_modeles.idl Grafana/

# Compile DDS
cd ../OpenDDS-3.13.2
./configure --target=linux-cross --target-compiler=arm-linux-gnueabihf-g++ --verbose --no-tests
make -j4

# Compile Grafana Interface
./configure --verbose --no-tests
make -j4

# Clean Projects
rm -f -R ../WallanceDDS/OpenDDS/*modelesC* ../WallanceDDS/OpenDDS/*modelesS* ../WallanceDDS/OpenDDS/*Type* ../WallanceDDS/OpenDDS/GNU* ../WallanceDDS/OpenDDS/.o* ../WallanceDDS/OpenDDS/.d*
rm -f -R ../WallanceDDS/Grafana/*modelesC* ../WallanceDDS/Grafana/*modelesS* ../WallanceDDS/Grafana/*Type* ../WallanceDDS/Grafana/GNU* ../WallanceDDS/Grafana/.o* ../WallanceDDS/Grafana/.d*

cd ../WallanceDDS/



# ----------------------------------------
# GRAFANA INTERFACE - DASHBOARD PARAMETERS
# ----------------------------------------

# MySQLAccess.sh - MAJORITY_THRESHOLD
Majority_Threshold=$(grep '#define MAJORITY_THRESHOLD' Consensus/Consensus.h | cut -f 5)	# Delimiter: tab (x4 +1)
Numerator=$(echo $Majority_Threshold | cut -f2 -d '(' | cut -f1 -d '/')
Denominator=$(echo $Majority_Threshold | cut -f2 -d '/' | cut -f1 -d ')')
sed -i "s/MAJORITY_THRESHOLD=.*/MAJORITY_THRESHOLD=$Numerator\/$Denominator/g" Grafana/MySQLAccess.sh

# MySQLAccess.sh - DIFFICULTY
Difficulty=$(grep '#define DIFFICULTY' Consensus/Consensus.h | cut -f 7)	# Delimiter: tab (x6 +1)
sed -i "s/DIFFICULTY=.*/DIFFICULTY=$Difficulty/g" Grafana/MySQLAccess.sh


# MySQLAccess.sh - DCOIN_RATE
DCoin_Rate=$(grep '#define DCOIN_RATE' Consensus/Consensus.h | cut -f 7)	# Delimiter: tab (x6 +1)
sed -i "s/DCOIN_RATE=.*/DCOIN_RATE=$DCoin_Rate/g" Grafana/MySQLAccess.sh


# MySQLAccess.sh - DCOIN_REWARD
DCoin_Reward=$(grep '#define DCOIN_REWARD' Consensus/Consensus.h | cut -f 6)	# Delimiter: tab (x5 +1)
sed -i "s/DCOIN_REWARD=.*/DCOIN_REWARD=$DCoin_Reward/g" Grafana/MySQLAccess.sh


# MySQLAccess.sh - GENESIS_STATE
Genesis_State=$(grep '#define GENESIS_STATE' Consensus/Consensus.h | cut -f 6)	# Delimiter: tab (x5 +1)
sed -i "s/GENESIS_STATE=.*/GENESIS_STATE=$Genesis_State/g" Grafana/MySQLAccess.sh


# MySQLAccess.sh - TRANSACTION_OUTDATE
Transaction_Outdate=$(grep '#define TRANSACTION_OUTDATE' Consensus/Consensus.h | cut -f 5)	# Delimiter: tab (x4 +1)
sed -i "s/TRANSACTION_OUTDATE=.*/TRANSACTION_OUTDATE=$Transaction_Outdate/g" Grafana/MySQLAccess.sh


# WallanceDDS_Dashboard.js - DCOIN_RATE
sed -i "s/SELECT .* AS DCOIN_RATE/SELECT $DCoin_Rate AS DCOIN_RATE/g" Grafana/WallanceDDS_Dashboard.js



# --------------
# SMARTCONTRACTS
# --------------

for i in $(ls SmartContract)
do
	rm -f SmartContract/$i/*.bin
	arm-linux-gnueabihf-g++ -o SmartContract/$i/$i.bin SmartContract/$i/$i.cpp #Consensus/*.cpp ../SQLite3/sqlite3.o -I Consensus/ -lpthread -ldl
done



# ------
# SENSOR
# ------
arm-linux-gnueabihf-g++ -o Sensor/Sensor.bin Sensor/Sensor.cpp



# -----
# PIPES
# -----
rm -R -f Pipes
mkdir -p Pipes



# --------------------------
# EXPORT WALLANCE NODE - ARM
# --------------------------

# WallanceDDS Node Service
zip WallanceDDS_Node.zip WallanceDDS_Node.service

# Launcher Scripts
zip -u WallanceDDS_Node.zip Run_Node.sh

# Consensus (Repository Only)
zip -u WallanceDDS_Node.zip Consensus

# OpenDDS (bin, ini)
zip -u WallanceDDS_Node.zip \
OpenDDS/DDS.bin \
OpenDDS/rtps.ini

# SmartContracts
zip -x SmartContract/*/*.c* -x SmartContract/*/*.h \
-x SmartContract/*/*.png \
-u WallanceDDS_Node.zip SmartContract/*/*

# Sensor
zip -u WallanceDDS_Node.zip Sensor/Sensor.bin

# Pipes (Repository Only)
zip -u WallanceDDS_Node.zip Pipes

# OpenDDS (Libs)
zip -u -j WallanceDDS_Node.zip \
../OpenDDS-3.13.2/build/target/ACE_wrappers/ace/libACE.so* \
../OpenDDS-3.13.2/build/target/ACE_wrappers/TAO/tao/libTAO.so* \
../OpenDDS-3.13.2/build/target/lib/libOpenDDS_Dcps.so* \
../OpenDDS-3.13.2/build/target/dds/DCPS/InfoRepoDiscovery/libOpenDDS_InfoRepoDiscovery.so* \
../OpenDDS-3.13.2/build/target/tools/modeling/codegen/model/libOpenDDS_Model.so* \
../OpenDDS-3.13.2/build/target/dds/DCPS/RTPS/libOpenDDS_Rtps.so* \
../OpenDDS-3.13.2/build/target/dds/DCPS/transport/rtps_udp/libOpenDDS_Rtps_Udp.so* \
../OpenDDS-3.13.2/build/target/dds/DCPS/transport/tcp/libOpenDDS_Tcp.so* \
../OpenDDS-3.13.2/build/target/ACE_wrappers/TAO/tao/PortableServer/libTAO_PortableServer.so* \
../OpenDDS-3.13.2/build/target/ACE_wrappers/TAO/tao/PI/libTAO_PI.so* \
../OpenDDS-3.13.2/build/target/ACE_wrappers/TAO/tao/CodecFactory/libTAO_CodecFactory.so* \
../OpenDDS-3.13.2/build/target/ACE_wrappers/TAO/tao/BiDir_GIOP/libTAO_BiDirGIOP.so* \
../OpenDDS-3.13.2/build/target/ACE_wrappers/TAO/tao/AnyTypeCode/libTAO_AnyTypeCode.so*

# Create Install WallanceDDS Node Script
MyCMD='#!/bin/bash\n\n'
echo -e $MyCMD > Install_WallanceDDS_Node.sh
echo -e "sudo apt-get update\nsudo apt-get install -y net-tools" >> Install_WallanceDDS_Node.sh
echo "find \$(pwd) -type f -iname \"*.sh\" -exec chmod +x {} \;" >> Install_WallanceDDS_Node.sh
echo "sudo mv lib* /usr/local/lib" >> Install_WallanceDDS_Node.sh
echo "sudo mv WallanceDDS_Node.service /etc/systemd/system/" >> Install_WallanceDDS_Node.sh
echo "sudo systemctl daemon-reload" >> Install_WallanceDDS_Node.sh
echo "sudo rm -f -R WallanceDDS && mkdir WallanceDDS" >> Install_WallanceDDS_Node.sh
echo "mv Run_Node.sh Consensus OpenDDS SmartContract Sensor Pipes WallanceDDS" >> Install_WallanceDDS_Node.sh
echo "rm -f WallanceDDS_Node.zip" >> Install_WallanceDDS_Node.sh
echo "rm -f Install_WallanceDDS_Node.sh" >> Install_WallanceDDS_Node.sh
chmod +x Install_WallanceDDS_Node.sh
zip -u WallanceDDS_Node.zip Install_WallanceDDS_Node.sh
rm Install_WallanceDDS_Node.sh
