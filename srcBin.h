/* 
 * File:   srcBin.h
 * Author: Callum McLean
 *
 * Created on 10 June 2014, 19:03
 */

#ifndef SRCBIN_H
#define	SRCBIN_H

bool __srcbin_on_pad_added(GstElement *decoder, GstPad *pad, gpointer data);

/**
 * This creates a bin, ready to use, with a uridecode and an identity
 * 
 * @param uri The URI to be used as the source
 * @return GstElement (bin type)
 */
GstElement* create_new_srcbin(char* uri);

#endif	/* SRCBIN_H */

