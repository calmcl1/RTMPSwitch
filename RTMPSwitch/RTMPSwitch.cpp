// RTMPSwitch.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <stdio.h>

#include <vector>

extern "C" {
#include "include/librtmp/rtmp.h"
#include <gstreamer-1.0/gst/gst.h>
}
#include <glog/logging.h>
#include "RTMPSwitch.h"

using namespace std;

static GMainLoop * loop;
GstElement *pipeline, *audiobin;
GstElement *decoder, *conv, *vorbisenc, *sink;

gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer data) {
    //cout << "Got message: " << GST_MESSAGE_TYPE_NAME(message) << endl;

    switch (GST_MESSAGE_TYPE(message)) {
        case GST_MESSAGE_EOS:
            g_main_loop_quit(loop);
            break;
        case GST_MESSAGE_ERROR:
            GError * err;
            gchar * debug;

            gst_message_parse_error(message, &err, &debug);
            cout << "Error: " << err->message << endl;
            g_error_free(err);
            g_free(debug);
            
            g_main_loop_quit(loop);
    }
}

void on_pad_added(GstElement *decoder, GstPad *pad, gpointer data) {

    GstCaps *caps;
    GstStructure *str;
    GstPad *audiopad;

    audiopad = gst_element_get_static_pad(audiobin, "sink");
    if (GST_PAD_IS_LINKED(audiopad)) {
        g_object_unref(audiopad);
        return;
    }

    caps = gst_pad_query_caps(pad, NULL);
    str = gst_caps_get_structure(caps, 0);
    if (!g_strrstr(gst_structure_get_name(str), "audio")) {
        gst_caps_unref(caps);
        gst_object_unref(audiopad);
        return;
    }

    gst_caps_unref(caps);
    gst_pad_link(pad, audiopad);
    g_object_unref(audiopad);

}

int main(int argc, char* argv[]) {

    google::InitGoogleLogging(argv[0]);
    
    // Set program version
    const char version[] = "0.1.0";

    // Initialize GST
    const gchar *nano_str;
    guint gst_major, gst_minor, gst_micro, gst_nano;

    gst_init(&argc, &argv);
    gst_version(&gst_major, &gst_minor, &gst_micro, &gst_nano);
    loop = g_main_loop_new(NULL, FALSE);

    // Rock and roll!
    printf("Starting RTMPSwitch v%s (linked against Gstreamer %d.%d.%d)\n",
            version, gst_major, gst_minor, gst_micro);


    pipeline = gst_pipeline_new("my-pipeline");
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    guint bus_watch_id = gst_bus_add_watch(bus, bus_callback, NULL);
    gst_object_unref(bus);

    decoder = gst_element_factory_make("uridecodebin", "uridec");
    g_object_set(G_OBJECT(decoder), "uri", "http://192.168.0.102:8080/test.ogg",
            NULL);
    g_signal_connect(decoder, "pad-added", G_CALLBACK(on_pad_added), NULL);


    //sink = gst_element_factory_make("fdsink", "sink");

    audiobin = gst_bin_new("audiobin");
    conv = gst_element_factory_make("audioconvert", "aconv");
    GstPad * audiopad = gst_element_get_static_pad(conv, "sink");
    vorbisenc = gst_element_factory_make("vorbisenc", "enc");
    sink = gst_element_factory_make("filesink", "sink");
    g_object_set(G_OBJECT(sink), "location", "test.ogg", NULL);
    //g_object_set(G_OBJECT(sink), "dump", TRUE, NULL);
    gst_bin_add_many(GST_BIN(audiobin), conv, vorbisenc, sink, NULL);
    //gst_bin_add_many(GST_BIN(audiobin), conv, sink, NULL);
    gst_element_link_many(conv, vorbisenc, sink, NULL);
    //gst_element_link_many(conv,sink,NULL);

    gst_element_add_pad(audiobin, gst_ghost_pad_new("sink", audiopad));

    gst_bin_add_many(GST_BIN(pipeline), decoder, audiobin, NULL);

    // Check out 'inputselector' and 'fdsink' elements

    // To keep track of all our RTP receivers
    //vector<GstElement> rtp_receivers;
    //GstElement rtp1 = create_rtp_receiver("12345");
    //rtp_receivers.assign(1,*rtp1);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    g_main_loop_run(loop);
    return 0;

}

GstElement create_rtp_receiver(char ip[]) {
    GstElement * element = gst_element_factory_make("fakesrc", "src");
    if (!element) {
        printf("Could not make element\n");
    } else {
        printf("got element\n");
        return *element;
    }
}