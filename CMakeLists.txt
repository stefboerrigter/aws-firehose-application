cmake_minimum_required(VERSION 2.6)
project(FirehoseClientLib)

IF (NOT CMAKE_BUILD_TYPE)

  SET(CMAKE_BUILD_TYPE "Debug")
ELSE()

  SET(CMAKE_BUILD_TYPE "Release")

ENDIF (NOT CMAKE_BUILD_TYPE)

#OPTION(BUILD_SHARED_LIBS "Use shared libraries" OFF)

#no optimalisation for eclipse debugging
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


set(INCLUDE_DIRECTORIES
  "${AWS_NATIVE_SDK_ROOT}/aws-cpp-sdk-core/include/"
  "${AWS_NATIVE_SDK_ROOT}/aws-cpp-sdk-firehose/include/"
  "${AWS_NATIVE_SDK_ROOT}/aws-cpp-sdk-cognito-identity/include/"
  "${AWS_NATIVE_SDK_ROOT}/aws-cpp-sdk-access-management/include/"  
  "${AWS_NATIVE_SDK_ROOT}/aws-cpp-sdk-iam/include/"
  "${AWS_NATIVE_SDK_ROOT}/testing-resources/include/"
  "*.h"
)

include_directories(${INCLUDE_DIRECTORIES})

find_library(AWS_SDK_CORE aws-cpp-sdk-core)
#find_library(AWS_SDK_SQS aws-cpp-sdk-sqs)
find_library(AWS_SDK_AM aws-cpp-sdk-access-management)
find_library(AWS_SDK_IAM aws-cpp-sdk-iam)
find_library(AWS_SDK_COG aws-cpp-sdk-cognito-identity)
find_library(AWS_SDK_FH aws-cpp-sdk-firehose)

if((NOT AWS_SDK_CORE) OR (NOT AWS_SDK_AM) OR (NOT AWS_SDK_COG) OR (NOT AWS_SDK_FH) OR (NOT AWS_SDK_IAM))
	message(SEND_ERROR " Failed to find AWS libraries")
	message("-- ${AWS_SDK_FH}")
	message("-- ${AWS_SDK_COG}")
	message("-- ${AWS_SDK_AM}")
	message("-- ${AWS_SDK_CORE}")
	message("-- ${AWS_SDK_IAM}")
	
endif()

SET( FIREHOSE_SOURCES
  FirehoseLibraryClient.cpp
)


add_library (aws-firehose
			${FIREHOSE_SOURCES}
)

# link aws stuff to this lib
target_link_libraries(aws-firehose
	${AWS_SDK_CORE}
	${AWS_SDK_COG}
	${AWS_SDK_FH}
	${AWS_SDK_AM}
	${AWS_SDK_IAM}
)

SET(FIREHOSE_TEST_APP
  firehoseTests.cpp
)
 
add_executable(firehose-application ${FIREHOSE_TEST_APP})


target_link_libraries(firehose-application
					  aws-firehose

)

#testing-resources
#copyDlls(firehose-application aws-cpp-sdk-core aws-cpp-sdk-firehose aws-cpp-sdk-access-management aws-cpp-sdk-iam aws-cpp-sdk-cognito-identity)
