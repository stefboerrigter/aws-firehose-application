#pragma once

#include <fstream>
#include <vector>

namespace Aws {
	namespace Firehose{
		class FirehoseClient;
	}
	namespace Client{
		class ClientConfiguration;
	}
	class SDKOptions;
}



class FirehoseLibraryClient
{
  private:
	Aws::Client::ClientConfiguration* m_config;
	Aws::Firehose::FirehoseClient *m_firehoseClient;
    std::string m_streamName;
    std::string m_bucketName;
    Aws::SDKOptions *m_options;
    bool m_initialized;
  public:
    FirehoseLibraryClient();
    FirehoseLibraryClient(std::string name, std::string bucketName);
    ~FirehoseLibraryClient();
    
    bool initQueue(std::string bucketPrefix);

    bool sendMessage(const std::ifstream &data, int repetitions = 0);
    bool sendMessage(const std::vector<char> &vectorData);
};
