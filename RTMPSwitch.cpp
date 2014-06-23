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
#include "srcBin.h"

using namespace std;

static GMainLoop * loop;
GstElement *pipeline, *audiobin, *videobin;
GstElement *decoder;
GstElement *aconv, *ares, *acaps, *vorbisenc;
GstElement *vconv, *vscale, *vrate, *vcaps, *theoraenc;
GstElement *oggmux, *sink;
GstCaps *audio_caps, *video_caps;

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

    // When uridecode finds a stream, it adds a pad and calls this

    GstCaps *caps;
    GstStructure *str;
    GstPad *audiopad;

    // Make sure the audiobin isn't already linked
    audiopad = gst_element_get_static_pad(audiobin, "sink");
    if (GST_PAD_IS_LINKED(audiopad)) {
        g_object_unref(audiopad);
        return;
    }

    // Find out what kind of pad is available
    caps = gst_pad_query_caps(pad, NULL);
    str = gst_caps_get_structure(caps, 0);
    if (!g_strrstr(gst_structure_get_name(str), "audio")) {
        gst_caps_unref(caps);
        gst_object_unref(audiopad);
        return;
    }

    gst_caps_unref(caps);
    // Link the new pad to the audiobin
    gst_pad_link(pad, audiopad);
    g_object_unref(audiopad);

}

int main(int argc, char* argv[]) {

    // Set program version
    const char version[] = "0.1.0";

    // Initialize GST
    const gchar *nano_str;
    guint gst_major, gst_minor, gst_micro, gst_nano;

    gst_init(&argc, &argv);
    gst_version(&gst_major, &gst_minor, &gst_micro, &gst_nano);
    loop = g_main_loop_new(NULL, FALSE);

    cerr << "Starting RTMPSwitch v" << version;
    cerr << " (linked against Gstreamer ";
    cerr << gst_major << "." << gst_minor << "." << gst_micro << ")" << endl;

    // Create the pipeline, attach a bus
    pipeline = gst_pipeline_new("videoswitch-pipeline");
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    guint bus_watch_id = gst_bus_add_watch(bus, bus_callback, NULL);
    gst_object_unref(bus);

    // Create the first half of the pipeline
    char src[] = "file:///home/dev/Desktop/dewtwg-short.mp4";
    decoder = create_new_srcbin(src);

    // Now create the audio-playing part of the pipeline
    audiobin = gst_bin_new("audiobin");

    aconv = gst_element_factory_make("audioconvert", "aconv");
    ares = gst_element_factory_make("audioresample", "ares");

    vorbisenc = gst_element_factory_make("avenc_aac", "aenc");
    g_object_set(G_OBJECT(vorbisenc), "bitrate", 96000, NULL);

    audio_caps = gst_caps_new_simple("audio/x-raw",
            "rate", G_TYPE_INT, 44100,
            NULL);
    GstElement * acaps = gst_element_factory_make("capsfilter", "acaps");
    g_object_set(G_OBJECT(acaps), "caps", audio_caps, NULL);

    // The next line is so that we can grab the sink of audioconvert
    // and attach it to a ghost pad to use for the sink of audiobin
    GstPad * audiosink = gst_element_get_static_pad(ares, "sink");
    GstPad * audiosrc = gst_element_get_static_pad(vorbisenc, "src");

    gst_bin_add_many(GST_BIN(audiobin), aconv, ares, acaps, vorbisenc, NULL);

    cerr << "linked ares to aconv? " << gst_element_link(ares, aconv) << endl;
    cerr << "linked aconv to acaps? " << gst_element_link(aconv, acaps) << endl;
    cerr << "linked acaps to vorbisenc? " << gst_element_link(acaps, vorbisenc) << endl;


    // Create the audiobin sink so we can attach it
    cerr << "Added audiobin sink? " << gst_element_add_pad(audiobin, gst_ghost_pad_new("sink", audiosink)) << endl;
    cerr << "Added audiobin src? " << gst_element_add_pad(audiobin, gst_ghost_pad_new("src", audiosrc)) << endl;

    // Now create the video-playing part.
    videobin = gst_bin_new("videobin");

    vconv = gst_element_factory_make("videoconvert", "vconv");
    cerr << "vconv: " << vconv << endl;
    vscale = gst_element_factory_make("videoscale", "vscale");
    cerr << "vscale: " << vscale << endl;
    vrate = gst_element_factory_make("videorate", "vrate");
    cerr << "vrate: " << vrate << endl;
    vcaps = gst_element_factory_make("capsfilter", "vcaps");
    cerr << "vcaps: " << vcaps << endl;

    video_caps = gst_caps_new_simple("video/x-raw",
            //"framerate", GST_TYPE_FRACTION, "(fraction)30/1",
            "width", G_TYPE_INT, 1280,
            "height", G_TYPE_INT, 720,
            NULL);

    cerr << "video_caps: " << video_caps << endl;
    g_object_set(G_OBJECT(vcaps), "caps", video_caps, NULL);

    theoraenc = gst_element_factory_make("x264enc", "venc");
    cerr << "theoraenc: " << theoraenc << endl;
    g_object_set(G_OBJECT(theoraenc), "bitrate", 1000, NULL);
    g_object_set(G_OBJECT(theoraenc), "speed-preset", 4, NULL);

    //Again, grab the sink pad of vconv so we can use it as the sink for
    //videobin
    GstPad * videosink = gst_element_get_static_pad(vconv, "sink");
    GstPad * videosrc = gst_element_get_static_pad(theoraenc, "src");

    gst_bin_add_many(GST_BIN(videobin), vconv, vscale, vrate, vcaps, theoraenc, NULL);
    cerr << "linked vconv to vscale? " << gst_element_link(vconv, vscale) << endl;
    cerr << "linked vscale to vrate? " << gst_element_link(vscale, vrate) << endl;
    cerr << "linked vrate to vcaps? " << gst_element_link(vrate, vcaps) << endl;
    cerr << "linked vcaps to theoraenc? " << gst_element_link(vcaps, theoraenc) << endl;

    gst_element_add_pad(videobin, gst_ghost_pad_new("sink", videosink));
    gst_element_add_pad(videobin, gst_ghost_pad_new("src", videosrc));

    //oggmux = gst_element_factory_make("oggmux", "mux");
    oggmux = gst_element_factory_make("flvmux", "mux");
    g_object_set(G_OBJECT(oggmux), "streamable", true, NULL);
    sink = gst_element_factory_make("fdsink", "sink");

    gst_bin_add_many(GST_BIN(pipeline), decoder, audiobin, videobin, oggmux,
            sink, NULL);
    cerr << "linked decoder audio to audiobin? " << gst_element_link_pads(decoder, "src_a", audiobin, "sink") << endl;
    cerr << "linked decoder video to videobin? " << gst_element_link_pads(decoder, "src_v", videobin, "sink") << endl;

    cerr << "linked audiobin to oggmux? " << gst_element_link(audiobin, oggmux) << endl;
    cerr << "linked videobin to oggmux? " << gst_element_link(videobin, oggmux) << endl;
    cerr << "linked oggmux to sink? " << gst_element_link(oggmux, sink) << endl;

    // Check out 'inputselector' and 'fdsink' elements

    // To keep track of all our RTP receivers
    //vector<GstElement> sources;
    //GstElement src1 = create_new_srcbin("12345");
    //sources.assign(1,src1);

    // On we go!
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    g_main_loop_run(loop);
    return 0;

}