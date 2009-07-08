package javax.microedition.media;

import java.io.*;	

class MyPlayer implements Player
{
	final static boolean iDebug = true;
	String iType;
	byte[] iData = null;
	
	private static void debugMessage(String aMsg)
	{
		if(iDebug)
		{
			System.out.println(aMsg);
		}
	}
	public MyPlayer(InputStream stream, String type)
	{
		iType = type;
		try
		{
			iData = new byte[stream.available()];
			stream.read(iData);
		}
		catch (IOException e)
		{
			debugMessage("Error: MyPlayer constructor failed!");
			iData = null;
		}
	}
	public MyPlayer(String aLocator)
	{
		debugMessage("!!!!! NOT IMPLEMENTED MyPlayer " + aLocator + " !!!!!");
	}
	
	public Control[] getControls()
	{
	return null;
	}
	
	public Control getControl(String controlType)
	{
	return null;
	}
	
	public void realize() throws MediaException
	{}

    // JAVADOC COMMENT ELIDED
    public void prefetch() throws MediaException
	{
		debugMessage("!!!!! NOT IMPLEMENTED MyPlayer.prefetch() !!!!!");
	}

    // JAVADOC COMMENT ELIDED
    public void start() throws MediaException
	{
		if (iData != null)
		{
			playerStart(iType, iData);
		}
	}

    // JAVADOC COMMENT ELIDED
    public void stop() throws MediaException
	{
		playerStop();
	}

    // JAVADOC COMMENT ELIDED
    public void deallocate()
	{}

    // JAVADOC COMMENT ELIDED
    public void close()
	{}
    

    // JAVADOC COMMENT ELIDED
    public long setMediaTime(long now) throws MediaException
	{
		return 0;
	}

    // JAVADOC COMMENT ELIDED
    public long getMediaTime()
	{
	return 0;
	}

    // JAVADOC COMMENT ELIDED
    public int getState()
	{
	return 0;
	}

    // JAVADOC COMMENT ELIDED
    public long getDuration()
	{
	return 0;
	}

    // JAVADOC COMMENT ELIDED
    public String getContentType()
	{
	return "";
	}


    // JAVADOC COMMENT ELIDED

    public void setLoopCount(int count)
	{}

    // JAVADOC COMMENT ELIDED
    public void addPlayerListener(PlayerListener playerListener)
	{}

    // JAVADOC COMMENT ELIDED
    public void removePlayerListener(PlayerListener playerListener)
	{}
	
	private native void playerStart(String aType, byte[] aData);
	private native void playerStop();	
}
