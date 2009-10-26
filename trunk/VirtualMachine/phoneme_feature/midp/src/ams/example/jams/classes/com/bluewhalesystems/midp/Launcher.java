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

import com.sun.cldc.isolate.*;
import com.sun.midp.installer.*;
import com.sun.midp.configurator.Constants;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;
import java.io.*;
import javax.microedition.io.*;
import com.sun.midp.io.j2me.storage.File;
import com.sun.midp.io.j2me.storage.RandomAccessStream;
import com.sun.midp.installer.*;
import com.sun.midp.events.*;
import com.sun.midp.main.*;
import com.sun.midp.midlet.*;
import com.sun.midp.midletsuite.*;
import com.sun.midp.configurator.Constants;
import com.sun.midp.security.*;

public class Launcher extends MIDlet implements Runnable, CommandListener,MIDletProxyListListener,PlatformRequestListener.PlatformRequestListenerHandler {
    private static boolean iDebug = false;
    private static final int DOWNLOAD_IN_PROGRESS = 0;
    private static final int DOWNLOAD_FINISHED = 1;
    private static final int DOWNLOAD_FAILED = 2;
    private static final int DOWNLOAD_UNNECESSARY = 3;
    private static final int DOWNLOAD_CANCELLED = 4;
    private static final int DOWNLOAD_WAITING_FOR_USER = 5;
    private static final String SONY_ERICSSON_AUTOSTART	= "autostart://:";
	private static final String INSTALLER = "com.sun.midp.installer.GraphicalInstaller";
	private String iMIDletName;
	private String iMIDletFullName;
    private Form iForm;
    private StringItem iProgressString;
    private HttpConnection iHttpConnection;
    private JadProperties iCurrentProperties;
    private JadProperties iNewProperties;
    private Command iCommandCancel;
    private Command iCommandExit;
    private Command iCommandYes;
    private Command iCommandNo;
    private Command iCommandStopAutoStart;
    private Command iCommandStartAutoStart;
    private Command iCommandRetry;
    private Command iCommandMenuCancel;
    private int iStatus;
    private Object iLock;
    private LauncherCustomItem iLauncherCustomItem;
    private int iProgId;
    private static final String DEBUG_HOST_URL = "http://preprod.bluewhalesystems.com";
    private static final String DEBUG_URL = DEBUG_HOST_URL + "/download/wap.dwn?id=midlet_signed";
    private static final String DEFAULT_URL = "http://www.bluewhalemail.com/download/wap.dwn?id=midlet_signed";
	private static PlatformRequestListener iPlatformRequestListener;
	private EventQueue iEventQueue;
	private BWMDisplayController iDisplayController;
    private String iUrl = null;
    
    private static void debugMessage(String aMessage) {
        if (iDebug) {
            System.out.println(aMessage);
        }
    }

    public Launcher() {
        MIDletProxyList midletProxyList = MIDletProxyList.getMIDletProxyList();
		iMIDletName = System.getProperty("x-bw-app-name");
		iMIDletFullName = System.getProperty("x-bw-app-full-name");
        midletProxyList.getMIDletProxyList().addListener(this);
        iProgressString = new StringItem(null, "");
        iForm = new Form(iMIDletName);
        iLauncherCustomItem = new LauncherCustomItem(iForm);
        iProgId = iForm.append(iProgressString);
        iForm.append(iLauncherCustomItem);

        iCommandCancel = new Command("Cancel", Command.CANCEL, 0);
        iCommandMenuCancel = new Command("Cancel", Command.ITEM, 0);	// cancel options that appears in the menu rather than cba
        iForm.addCommand(iCommandCancel);
        iCommandExit = new Command("Exit", Command.BACK, 0);
        iCommandYes = new Command("Yes", Command.BACK, 0);
        iCommandNo = new Command("No", Command.ITEM, 0);
        iCommandStopAutoStart = new Command("Stop Autostart", Command.ITEM, 1);
        iCommandStartAutoStart = new Command("Autostart", Command.ITEM, 1);
        iCommandRetry = new Command("Retry", Command.BACK, 0);
        iForm.setCommandListener(this);
        iLock = new Object();

        MIDletProxy thisMidlet;
        for (; ; ) {
            thisMidlet = midletProxyList.findMIDletProxy(
                MIDletSuite.INTERNAL_SUITE_ID, this.getClass().getName());

            if (thisMidlet != null) {
                break;
            }

            try {
                Thread.sleep(10);
            } catch (InterruptedException ie) {
                // ignore
            }
        }
        
        iDisplayController = new BWMDisplayController(midletProxyList); 
        midletProxyList.setDisplayController(iDisplayController);

        if (!isInstalled()) {
            iProgressString.setText(iMIDletName + " needs to check for features specific to your phone. This will use airtime. Proceed?\n");
            iForm.removeCommand(iCommandCancel);
            iForm.addCommand(iCommandYes);
            iForm.addCommand(iCommandNo);
            if (isRegisteredForAutostart()) {
                iForm.addCommand(iCommandStopAutoStart);
                iForm.removeCommand(iCommandStartAutoStart);
            } else {
                iForm.addCommand(iCommandStartAutoStart);
                iForm.removeCommand(iCommandStopAutoStart);
            }
            iStatus = DOWNLOAD_WAITING_FOR_USER;
        } else {
            iStatus = DOWNLOAD_UNNECESSARY;
        }
        initializeListener();
    }

    private void initializeListener()
	{	
		iEventQueue = EventQueue.getEventQueue();
		iPlatformRequestListener = new PlatformRequestListener(iEventQueue,this);
	}

    private boolean waitForUser() {
        synchronized (iLock) {
            if (iStatus == DOWNLOAD_WAITING_FOR_USER) {
                try {
                    iLock.wait();
                } catch (Exception e) {
                    debugMessage("Exception while locking thread " + e.toString());
                    notifyDestroyed();
                    return false;
                }
            }
        }
        return true;
    }

    private void clearAPN() {
        try {
            platformRequest("x-bw-clearapn://:");
        } catch (Exception e) {
            debugMessage("Exception resetting apn " + e.toString());
        }
    }

    private void setAutoAPN() {
        try {
            platformRequest("x-bw-autoapn://:");
        } catch (Exception e) {
            debugMessage("Exception setting auto apn " + e.toString());
        }
    }

    public String switchToDebugURL(String aUrl) {
        // Attempt to preserve the URL but switch host to preprod

        String postHostName = null;
        String httpStart = "http://";
        int httpIndex = aUrl.indexOf(httpStart);
        if (httpIndex != -1) {
            int bareHostNameEnd = aUrl.indexOf('/', httpIndex + httpStart.length());
            if (bareHostNameEnd == -1) {
                bareHostNameEnd = aUrl.indexOf('?');
            }
            if (bareHostNameEnd != -1) {
                postHostName = aUrl.substring(bareHostNameEnd);
            }
        }

        if (postHostName == null) {
            // parsing failed - fallback URL
            return DEBUG_URL;
        } else {
            return DEBUG_HOST_URL + postHostName;
        }
    }

    public void run() {
        // thread started. Wait for the user to press yes
        if (!waitForUser()) {
            return;
        }

        if (!isInstalled()) {
            clearAPN();
        }

        iCurrentProperties = getCurrentMIDletProperties();

        boolean finished = false;
        while (!finished) {
            if (!waitForUser()) {
                return;
            }
            try {
                if (iStatus == DOWNLOAD_CANCELLED) {
                    // Network connection or download was cancelled during previous iteration
                    iForm.removeCommand(iCommandMenuCancel);
                    iForm.removeCommand(iCommandRetry);
                    iForm.removeCommand(iCommandStartAutoStart);
                    iForm.removeCommand(iCommandStopAutoStart);
                    debugMessage("Network connection or download was cancelled");
                } else if (isInstalled()) {
                    // updateSuite();
                    iStatus = DOWNLOAD_UNNECESSARY;
                } else if (iLauncherCustomItem.getPreprod()) {
                    setAutoAPN();
                    String jadUrl = switchToDebugURL(DEFAULT_URL);
                    downloadMidlet(jadUrl);
                } else {
					String smsBody = SMSTextReader.getInstallSMSBody("BlueWhale");
					debugMessage("smsBody " + smsBody);
					String jadUrl = parseJadDownloadURL(smsBody);
					debugMessage("jadUrl " + jadUrl);
					if (jadUrl == null) {
					jadUrl = DEFAULT_URL;
					}

                    downloadMidlet(jadUrl);
                }
                finished = (iStatus != DOWNLOAD_FAILED);
            } catch (ConnectionNotFoundException e) {
                iForm.deleteAll();
                iProgressString.setText("Download problem: " + iMIDletName + " can't connect to the internet. Please make sure you are in good coverage.\nIf you are asked to \"Select access point\" from a list, pick \"BlueWhale\" if it's there, otherwise just pick one. If it does not work, try again with a different one.\nConsult http://www.bluewhalemail.com/forum/ for help.\n");
                iProgId = iForm.append(iProgressString);
                iForm.append(iLauncherCustomItem);
                iForm.removeCommand(iCommandExit);
                iForm.removeCommand(iCommandCancel);
                iForm.addCommand(iCommandRetry);
                iForm.addCommand(iCommandMenuCancel);
                if (isRegisteredForAutostart()) {
                    iForm.addCommand(iCommandStopAutoStart);
                    iForm.removeCommand(iCommandStartAutoStart);
                } else {
                    iForm.addCommand(iCommandStartAutoStart);
                    iForm.removeCommand(iCommandStopAutoStart);
                }
                synchronized (iLock) {
                    iStatus = DOWNLOAD_WAITING_FOR_USER;
                }
                clearAPN();
            } catch (Exception e) {
                debugMessage("Exception while downloading MIDlet " + e.toString());
                synchronized (iLock) {
                    iStatus = DOWNLOAD_FAILED;
                }
            }

            switch (iStatus) {
            case DOWNLOAD_UNNECESSARY:
                debugMessage("MIDlet is installed\n");
                iForm.removeCommand(iCommandCancel);
                iForm.addCommand(iCommandExit);
                finished = true;
                break;
            case DOWNLOAD_FAILED:
                iProgressString.setText("Failed. Retry?\n");
                iForm.removeCommand(iCommandCancel);
                iForm.removeCommand(iCommandExit);
                iForm.addCommand(iCommandYes);
                iForm.addCommand(iCommandNo);
                if (isRegisteredForAutostart()) {
                    iForm.addCommand(iCommandStopAutoStart);
                    iForm.removeCommand(iCommandStartAutoStart);
                } else {
                    iForm.addCommand(iCommandStartAutoStart);
                    iForm.removeCommand(iCommandStopAutoStart);
                }
                synchronized (iLock) {
                    iStatus = DOWNLOAD_WAITING_FOR_USER;
                }
                clearAPN();
                break;
            case DOWNLOAD_CANCELLED:
            case DOWNLOAD_FINISHED:
                iProgressString.setText("");
                iForm.removeCommand(iCommandCancel);
                iForm.addCommand(iCommandExit);
                finished = true;
                break;
            default:
                break;
            }
        }
        doRunMidlet();
		
    }

    public void startApp() {
        Display.getDisplay(this).setCurrent(iForm);
        Thread t = new Thread(this);
        t.start();
    }

    private boolean isRegisteredForAutostart() {
        String result = System.getProperty( SONY_ERICSSON_AUTOSTART );
        debugMessage("SONY_ERICSSON_AUTOSTART " + result);
        int value = (result == null ? 0 : Integer.parseInt( result ));
        if ( 1 == value ) {
            return true;
        }
        return false;
    }

    public void pauseApp() {}

    public void destroyApp(boolean unconditional) {
        MIDletProxyList.getMIDletProxyList().shutdown();
    }

    public void commandAction(Command c, Displayable s) {
        synchronized (iLock) {
            // Midlet not installed, or connecting to network failed
            if (iStatus == DOWNLOAD_WAITING_FOR_USER) {
                if ((c == iCommandYes) || (c == iCommandRetry)) {
					iProgressString.setText("Connecting...");
					iForm.removeCommand(iCommandYes);
                    iForm.removeCommand(iCommandStopAutoStart);
                    iForm.removeCommand(iCommandStartAutoStart);
                    iForm.removeCommand(iCommandNo);
                    iForm.removeCommand(iCommandMenuCancel);
                    iForm.removeCommand(iCommandRetry);
                    iForm.addCommand(iCommandExit);
                    iStatus = DOWNLOAD_IN_PROGRESS;
                    iLock.notify();
                    return;
                } else if ((c == iCommandNo) || (c == iCommandCancel) || (c == iCommandMenuCancel)) {
                    debugMessage("User canceled connection");
                    iStatus = DOWNLOAD_CANCELLED;
                    iLock.notify();
                    return;
                } else if (c == iCommandStopAutoStart) {
                    try {
                        javax.microedition.io.PushRegistry.unregisterConnection(SONY_ERICSSON_AUTOSTART);
                        iForm.addCommand(iCommandStartAutoStart);
                        iForm.removeCommand(iCommandStopAutoStart);
                    } catch (Exception e) {
                        debugMessage("unregisterConnection autostart " + e);
                    }
                    return;
                } else if (c == iCommandStartAutoStart) {
                    try {
                        String cn = this.getClass().getName();
                        javax.microedition.io.PushRegistry.registerConnection(SONY_ERICSSON_AUTOSTART, cn, "*");
                        iForm.addCommand(iCommandStopAutoStart);
                        iForm.removeCommand(iCommandStartAutoStart);
                    } catch (Exception e) {
                        debugMessage("registerConnection autostart " + e);
                    }
                    return;
                }
            } else {
                if ((c == iCommandNo) || (c == iCommandCancel) || (c == iCommandMenuCancel)) {
                    debugMessage("User canceled download");
                    iStatus = DOWNLOAD_CANCELLED;
                    return;
                }
            }
        }
        notifyDestroyed();
    }

    private void GetHttpConnection(String aUrl) throws java.io.IOException {
        int status = -1;

        // Open the connection and check for redirects
        String url = aUrl;
        while (true) {
            iHttpConnection = (HttpConnection)Connector.open(url);
            status = iHttpConnection.getResponseCode();
            debugMessage("HttpConnection response code = " + status);

            if ((status == HttpConnection.HTTP_TEMP_REDIRECT) ||
                    (status == HttpConnection.HTTP_MOVED_TEMP) ||
                    (status == HttpConnection.HTTP_MOVED_PERM)) {
                url = iHttpConnection.getHeaderField("location");
                debugMessage("new url = " + url);
                iHttpConnection.close();

                synchronized (iLock) {
                    if (iStatus != DOWNLOAD_IN_PROGRESS) {
                        break;
                    }
                }
            } else {
                break;
            }
        }

        if (status != HttpConnection.HTTP_OK) {
            iHttpConnection.close();
            iHttpConnection = null;
        }
    }

    private String getJarUrl(String aJadUrl) throws java.io.IOException {
        String url = aJadUrl;
        String result = null;
        String jarUrlProperty = new String("MIDlet-Jar-URL: ");

        // access the .jad file
        GetHttpConnection(aJadUrl);
        if (iHttpConnection == null) {
            return result;
        }

        InputStream inputStream = iHttpConnection.openInputStream();
        int length = (int)iHttpConnection.getLength();
        debugMessage("HttpConnection length = " + length);
        if (length > 0) {
            byte serverData[] = new byte[length];
            int count = 0;
            int totalCount = 0;
            do {
                count = inputStream.read(serverData, totalCount, length - totalCount);
                if (count != -1) {
                    totalCount += count;
                }
                synchronized (iLock) {
                    if (iStatus != DOWNLOAD_IN_PROGRESS) {
                        inputStream.close();
                        iHttpConnection.close();
                        return result;
                    }
                }
            } while ((count != -1) && (totalCount < length));

            iNewProperties = new JadProperties();
            try {
                iNewProperties.load(new ByteArrayInputStream(serverData), "ISO-8859-1");
            } catch (Exception e) {
                debugMessage("Exception while reading jad " + e.toString());
            }

            String jarUrl = iNewProperties.getProperty("MIDlet-Jar-URL");
            debugMessage("Jar url is " + jarUrl);
            if (jarUrl != null) {
                result = jarUrl;
            }
        }
        inputStream.close();
        iHttpConnection.close();
        return result;
    }

    private JadProperties getCurrentMIDletProperties() {
        JadProperties props = new JadProperties();

        int suiteId = getMidletSuiteId();

        if (suiteId == -1) {
            //iProgressString.setText("Properties Midlet: invalid suite ID\n");
            debugMessage("Properties Midlet: invalid suite ID\n");
            return props;
        }

        try {
            MIDletSuite suite = MIDletSuiteStorage.getMIDletSuiteStorage().getMIDletSuite(suiteId, false);
            props.setProperty("MIDlet-Name", suite.getProperty("MIDlet-Name"));
            props.setProperty("MIDlet-Version", suite.getProperty("MIDlet-Version"));
            props.setProperty("MIDlet-Vendor", suite.getProperty("MIDlet-Vendor"));
            props.setProperty("MIDlet-Description", suite.getProperty("MIDlet-Description"));
            props.setProperty("MIDlet-Jar-URL", suite.getProperty("MIDlet-Jar-URL"));
            suite.close();
        } catch (MIDletSuiteLockedException msle) {
        } catch (Throwable t) {
        }

        return props;
    }

    private int getMidletSuiteId() {
        int suiteId = -1;
        try {
            MIDletSuiteStorage storage = MIDletSuiteStorage.getMIDletSuiteStorage();

            int[] suiteIds = storage.getListOfSuites();
            for (int i = 0; i < suiteIds.length; i++) {
                MIDletSuite suite = storage.getMIDletSuite(suiteIds[i], false);
                for (int j = 1; ; j++) {
                    String value = suite.getProperty("MIDlet-" + j);
                    if (value == null) {
                        break;
                    }

                    MIDletInfo midlet = new MIDletInfo(value);
                    if (midlet.classname.equals(iMIDletFullName)) {
                        suiteId = suiteIds[i];
                    }
                }
                suite.close();
            }

        } catch (MIDletSuiteLockedException msle) {
        } catch (Throwable t) {
        }

        return suiteId;
    }

    private boolean isInstalled() {
        return (getMidletSuiteId() != -1);
    }

    private boolean checkOnlineVersion(String aJadUrl) throws java.io.IOException {
        debugMessage("Checking online version: " + aJadUrl + "\n");
        boolean ret = true;
        String jarUrl = getJarUrl(aJadUrl);
        String currentVersion = iCurrentProperties.getProperty("MIDlet-Version");
        String newVersion = iNewProperties.getProperty("MIDlet-Version");

        debugMessage("Current version: " + currentVersion);
        debugMessage("New version:     " + newVersion);
        if ((newVersion != null) &&
                (currentVersion != null) &&
                (currentVersion.compareTo(newVersion) >= 0 &&
                 currentVersion.compareTo("00.00.00") != 0)) {
            synchronized (iLock) {
                if (iStatus == DOWNLOAD_IN_PROGRESS) {
                    iStatus = DOWNLOAD_UNNECESSARY;
                    ret = false;
                }
            }
        }
        return ret;
    }

    private Isolate launchInstaller(String aMode,String aJadUrl)  throws Exception 
    {
        return AmsUtil.startMidletInNewIsolate(MIDletSuite.INTERNAL_SUITE_ID,
                                                 INSTALLER, "Installer",
                                                 aMode, aJadUrl, null);
    }
    
    private void downloadMidlet(String aJadUrl) throws Exception {
        debugMessage("Jad url is " + aJadUrl);
        if (checkOnlineVersion(aJadUrl)) {
            debugMessage("Downloading " + aJadUrl + " ...\n");
            try {
                Isolate installTask = launchInstaller("I", aJadUrl);
                installTask.waitForExit();
                synchronized (iLock) {
                    iStatus = DOWNLOAD_FINISHED;
                }
                debugMessage("Downloading " + aJadUrl + " finished.");
            } catch (Throwable t) {
                debugMessage("Error install the suite: " + t.getMessage());
                iStatus = DOWNLOAD_FAILED;
                throw new ConnectionNotFoundException();
            }
        }
    }

    
    private void updateSuite() throws Exception {
        int suiteId = getMidletSuiteId();

        if (suiteId == -1) {
            iProgressString.setText("Update Midlet: invalid suite ID\n");
            debugMessage("Update Midlet: invalid suite ID\n");
            return;
        }

        try {
            MIDletSuiteImpl midletSuite = MIDletSuiteStorage.getMIDletSuiteStorage().getMIDletSuite(suiteId, false);
            InstallInfo installInfo = midletSuite.getInstallInfo();
            midletSuite.close();
            
            String aJadUrl = installInfo.getDownloadUrl();
            debugMessage("Jad url is " + aJadUrl);
            if (checkOnlineVersion(aJadUrl)) {
                iProgressString.setText("Upgrading " + aJadUrl + " ...\n");
                debugMessage("Upgrading " + aJadUrl + " ...\n");

                /*
                debugMessage("Removing suite ID " + getMidletSuiteId() + " ...\n");
                Isolate removeTask = AmsUtil.startMidletInNewIsolate(MIDletSuite.INTERNAL_SUITE_ID,
                                                "com.sun.midp.scriptutil.SuiteRemover", "Remover",
                                                "" + getMidletSuiteId(), null, null);
                removeTask.waitForExit();

                MIDletSuiteStorage storage = MIDletSuiteStorage.getMIDletSuiteStorage();
                storage.remove(getMidletSuiteId());
                */

                debugMessage("Updating " + aJadUrl + " ...\n");
                Isolate installTask = launchInstaller("FU", aJadUrl);
                installTask.waitForExit();
                synchronized (iLock) {
                    iStatus = DOWNLOAD_FINISHED;
                }

                iProgressString.setText("Upgrading " + aJadUrl + " finished.");
                debugMessage("Upgrading " + aJadUrl + " finished.");
            }
        } catch (Throwable t) {
            debugMessage("Error install the suite: " + t.getMessage());
            iStatus = DOWNLOAD_FAILED;
            throw new ConnectionNotFoundException();
        }
    }

    private void doRunMidlet() {
        int suiteId = getMidletSuiteId();

        if (suiteId == -1) {
            debugMessage("BlueWhaleMail not installed ...");
            notifyDestroyed();
            return;
        }

        debugMessage("Running BlueWhaleMail ...");

        try {
            Display.getDisplay(this).setCurrent(null);
            // notifyPaused();

            Isolate runTask = AmsUtil.startMidletInNewIsolate(suiteId,
                                     iMIDletFullName, iMIDletName, null, null, null);

            //runTask.waitForExit();
            
        } catch (Throwable t) {
            debugMessage("Error running the suite: " + t.getMessage());
        }
    }

    public void midletAdded(MIDletProxy midlet)
    {
        debugMessage("midletAdded " + midlet);
	}
    
    public void midletUpdated(MIDletProxy midlet, int aReason)
    {
        debugMessage("midletUpdated " + midlet + "\nReason " + aReason);
		if(midlet.getClassName().equals(iMIDletFullName)
			&& midlet.getMidletState() == MIDletProxy.MIDLET_ACTIVE
			&& midlet.wantsForeground())
			{
				BWMDisplayController.requestForeground0(midlet.getDisplayId(),midlet.getIsolateId());
			}
    }
    
    public void midletRemoved(MIDletProxy midlet)
    {
		debugMessage("midletRemoved " + midlet);
		if(midlet.getClassName().equals(iMIDletFullName))
        {
            if(iUrl != null)
            {
                try
                {
                    launchInstaller("FU", iUrl);
                }
                catch(Exception err)
                {
                    // failed to start installer, rerun midlet
                    doRunMidlet();
                }
                finally
                {
                    iUrl = null;                          
                }
            }
            else
            {
                notifyDestroyed();
            }
        }
        else if(midlet.getClassName().equals(INSTALLER))
        {
            doRunMidlet();        
        }
    }
    
    public void midletStartError(int externalAppId, int suiteId, String className,
                          int errorCode, String errorDetails)
                          {}

    public void installRequest(String aUrl)
    {
        debugMessage("Install midlet " + aUrl);
        iUrl = aUrl;
    }
	
private String parseJadDownloadURL(String aSmsBody)
    {
        String jadUrl = null;
        try
        {
            if( aSmsBody != null )
            {
                debugMessage( "sms body: " + aSmsBody );
                String httpStart = "http://";
                int httpIndex = aSmsBody.indexOf( httpStart );
                if( httpIndex != -1 )
                {
                    // To support both preprod, live, and other shorter URL's we might use,
                    // like m.bluewhale.net, strip out the bare hostname.
                    // e.g. from "http://www.bluewhalemail.com/download..."
                    // we get "www.bluewhalemail.com".

                    int bareHostNameStart = httpIndex + httpStart.length();
                    int bareHostNameEnd = aSmsBody.indexOf( '/', bareHostNameStart );
                    if( -1 == bareHostNameEnd )
                    {
                        bareHostNameEnd = aSmsBody.indexOf( '?', bareHostNameStart );
                    }
                    String bareHostname = aSmsBody.substring( httpIndex + httpStart.length(), bareHostNameEnd );

                    // Check to make sure it's really from us.
                    // Add skymobileemail for ticket:3664 VM needs to recognize skymobileemail.com addresses in the SMS
                    if( bareHostname.endsWith( ".bluewhalemail.com" ) || bareHostname.endsWith( ".bluewhalesystems.com" ) || 
                        bareHostname.endsWith( ".bluewhale.net" ) || bareHostname.endsWith( ".skymobileemail.com" ) )
                    {
                        int oIndex = aSmsBody.indexOf( "o=" );
                        if( oIndex != -1 )
                        {
                            int spaceIndex = aSmsBody.indexOf( ' ', oIndex );
                            int endIndex = spaceIndex;
                            if( -1 == endIndex )
                            {
                                endIndex = aSmsBody.length();
                            }
                            jadUrl = "http://" + bareHostname + "/download/wap.dwn?id=midlet_signed&" + aSmsBody.substring( oIndex, endIndex ).trim();
                        }
                    }
                }
            }
        }
        catch( Exception e )
        {
            debugMessage( "Exception thrown, using default jad url" );
        }
        debugMessage( "jad url is " + jadUrl );
        return ( jadUrl );
    }
	    
}
