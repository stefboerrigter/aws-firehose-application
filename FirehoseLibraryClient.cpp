#include "FirehoseLibraryClient.h"
#include <aws/core/client/CoreErrors.h>
#include <aws/core/auth/AWSCredentialsProviderChain.h>
#include <aws/core/utils/json/JsonSerializer.h>
#include <aws/core/utils/xml/XmlSerializer.h>

#include <aws/core/utils/Outcome.h>
#include <aws/access-management/AccessManagementClient.h>
#include <aws/iam/IAMClient.h>
#include <aws/cognito-identity/CognitoIdentityClient.h>

//Verified includes:
#include <aws/firehose/model/CreateDeliveryStreamRequest.h>
#include <aws/firehose/model/S3DestinationConfiguration.h>

#include <aws/firehose/model/PutRecordRequest.h>

#define DEBUG_INFO 

using namespace std;
using namespace Aws;
using namespace Aws::Http;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::Client;
using namespace Aws::Firehose;
using namespace Aws::Firehose::Model;
using namespace Aws::Utils::Json;

static const int DATA_SIZE = (1024 * 200); //~200 KB 

//Constructor
FirehoseLibraryClient::FirehoseLibraryClient(Aws::String streamName) :
  m_firehoseClient(nullptr),
  m_streamName(streamName)
{
  m_config.scheme = Scheme::HTTPS;
  //m_config.region = Region::EU_WEST_1;
  m_config.region = Region::US_EAST_1;
  
  m_firehoseClient = new FirehoseClient(m_config);
}

//destructor
FirehoseLibraryClient::~FirehoseLibraryClient()
{
  
}

bool FirehoseLibraryClient::initQueue()
{
  auto cognitoClient = Aws::MakeShared<Aws::CognitoIdentity::CognitoIdentityClient>("QueueOperationTest", m_config);

  auto iamClient = Aws::MakeShared<Aws::IAM::IAMClient>("QueueOperationTest", m_config);
  Aws::AccessManagement::AccessManagementClient accessManagementClient(iamClient, cognitoClient);
  
  Aws::String m_accountId = accessManagementClient.GetAccountId();
  
  Aws::String user;
  Aws::IAM::Model::User data;
  
  accessManagementClient.GetUser(user, data);
#ifdef DEBUG_INFO 
  cout << "Account ID : " << m_accountId << user <<endl;
#endif
  
  CreateDeliveryStreamRequest request;
  request.SetDeliveryStreamName(m_streamName);
  
  S3DestinationConfiguration s3Config;

  //TBD; role is fixed now... 
  Aws::String roleARN = "arn:aws:iam::" + m_accountId + ":role/firehose_delivery_role";
  
  s3Config.SetRoleARN(roleARN);
  s3Config.SetBucketARN("arn:aws:s3:::testbucketstef");
  s3Config.SetPrefix("stef_");
  s3Config.SetBufferingHints(BufferingHints());
  s3Config.SetCompressionFormat(CompressionFormat::UNCOMPRESSED);
  s3Config.SetEncryptionConfiguration(EncryptionConfiguration());
  request.SetS3DestinationConfiguration(s3Config);
  
  CreateDeliveryStreamOutcome outcome = m_firehoseClient->CreateDeliveryStream(request);
  if (outcome.IsSuccess())
  {
    cout << "Succesfull created new deliverystream [" << m_streamName << "]" << endl ;
    return true;
  }else if (outcome.GetError().GetErrorType() == FirehoseErrors::RESOURCE_IN_USE){
#ifdef DEBUG_INFO
    cout << "Resource in use, stream [" << m_streamName << "] exists already"<< endl;
#endif
    return true;
  }else if (outcome.GetError().GetErrorType() == FirehoseErrors::LIMIT_EXCEEDED){
    cout << "Limit Exceeded "<< endl;
  }else if (outcome.GetError().GetErrorType() == FirehoseErrors::UNKNOWN){
    cout << "Unknown error "<< endl;
  }else if (outcome.GetError().GetErrorType() == FirehoseErrors::ACCESS_DENIED){
    cout << "Access Denied "<< endl;
  }else{
    cout << "Other error.... "<< endl;
  }
  return false;
}

bool FirehoseLibraryClient::sendMessage(const Aws::StringStream& data, int repetitions/* = 0*/)
{
  
  PutRecordRequest request;
  //set stream name;
  request.SetDeliveryStreamName(m_streamName);
  
  Record record;
#ifdef DEBUG_INFO  
  cout << "Buff Size to transfer: [" << data.str().length() << "]" << endl;
#endif
  Aws::Utils::ByteBuffer buff((unsigned char*)data.str().c_str(), data.str().length());
  
  //apply stream data to record buffer Data
  record.SetData(buff);
  
  //set record to request
  request.SetRecord(record);
  //for loop is for testing purposes only
  for(int i = 0; i <= repetitions; i++)
  {
    //send request to cloud
    Model::PutRecordOutcome outcome = m_firehoseClient->PutRecord(request);
    if(!outcome.IsSuccess())
    {
      cout << "Error sending message " << i + 1 << "." << endl;
      i = repetitions;
      return false;
    }
  }
  return true;
}
