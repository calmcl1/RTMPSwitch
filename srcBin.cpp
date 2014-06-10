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

GstElement create_new_srcbin(char* uri){
    // Create a unique identifier for the bin and elements
    // by using the current time
    int t = time(NULL);
    char bin_id[16], uridec_id[16], ident_id[16];
    sprintf(bin_id,"srcbin%d",t);
    sprintf(uridec_id,"uridec%d",t);
    sprintf(ident_id,"ident%d",t);
    
    // We'll need these...
    GstElement *uridecode, *identity;
    
    uridecode = gst_element_factory_make("uridecode",uridec_id);
    identity = gst_element_factory_make("identity", ident_id);
    
    // Finally, return the bin
    GstElement * srcBin = gst_bin_new(bin_id);
}