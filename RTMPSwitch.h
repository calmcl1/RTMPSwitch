/* 
 * File:   RTMPSwitch.h
 * Author: callum
 *
 * Created on 04 June 2014, 16:51
 */

#ifndef RTMPSWITCH_H
#define	RTMPSWITCH_H

gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer data);

void on_pad_added(GstElement *decoder, GstPad *pad, gpointer data);

int main(int argc, char* argv[]);

#endif	/* RTMPSWITCH_H */

