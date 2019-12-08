#include <iostream>
#include <ace/Log_Msg.h>
#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsPublicationC.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include "dds/DCPS/StaticIncludes.h"
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>


#include "Sensor.h"
#include "Consensus.h"

#include "DataReaderListenerImpl_sensor.h"
#include "DataReaderListenerImpl_consensus.h"

#include "DDS_modelesTypeSupportC.h"
#include "DDS_modelesTypeSupportImpl.h"


using namespace std;






/********** PROTOTYPES **********/

void catch_function(int signo);
int DDS_Init(int argc, char **argv);
int DDS_PUB_Topics(void);
int DDS_SUB_Topics(void);
void Publisher_Sensor_Value(int Sensor_value);
void Publisher_Transaction(Transaction MyTX);
void DDS_Close(void);