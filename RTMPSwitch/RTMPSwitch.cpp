// RTMPSwitch.cpp : Defines the entry point for the console application.
//

#include "RTMPSwitch.h"
#include <iostream>
#include <stdio.h>


extern "C" {
#include "include/librtmp/rtmp.h"
#include <gstreamer-1.0/gst/gst.h>
}

using namespace std;

int main(int argc, char* argv[]) {

    // Set program version
    const char version[] = "0.1.0";
    
    // Initialize GST
    const gchar *nano_str;
    guint gst_major, gst_minor, gst_micro, gst_nano;

    gst_init(&argc, &argv);
    gst_version(&gst_major, &gst_minor, &gst_micro, &gst_nano);

    // Rock and roll!
    printf("Starting RTMPSwitch v%s (linked against Gstreamer %d.%d.%d)\n",
            version, gst_major, gst_minor, gst_micro);

    return 0;

}

