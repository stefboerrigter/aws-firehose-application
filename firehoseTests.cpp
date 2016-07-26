/*
  * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
  * 
  * Licensed under the Apache License, Version 2.0 (the "License").
  * You may not use this file except in compliance with the License.
  * A copy of the License is located at
  * 
  *  http://aws.amazon.com/apache2.0
  * 
  * or in the "license" file accompanying this file. This file is distributed
  * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
  * express or implied. See the License for the specific language governing
  * permissions and limitations under the License.
  */

//#include <aws/external/gtest.h>
#include <aws/core/Aws.h>
#include "FirehoseLibraryClient.h"

#include <fstream>

int main(int argc, char** argv)
{
    std::cout << "Starting Firehose Test " << std::endl;
    Aws::SDKOptions options;
    Aws::String handle;
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    Aws::InitAPI(options);
    std::string streamName("test-default-queue-name");
    std::string bucketName("testbucketstef");
    int amount_messages = 1;
      
    std::string paramAmount("-n");
    std::string paramStream("-s");
    std::string paramHelp("-h");
    
    for(int i = 1; i < argc; i++)
    {
       if(paramAmount.compare(argv[i]) == 0)
       {
         i++;
         amount_messages = atoi(argv[i]);
       }
       
       if(paramStream.compare(argv[i]) == 0)
       {
         i++;
         streamName = (argv[i]);
       }
       
       else if(paramHelp.compare(argv[i]) == 0)
       {
         std::cout << "Help" <<std::endl << "-n <amount> for amount of messages" << std::endl << std::endl;
         return 0;
       }
    }
    
    std::ifstream fc("demofile.txt", std::ios_base::in | std::ios_base::binary);
  
    //start of class
    FirehoseLibraryClient app(streamName, bucketName);
    if(app.initQueue()) //success
    {
      app.sendMessage(fc, amount_messages);
    }
    
    Aws::ShutdownAPI(options);
    return 0;
}



