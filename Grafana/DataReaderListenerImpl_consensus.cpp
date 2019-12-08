/*
 *
 *
 * Distributed under the OpenDDS License.
 * See: http://www.opendds.org/license.html
 */

#include "Grafana_Interface.h"
#include <ace/OS_NS_stdlib.h>
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

#include "DataReaderListenerImpl_consensus.h"

#include "DDS_modelesTypeSupportC.h"
#include "DDS_modelesTypeSupportImpl.h"
/*
#include <iostream>

#include <ace/Log_Msg.h>
#include <ace/OS_NS_stdlib.h>

#include "DataReaderListenerImpl_consensus.h"
#include "DDS_modelesTypeSupportC.h"
#include "DDS_modelesTypeSupportImpl.h"
*/

void
DataReaderListenerImpl_consensus::on_requested_deadline_missed(DDS::DataReader_ptr /*reader*/,
  const DDS::RequestedDeadlineMissedStatus& /*status*/)
{
}

void
DataReaderListenerImpl_consensus::on_requested_incompatible_qos(
  DDS::DataReader_ptr /*reader*/,
  const DDS::RequestedIncompatibleQosStatus& /*status*/)
{
}

void
DataReaderListenerImpl_consensus::on_sample_rejected(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SampleRejectedStatus& /*status*/)
{
}

void
DataReaderListenerImpl_consensus::on_liveliness_changed(
  DDS::DataReader_ptr /*reader*/,
  const DDS::LivelinessChangedStatus& /*status*/)
{
}

void
DataReaderListenerImpl_consensus::on_data_available(DDS::DataReader_ptr reader){

  DDS_modeles::consensusDataReader_var dr_consensus = DDS_modeles::consensusDataReader::_narrow(reader);
  if (!dr_consensus) {
    std::cerr << "read: _narrow failed." << std::endl;
    return;
  }

  DDS_modeles::consensus message;
  DDS::SampleInfo si ;
  DDS::ReturnCode_t status = dr_consensus->take_next_sample(message, si) ;
  if (status == DDS::RETCODE_OK) {

    if (si.valid_data == 1) {

      /**** LD EDIT ****/
      char MyCMD[DATA_FORMATING_LENGTH];
      Transaction ConsTX;

      ConsTX.Subscriber    = message.subscriber;
      ConsTX.Publisher     = message.publisher;
      ConsTX.SmartContract = message.smartcontract;
      ConsTX.Price         = message.price;
      ConsTX.Time          = message.time;
      ConsTX.PrevState     = message.prevstate;
      ConsTX.DCoin         = message.dcoin;
      ConsTX.Nonce         = message.nonce;

      // Check Light PoW
      if (Check_Light_PoW(ConsTX) == 0)
      {
        // Add New Buying Request
        if (ConsTX.Subscriber == ConsTX.Publisher)
          snprintf(MyCMD, DATA_FORMATING_LENGTH, "./MySQLAccess.sh 2 %s %s %s %d %d %s %d %d", ConsTX.Subscriber.c_str(), ConsTX.Publisher.c_str(), ConsTX.SmartContract.c_str(), ConsTX.Price, ConsTX.Time, ConsTX.PrevState.c_str(), ConsTX.DCoin, ConsTX.Nonce);
          
        // Add New Consensus Response
        else
          snprintf(MyCMD, DATA_FORMATING_LENGTH, "./MySQLAccess.sh 3 %s %s %s %d %d %s %d %d", ConsTX.Subscriber.c_str(), ConsTX.Publisher.c_str(), ConsTX.SmartContract.c_str(), ConsTX.Price, ConsTX.Time, ConsTX.PrevState.c_str(), ConsTX.DCoin, ConsTX.Nonce);
          
        system(MyCMD);
      }
  
    } else if (si.instance_state == DDS::NOT_ALIVE_DISPOSED_INSTANCE_STATE){
      std::cout << "instance is disposed" << std::endl;
    } else if (si.instance_state == DDS::NOT_ALIVE_NO_WRITERS_INSTANCE_STATE){
      std::cout << "instance is unregistered" << std::endl;
    } else {
      std::cerr << "ERROR: received unknown instance state " << si.instance_state << std::endl;
    }
  } else if (status == DDS::RETCODE_NO_DATA) {
    std::cerr << "ERROR: reader received DDS::RETCODE_NO_DATA!" << std::endl;
  } else {
    std::cerr << "ERROR: read Message: Error: " <<  status << std::endl;
  }

}

void
DataReaderListenerImpl_consensus::on_subscription_matched(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SubscriptionMatchedStatus& /*status*/)
{
}

void
DataReaderListenerImpl_consensus::on_sample_lost(
  DDS::DataReader_ptr /*reader*/,
  const DDS::SampleLostStatus& /*status*/)
{
}