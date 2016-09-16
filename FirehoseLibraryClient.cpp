#include "FirehoseLibraryClient.h"
#include <aws/core/Aws.h>
#include <aws/core/client/CoreErrors.h>
#include <aws/core/auth/AWSCredentialsProviderChain.h>

#include <aws/core/utils/Outcome.h>
#include <aws/access-management/AccessManagementClient.h>
#include <aws/iam/IAMClient.h>
#include <aws/cognito-identity/CognitoIdentityClient.h>

#include <aws/core/client/ClientConfiguration.h>

#include <aws/core/utils/memory/stl/AWSStringStream.h>
#include <aws/firehose/FirehoseClient.h>
#include <aws/firehose/model/CreateDeliveryStreamRequest.h>
#include <aws/firehose/model/S3DestinationConfiguration.h>

#include <aws/firehose/model/PutRecordRequest.h>

#define DEBUG_INFO 

using namespace std;
using namespace Aws;
using namespace Aws::Http;
using namespace Aws::Auth;
using namespace Aws::Client;
using namespace Aws::Firehose;
using namespace Aws::Firehose::Model;
using namespace Aws::Utils::Json;

//Constructor
FirehoseLibraryClient::FirehoseLibraryClient(string streamName, std::string bucketName) :
  m_firehoseClient(nullptr),
  m_config(new ClientConfiguration()),
  m_streamName(streamName),
  m_bucketName(bucketName),
  m_options(new Aws::SDKOptions()),
  m_initialized(true)
{
  //m_options = new Aws::SDKOptions;
  m_options->loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
  Aws::InitAPI(*m_options);

  m_config->scheme = Scheme::HTTPS;
  m_config->region = Region::US_EAST_1;

  m_firehoseClient = new FirehoseClient(*m_config);
}


//Constructor
FirehoseLibraryClient::FirehoseLibraryClient() :
  m_firehoseClient(nullptr),
  m_config(new ClientConfiguration()),
  m_options(new Aws::SDKOptions()),
  m_streamName(""),
  m_bucketName(""),
  m_initialized(false)
{
  //m_options = new Aws::SDKOptions;
  m_options->loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
  Aws::InitAPI(*m_options);

  m_config->scheme = Scheme::HTTPS;
  m_config->region = Region::US_EAST_1;

  m_firehoseClient = new FirehoseClient(*m_config);
}

//destructor
FirehoseLibraryClient::~FirehoseLibraryClient()
{
	Aws::ShutdownAPI(*m_options);
}

bool FirehoseLibraryClient::initQueue()
{
  if(!m_initialized)
  {
	return false;
  }

  auto cognitoClient = Aws::MakeShared<Aws::CognitoIdentity::CognitoIdentityClient>("QueueOperationTest", *m_config);
  auto iamClient = Aws::MakeShared<Aws::IAM::IAMClient>("QueueOperationTest", *m_config);
  Aws::AccessManagement::AccessManagementClient accessManagementClient(iamClient, cognitoClient);
  
  Aws::String accountId = accessManagementClient.GetAccountId();
  
  Aws::String user;
  Aws::IAM::Model::User data;

  accessManagementClient.GetUser(user, data);
#ifdef DEBUG_INFO 
  cout << "Account ID : " << accountId << user <<endl;
#endif
  
  CreateDeliveryStreamRequest request;

  request.SetDeliveryStreamName(m_streamName.c_str());

  S3DestinationConfiguration s3Config;

  //TBD; role is fixed now... 
  Aws::String roleARN = "arn:aws:iam::" + accountId + ":role/firehose_delivery_role";
  string bucketARN("arn:aws:s3:::" + m_bucketName);
  Aws::String bucketPrefix = "prefix_";

  s3Config.SetRoleARN(roleARN);
  s3Config.SetBucketARN(bucketARN.c_str());
  s3Config.SetPrefix(bucketPrefix);
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
  }else if (outcome.GetError().GetErrorType() == FirehoseErrors::ACCESS_DENIED){
    cout << "Access Denied "<< endl;
  }else{
    cout << "Other error.... [" << outcome.GetError().GetMessage() << "]" << endl;
  }
  return false;
}

bool FirehoseLibraryClient::sendMessage(const ifstream& data, int repetitions/* = 0*/)
{
  if(!m_initialized)
  {
	return false;
  }
  
  PutRecordRequest request;
  //set stream name;
  Aws::String __streamName("TMP");
  request.SetDeliveryStreamName(m_streamName.c_str());
  
  Record record;

  Aws::StringStream dataStream;
  dataStream << data.rdbuf();

#ifdef DEBUG_INFO  
  cout << "Buff Size to transfer: [" << dataStream.str().length() << "]" << endl;
#endif
  Aws::Utils::ByteBuffer buff((unsigned char*)dataStream.str().c_str(), dataStream.str().length());
  
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

void FirehoseLibraryClient::setName(string name)
{
	m_streamName = name;
	if(!m_bucketName.compare(""))
	{
		m_initialized = true;
	}

}
void FirehoseLibraryClient::setBucket(string bucket)
{
	m_bucketName = bucket;
	if(!m_streamName.compare(""))
	{
		m_initialized = true;
	}
}
