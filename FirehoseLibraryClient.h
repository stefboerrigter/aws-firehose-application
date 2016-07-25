#pragma once

#include <aws/firehose/FirehoseClient.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/utils/memory/stl/AWSStringStream.h>

class FirehoseLibraryClient
{
  private:
    Aws::Client::ClientConfiguration m_config;
    Aws::Firehose::FirehoseClient *m_firehoseClient;
    Aws::String m_streamName;
    Aws::String m_bucketName;

  public:
    FirehoseLibraryClient(Aws::String name);
    ~FirehoseLibraryClient();
    
    bool initQueue(Aws::String bucketName);
    
    bool sendMessage(const Aws::StringStream& data, int repetitions = 0);
};
