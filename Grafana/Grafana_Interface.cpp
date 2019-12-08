#include "Grafana_Interface.h"

#include <ace/Log_Msg.h>
#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DdsDcpsPublicationC.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/Service_Participant.h>
#include "dds/DCPS/StaticIncludes.h"
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "DataReaderListenerImpl_sensor.h"
#include "DataReaderListenerImpl_consensus.h"

#include "DDS_modelesTypeSupportC.h"
#include "DDS_modelesTypeSupportImpl.h"


/********** GLOBAL VARIABLES **********/

// DDS Variables
DDS::DomainParticipantFactory_var dpf;      // DomainParticipantFactory
DDS::DomainParticipant_var participant;     // DomainParticipant
DDS::Publisher_var pub;                     // Publisher
DDS::Subscriber_var sub;                    // Subscriber

/**** SENSOR ****/
// Subscriber
DDS_modeles::sensorTypeSupport_var ts_sensor;
DDS::Topic_var topic_sensor;
DDS::DataReader_var dr_sensor;
DDS::DataReaderListener_var listener_sensor(new DataReaderListenerImpl_sensor);


/**** CONSENSUS ****/
// Publisher
DDS_modeles::consensusTypeSupport_var ts_consensus;
DDS::Topic_var topic_consensus;
DDS::DataWriter_var dw_base_consensus;
DDS_modeles::consensusDataWriter_var dw_consensus;

// Subscriber
DDS::DataReader_var dr_consensus;
DDS::DataReaderListener_var listener_consensus(new DataReaderListenerImpl_consensus);



// Override of SIGINT signal to stop the execution of the program
int interrupt_detected = 0;






/********** PROTOTYPE DEFINITIONS **********/

void catch_function(int signo) {
    printf("\nInterrupt SIGINT detected. Terminating process...\n");
    interrupt_detected = 1;
}



int DDS_Init(int argc, char **argv) {
    
    try {
        // Initialize DomainParticipantFactory
        dpf = TheParticipantFactoryWithArgs(argc, argv);

        // Create DomainParticipant
        participant = dpf->create_participant(42, // domain ID - arbitrary
                                        PARTICIPANT_QOS_DEFAULT,
                                        0,  // No listener required
                                        OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!participant) {
            std::cerr << "create_participant failed." << std::endl;
            return -1;
        }

        // Create Publisher
        pub = participant->create_publisher(PUBLISHER_QOS_DEFAULT,
                                        0,    // No listener required
                                        OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!pub) {
            std::cerr << "create_publisher failed." << std::endl;
            return -1;
        }

        // Create Subscriber
        sub = participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
                                        0,    // No listener required
                                        OpenDDS::DCPS::DEFAULT_STATUS_MASK);

        if (!sub) {
            std::cerr << "create_subscriber failed." << std::endl;
            return -1;
        }

    } catch (const CORBA::Exception& e) {
        e._tao_print_exception("Exception caught in main():");
        return -1;
    }

    return 0;
}



int DDS_PUB_Topics(void)
{
    try {
    //================================================================
    // Register TypeSupport and create associated topics and datawriters

        //------------------------------------------------------------------
        // Sensor
        ts_sensor = new DDS_modeles::sensorTypeSupportImpl();
        if (DDS::RETCODE_OK != ts_sensor->register_type(participant, "")) {
            std::cerr << "register_type failed." << std::endl;
            return -1;
        }


        CORBA::String_var type_name = ts_sensor->get_type_name ();
        topic_sensor =
                participant->create_topic ("Sensor",
                            type_name,
                            TOPIC_QOS_DEFAULT,
                            0,   // No listener required
                            OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (!topic_sensor) {
            std::cerr << "create_topic failed." << std::endl;
            return -1;
        }



        //------------------------------------------------------------------
        // Consensus
        ts_consensus = new DDS_modeles::consensusTypeSupportImpl();
        if (DDS::RETCODE_OK != ts_consensus->register_type(participant, "")) {
            std::cerr << "register_type failed." << std::endl;
            return -1;
        }


        //CORBA::String_var type_name = ts_consensus->get_type_name ();
        type_name = ts_consensus->get_type_name ();
        topic_consensus =
                participant->create_topic ("Consensus",
                            type_name,
                            TOPIC_QOS_DEFAULT,
                            0,   // No listener required
                            OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (!topic_consensus) {
            std::cerr << "create_topic failed." << std::endl;
            return -1;
        }
  

        dw_base_consensus =
            pub->create_datawriter(topic_consensus,
                                DATAWRITER_QOS_DEFAULT,
                                0,    // No listener required
                                OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (!dw_base_consensus) {
            std::cerr << "create_datawriter failed." << std::endl;
            return -1;
        }

        dw_consensus = DDS_modeles::consensusDataWriter::_narrow(dw_base_consensus);
        if (!dw_consensus){
            std::cerr << "DataWriter could not be narrowed."<< std::endl;
            return -1;
        }



    } catch (const CORBA::Exception& e) {
        e._tao_print_exception("Exception caught in main():");
        return -1;
    }

    return 0;
}



int DDS_SUB_Topics(void)
{
    try {
    //================================================================
        // Register TypeSupport and create associated topics and datareaders

        //------------------------------------------------------------------
        // Sensor
        dr_sensor =
            sub->create_datareader(topic_sensor,
                                    DATAREADER_QOS_DEFAULT,
                                    listener_sensor,
                                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (!dr_sensor) {
            std::cerr << "create_datareader failed." << std::endl;
            return -1;
        }


        //------------------------------------------------------------------
        // Consensus
        dr_consensus =
            sub->create_datareader(topic_consensus,
                                    DATAREADER_QOS_DEFAULT,
                                    listener_consensus,
                                    OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (!dr_consensus) {
            std::cerr << "create_datareader failed." << std::endl;
            return -1;
        }



    } catch (const CORBA::Exception& e) {
        e._tao_print_exception("Exception caught in main():");
        return -1;
    }

    return 0;
}



void Publisher_Request_Transaction(char* MyCMD)
{
    try {
        
        // Inits
        setbuf(stdout, NULL);
        DDS_modeles::consensus consensus_1;

        Transaction MyTX = {.Subscriber={0}, .Publisher={0}, .SmartContract={0}, .Price=-1, .Time=-1, .PrevState={0}, .DCoin=-1, .Nonce=-1};
        
        // Parse MyCMD
        char *Token;

        Token = strtok(MyCMD, "/_.\n");
        MyTX.SmartContract = Token;

        Token = strtok(NULL, "/_.\n");
        MyTX.Price = atoi(Token);
        
        Token = strtok(NULL, "/_.\n");
        MyTX.Subscriber = Token;
        MyTX.Publisher = Token;

        Token = strtok(NULL, "/_.\n");
        MyTX.PrevState = Token;
        
        MyTX.Time = time(NULL);
        MyTX.DCoin = -1;
        MyTX.Nonce = Compute_Light_PoW(MyTX);

        consensus_1.subscriber      = TAO::String_Manager(MyTX.Subscriber.c_str());
        consensus_1.publisher       = TAO::String_Manager(MyTX.Publisher.c_str());
        consensus_1.smartcontract   = TAO::String_Manager(MyTX.SmartContract.c_str());
        consensus_1.price           = MyTX.Price;
        consensus_1.time            = MyTX.Time;
        consensus_1.prevstate       = TAO::String_Manager(MyTX.PrevState.c_str());
        consensus_1.dcoin           = MyTX.DCoin;
        consensus_1.nonce           = MyTX.Nonce;

        // Send Transaction to the Network
        dw_consensus->write(consensus_1, DDS::HANDLE_NIL);

    } catch (const CORBA::Exception& e) {
        e._tao_print_exception("Exception caught in main():");
    }
}



void DDS_Close(void)
{
    participant->delete_contained_entities();
    dpf->delete_participant(participant);
    TheServiceParticipant->shutdown();
}






/********** MAIN PART **********/

int main (int argc, char **argv)
{

    char MyCMD[DATA_FORMATING_LENGTH];

    // Reading Pipes: Web Interface -> Grafana Interface
    FILE* Pipe_Web_to_Grafana;
    fclose(fopen(PIPE_WEB_INTERFACE_TO_GRAFANA, "w"));
    Pipe_Web_to_Grafana = fopen(PIPE_WEB_INTERFACE_TO_GRAFANA, "r");
    if (Pipe_Web_to_Grafana == NULL)
    {
        printf("ERROR OPENNING PIPE WEB INTERFACE TO GRAFANA : %s\n", strerror(errno));
        return -1;
    }


    // Init DDS configs
    DDS_Init(argc, argv);
    DDS_PUB_Topics();
    DDS_SUB_Topics();

    // Init Majoritychain Database & Tables  
    system("./MySQLAccess.sh 0");

    signal(SIGINT, catch_function);
    while (interrupt_detected != 1)
    {
        // Read Pipe: Data from Grafana Interface to send on network
        while (fgets(MyCMD, sizeof(MyCMD), Pipe_Web_to_Grafana) != NULL)
        {
            Publisher_Request_Transaction(MyCMD);
        }

        // Consensus
        system("./MySQLAccess.sh 4");        

        // Sampling
        sleep(SAMPLING_TIME);
    }

    fclose(Pipe_Web_to_Grafana);
    DDS_Close();

    // Clear Grafana Database & Tables
    system("./MySQLAccess.sh 6");

    printf("By Grafana Interface\n");
    return 0;
}