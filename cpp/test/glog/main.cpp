#include <iostream>
#include <glog/logging.h>  

using namespace std;

// g++ main.cpp -o test -g -Wall -I/usr/local/include/ -Wl,-Bstatic -lglog -Wl,-Bdynamic -lpthread 

int main(int argc, char** argv) 
{

	FLAGS_log_dir=".";   
	google::InitGoogleLogging(argv[0]); 

	google::SetLogDestination(google::GLOG_INFO, "INFO.");
	google::SetLogDestination(google::GLOG_WARNING, "WARNING.");
	google::SetLogDestination(google::GLOG_ERROR, "ERROR.");
	google::SetLogDestination(google::GLOG_FATAL, "FATAL.");

	LOG(INFO) << "hello glog";     
	int num_cookies = 2;
	LOG_IF(INFO, num_cookies > 10) << "Got lots of cookies";
	LOG(ERROR) << "what't error.";     
	LOG(FATAL) << "what't error.";     


	return 0;
}
