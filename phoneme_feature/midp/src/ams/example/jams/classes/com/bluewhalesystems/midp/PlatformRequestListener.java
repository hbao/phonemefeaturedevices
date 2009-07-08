/**
 * Copyright (c) 2004-2008 Blue Whale Systems Ltd. All Rights Reserved. 
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER 
 *  
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License version 
 * 2 only, as published by the Free Software Foundation.  
 *  
 * This software is provided "as is," and the copyright holder makes no representations or warranties, express or
 * implied, including but not limited to warranties of merchantability or fitness for any particular purpose or that the
 * use of this software or documentation will not infringe any third party patents, copyrights, trademarks or other
 * rights.
 * 
 * The copyright holder will not be liable for any direct, indirect special or consequential damages arising out of any
 * use of this software or documentation.
 * 
 * See the GNU  General Public License version 2 for more details 
 * (a copy is included at /legal/license.txt).  
 *  
 * You should have received a copy of the GNU General Public License 
 * version 2 along with this work; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 
 * 02110-1301 USA  
 *  
 * Please contact Blue Whale Systems Ltd, Suite 123, The LightBox
 * 111 Power Road, Chiswick, London, W4 5PY, United Kingdom or visit 
 * www.bluewhalesystems.com if you need additional 
 * information or have any questions.  
 */ 
 
package com.bluewhalesystems.midp;


import com.sun.midp.events.*;

import com.sun.midp.security.Permissions;
import com.sun.midp.security.SecurityToken;

import com.sun.midp.log.Logging;
import com.sun.midp.log.LogChannels;
import com.sun.midp.io.j2me.storage.File;
import com.sun.midp.io.j2me.storage.RandomAccessStream;

public class PlatformRequestListener implements EventListener
{
    public interface PlatformRequestListenerHandler
    {
        public void installRequest(String aUrl);
    }
	private static boolean iDebug  = false;
	private EventQueue iEventQueue;
	private static final String AUTOSTARTKEY = "autostart://:";
	private PlatformRequestListenerHandler iHandler = null;
    
	private static void debugMessage(String aMessage)
	{
		if (iDebug)
		{
			System.out.println(aMessage);
		}
	}
		
    public PlatformRequestListener(EventQueue theEventQueue,PlatformRequestListenerHandler aHandler)
    {
        iEventQueue = theEventQueue;
        iEventQueue.registerEventListener(EventTypes.PUSH_REGISTER_EVENT, this);
        iEventQueue.registerEventListener(EventTypes.PLATFORM_REQUEST_INSTALL_EVENT, this);
        iHandler = aHandler;
    }

    public boolean preprocess(Event aNewEvent, Event waitingEvent)
    {
        debugMessage("preprocess " + aNewEvent);
        boolean ret = true;
        NativeEvent nativeEvent = (NativeEvent)aNewEvent;
        switch (nativeEvent.getType())
        {
            case EventTypes.PLATFORM_REQUEST_INSTALL_EVENT:
            {
               debugMessage("AMS recevied install event " + nativeEvent.stringParam1);
               if(iHandler != null)
               {
                    iHandler.installRequest(nativeEvent.stringParam1);
               }
               ret = false;
               break;
            }
        }

		return ret;
    }

    public void process(Event event)
	{
        NativeEvent nativeEvent = (NativeEvent)event;
        
        switch (nativeEvent.getType())
        {
			case EventTypes.PUSH_REGISTER_EVENT:
			{
                debugMessage("AMS recevied autostart event " + nativeEvent.intParam1);
				deleteSystemProperty0(AUTOSTARTKEY);
				setSystemProperty0(AUTOSTARTKEY,Integer.toString(nativeEvent.intParam1));
			}
            case EventTypes.PLATFORM_REQUEST_INSTALL_EVENT:
            {
               debugMessage("AMS recevied install event " + nativeEvent.stringParam1);
               if(iHandler != null)
               {
                    iHandler.installRequest(nativeEvent.stringParam1);
               }
                break;
            }
			default:
			{
				if (Logging.REPORT_LEVEL <= Logging.WARNING)
				{
					Logging.report(Logging.WARNING, LogChannels.LC_CORE, "unknown system event (" + event.getType() + ")");
				}
			}
        }
    }
	private native void deleteSystemProperty0(String aKey);
	private native void setSystemProperty0(String aKey, String aVal);
}
