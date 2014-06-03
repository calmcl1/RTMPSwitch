// RTMPSwitch.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "lib\librtmp\rtmp.h"
#include "lib\getopt\getopt.h"
#include <iostream>

using namespace std;

/*bool ProcessCommandLine(int argc, char *argv[]){
	const char optstr[] = "hu:";
	int opts = getopt(argc, argv, optstr);
	return opts;
}*/

int main(int argc, char* argv[])
{
	/*cout << "About to process options..." << endl;
	ProcessCommandLine(argc, argv);
	cout << "Processed options." << endl;
	*/

	cout << "Starting RTMPSwitch" << endl;

	RTMP * rtmp_handle = RTMP_Alloc();
	RTMP_Init(rtmp_handle);
	char url[] = "http://www.myurl.com";
	RTMP_SetupURL(rtmp_handle, url);
	RTMPPacket* rp;
	RTMP_Connect(rtmp_handle, rp);

}

