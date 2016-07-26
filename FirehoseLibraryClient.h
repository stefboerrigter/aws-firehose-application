#pragma once

#include <fstream>

namespace Aws {
	namespace Firehose{
		class FirehoseClient;
	}
}

#include <aws/core/client/ClientConfiguration.h>

class FirehoseLibraryClient
{
  private:
	Aws::Client::ClientConfiguration m_config;
	Aws::Firehose::FirehoseClient *m_firehoseClient;
    std::string m_streamName;
    std::string m_bucketName;

  public:
    FirehoseLibraryClient(std::string name, std::string bucketName);
    ~FirehoseLibraryClient();
    
    bool initQueue();
    
    bool sendMessage(const std::ifstream &data, int repetitions = 0);
};
