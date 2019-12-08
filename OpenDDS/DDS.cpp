#include "DDS.h"



/********** GLOBAL VARIABLES **********/

// DDS Variables
DDS::DomainParticipantFactory_var dpf;      // DomainParticipantFactory
DDS::DomainParticipant_var participant;     // DomainParticipant
DDS::Publisher_var pub;                     // Publisher
DDS::Subscriber_var sub;                    // Subscriber

/**** SENSOR TOPICS ****/
// Publisher
DDS_modeles::sensorTypeSupport_var ts_sensor;
DDS::Topic_var topic_sensor;
DDS::DataWriter_var dw_base_sensor;
DDS_modeles::sensorDataWriter_var dw_sensor;

// Subscriber
DDS::DataReader_var dr_sensor;
DDS::DataReaderListener_var listener_sensor(new DataReaderListenerImpl_sensor);


/**** CONSENSUS TOPIC ****/
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

// ID of this Node
extern string MyID;

// SQLite3 Access
extern sqlite3* DB;






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


        CORBA::String_var type_name = ts_sensor->get_type_name();
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
  

        dw_base_sensor =
            pub->create_datawriter(topic_sensor,
                                DATAWRITER_QOS_DEFAULT,
                                0,    // No listener required
                                OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (!dw_base_sensor) {
            std::cerr << "create_datawriter failed." << std::endl;
            return -1;
        }

        dw_sensor = DDS_modeles::sensorDataWriter::_narrow(dw_base_sensor);
        if (!dw_sensor){
            std::cerr << "DataWriter could not be narrowed."<< std::endl;
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
        type_name = ts_consensus->get_type_name();
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



void Publisher_Sensor_Value(int Sensor_value)
{
    try {
        
        // Inits
        setbuf(stdout, NULL);
        DDS_modeles::sensor sensor_1;

        sensor_1.publisher  = TAO::String_Manager(MyID.c_str());
        sensor_1.topic      = "SENSOR";
        sensor_1.value      = Sensor_value;

        /******* NEED TO SWITCH ACCORDING TO THE TOPIC OF SENSOR *******/
        dw_sensor->write(sensor_1, DDS::HANDLE_NIL);
        /***************************************************************/

    } catch (const CORBA::Exception& e) {
        e._tao_print_exception("Exception caught in main():");
    }
}



void Publisher_Transaction(Transaction MyTX)
{
    try {
        
        // Inits
        setbuf(stdout, NULL);
        DDS_modeles::consensus consensus_1;
        
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
    // Reception of Sensor value
    int MySensor;

    // Local Transaction
    Transaction MyTX;

    // Reading Pipes: Sensor -> DDS
    mkfifo(PIPE_SENSOR_TO_DDS, 0777);
    int Pipe_sensor_to_dds = open(PIPE_SENSOR_TO_DDS, O_RDONLY | O_NONBLOCK);
    if (Pipe_sensor_to_dds == -1)
    {
        printf("ERROR OPENNING PIPE SENSOR TO DDS : %s\n", strerror(errno));
        return -1;
    }


    // Init Consensus Database & Tables
    if (Init_WallanceDDS(argv) == -1)
        return -1;


    // Init DDS configs
    DDS_Init(argc, argv);
    DDS_PUB_Topics();
    DDS_SUB_Topics();  

    // Wait for other Participants
    sleep(4);

    // Launch Sensor process
    system("cd ../Sensor && ./Sensor.bin &");

    signal(SIGINT, catch_function);
    while (interrupt_detected != 1)
    {

        // Read Pipe: Data from Sensor to send on network
        if (read(Pipe_sensor_to_dds, &MySensor, sizeof(int)))
            Publisher_Sensor_Value(MySensor);


        while(Generate_Consensus_Transaction(&MyTX) != -1)
        {
            // Send New Transactions
            Publisher_Transaction(MyTX);
        }


        while(Consensus_Process() != -1);

        // Sampling
        sleep(SAMPLING_TIME);
    }

    // Close Pipe, DDS & Database
    close(Pipe_sensor_to_dds);
    DDS_Close();
    sqlite3_close(DB);

    printf("By DDS\n");
    return 0;
}