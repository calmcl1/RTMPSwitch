// RTMPSwitch.cpp : Defines the entry point for the console application.
//

extern "C"{
#include "lib\librtmp\rtmp.h"
}
#include "lib\live\liveMedia\include\liveMedia.hh"
#include "lib\live\BasicUsageEnvironment\include\BasicUsageEnvironment.hh"


#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{

	cout << "Starting RTMPSwitch" << endl;
	cout << RTMP_LibVersion << endl;

	BasicTaskScheduler* sch = BasicTaskScheduler::createNew();
	BasicUsageEnvironment* env = BasicUsageEnvironment::createNew(*sch);

	const char url[] = "http://example.com";

	RTSPClient* rtspclient = RTSPClient::createNew(&env, url);

	char eventLoopWatchVariable = 0;
	sch->doEventLoop(&eventLoopWatchVariable);

	/*RTMP * rtmp_handle = RTMP_Alloc();
	RTMP_Init(rtmp_handle);
	char url[] = "http://www.myurl.com";
	RTMP_SetupURL(rtmp_handle, url);
	RTMPPacket* rp;
	RTMP_Connect(rtmp_handle, rp);*/

}
