/* 
 * File:   srcBin.cpp
 * Author: Callum McLean
 * 
 * Created on 10 June 2014, 19:03
 */

#include <stdio.h>
#include <gstreamer-1.0/gst/gst.h>
#include "srcBin.h"
#include <time.h>

GstElement *srcbin;
GstElement *uridecode, *identity_a, *identity_v;

bool __srcbin_on_pad_added(GstElement *decoder, GstPad *pad, gpointer data) {
    // When uridecode finds a stream, it adds a pad and calls this

    GstCaps *caps;
    GstStructure *str;
    GstPad *sink_pad;

    // Find out what kind of pad is available
    caps = gst_pad_query_caps(pad, NULL);
    str = gst_caps_get_structure(caps, 0);

    // Make sure the find the right identity for A/V streams
    if (g_strrstr(gst_structure_get_name(str), "audio")) {
        sink_pad = gst_element_get_static_pad(identity_a, "sink");
    } else if (g_strrstr(gst_structure_get_name(str), "video")) {
        sink_pad = gst_element_get_static_pad(identity_v, "sink");
    } else {
        gst_caps_unref(caps);
        gst_object_unref(sink_pad);
        return false;
    }

    // And make sure it's not already linked
    if (GST_PAD_IS_LINKED(sink_pad)) {
        g_object_unref(sink_pad);
        return false;
    }

    gst_caps_unref(caps);
    // Link the new pad to the identity
    gst_pad_link(pad, sink_pad);
    g_object_unref(sink_pad);
    return true;
}

GstElement* create_new_srcbin(char* uri) {
    // Create a unique identifier for the bin and elements
    // by using the current time
    int t = time(NULL);
    char bin_id[16], uridec_id[16], ident_a_id[16], ident_v_id[16];
    sprintf(bin_id, "srcbin%d", t);
    sprintf(uridec_id, "uridec%d", t);
    sprintf(ident_a_id, "ident_a%d", t);
    sprintf(ident_v_id, "ident_v%d", t);

    srcbin = gst_bin_new(bin_id);
    uridecode = gst_element_factory_make("uridecode", uridec_id);
    identity_a = gst_element_factory_make("identity", ident_a_id);
    identity_v = gst_element_factory_make("identity", ident_v_id);
    g_object_set(G_OBJECT(uridecode), "uri", uri, NULL);
    g_signal_connect(uridecode, "pad-added", G_CALLBACK(__srcbin_on_pad_added), NULL);

    // Finally, return the bin
    return srcbin;
}