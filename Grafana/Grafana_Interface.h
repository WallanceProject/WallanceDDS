#include <iostream>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "Consensus.h"

using namespace std;

// Communication between Grafana Web Interface & Grafana Interface bin
#define PIPE_WEB_INTERFACE_TO_GRAFANA   "../Pipes/Pipe_Web_to_Grafana"






/********** PROTOTYPES **********/

void catch_function(int signo);
int DDS_Init(int argc, char **argv);
int DDS_PUB_Topics(void);
int DDS_SUB_Topics(void);
void Publisher_Request_Transaction(char* MyCMD);
void DDS_Close(void);