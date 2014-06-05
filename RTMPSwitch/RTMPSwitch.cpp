// RTMPSwitch.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <stdio.h>

#include <vector>

extern "C" {
#include "include/librtmp/rtmp.h"
#include <gstreamer-1.0/gst/gst.h>
}
#include "RTMPSwitch.h"

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
    
    // Some test stuff to get GStreamer going
    GstElement * pipeline;
    GstElement *source, *filter, *sink;
    
    pipeline = gst_pipeline_new("my-pipeline");
    
    source = gst_element_factory_make("fakesrc", "src");
    filter = gst_element_factory_make("identity", "filter");
    sink = gst_element_factory_make("fakesink", "sink");
    
    gst_bin_add_many(GST_BIN(pipeline),source,filter,sink,NULL);
    
    // To keep track of all our RTP receivers
    //vector<GstElement> rtp_receivers;
    //GstElement rtp1 = create_rtp_receiver("12345");
    //rtp_receivers.assign(1,*rtp1);

    return 0;

}

GstElement create_rtp_receiver(char ip[]){
    GstElement * element = gst_element_factory_make("fakesrc", "src");
    if (!element){
        printf("Could not make element\n");
    } else {
        printf("got element\n");
        return *element;
    }
}