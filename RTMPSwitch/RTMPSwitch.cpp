// RTMPSwitch.cpp : Defines the entry point for the console application.
//

extern "C"{
#include "lib\librtmp\rtmp.h"
}

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{

	cout << "Starting RTMPSwitch" << endl;
	cout << RTMP_LibVersion << endl;

	RTMP * rtmp_handle = RTMP_Alloc();
	RTMP_Init(rtmp_handle);
	char url[] = "http://www.myurl.com";
	RTMP_SetupURL(rtmp_handle, url);
	RTMPPacket* rp;
	RTMP_Connect(rtmp_handle, rp);

}
