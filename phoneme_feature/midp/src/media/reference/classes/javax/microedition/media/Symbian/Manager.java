package javax.microedition.media;

import java.io.*;	

public class Manager
{
	public static Player createPlayer(InputStream stream, String type)
	{
		return new MyPlayer(stream,type);
	}
	public static Player createPlayer(String locator) 
	{
		return new MyPlayer(locator);
	}
	public static String[] getSupportedContentTypes(String protocol)
	{
		return null;
	}
	public static String[] 	getSupportedProtocols(String content_type)
	{
		return null;
	}
	public static void playTone(int note, int duration, int volume) throws MediaException
	{
	}
}