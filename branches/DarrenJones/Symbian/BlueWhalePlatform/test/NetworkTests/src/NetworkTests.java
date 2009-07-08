import java.util.Timer;
import java.util.TimerTask;

import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;
import com.sun.midp.io.j2me.storage.RandomAccessStream;
import javax.microedition.io.*;
import java.io.*;


public class NetworkTests extends MIDlet
{

    public NetworkTests()
    {
        // TODO Auto-generated constructor stub
    }

    protected void destroyApp(boolean arg0) throws MIDletStateChangeException
    {
        // TODO Auto-generated method stub

    }

    protected void pauseApp()
    {
        // TODO Auto-generated method stub

    }

    private void runTests(String aName)
    {
		try
        {
			RandomAccessStream fileStream = new RandomAccessStream();
			fileStream.connect("results.txt", RandomAccessStream.READ_WRITE_TRUNCATE);
			String start = "starting test\n";
			fileStream.writeBytes(start.getBytes(),0,start.getBytes().length);
			for(int i=0;i<10;i++)
			{
				SocketConnection sc = null;
				OutputStream os = null; 
				try
				{
					sc = (SocketConnection) Connector.open("socket://127.0.0.1:8173");
					os = sc.openOutputStream();
					String msg = "test " + Integer.toString(i) + "\n";
					fileStream.writeBytes(msg.toString().getBytes(), 0, msg.toString().getBytes().length);
					for(int j=0;j<10;j++)
					{
						String msg2 = aName + " " +Thread.currentThread().getName() + " "  + Integer.toString(j) + "asdasdasdasdasdasdasdsdfsdfsdfsdsflksdfjsdfjsdf;lksjdfkls;dfsdfklsdf;lksdjfskl;dfjsdl;kfj\r\n";
						os.write(msg2.getBytes());
					}
				}
				catch(Exception e)
				{
					String msg = "Exception " + e.toString() + "\n";
					fileStream.writeBytes(msg.toString().getBytes(), 0, msg.toString().getBytes().length);
				}
				finally
				{
					if(os != null)
					{
						os.close();
						os = null;
					}
					if(sc != null)
					{
						sc.close();
						sc = null;
					}
				}
			}
			String end = "tests Complete\n";
			fileStream.writeBytes(end.getBytes(),0,end.getBytes().length);
			fileStream.close();
        }
        catch(Exception e)
        {
            System.out.println("network exception " + e);
        }
    }
    
    protected void startApp() throws MIDletStateChangeException
    {
        System.out.println("Starting network tests");
        Timer timer = new Timer();
        Thread task = null;
		for(int i=0;i<2;i++)
		{
	        task = new Thread()
	        {
	            public void run()
	            {
	                try
	                {
	                    runTests("Task ");
	                    notifyDestroyed();
	                }
	                catch(Exception e)
	                {}
	            }
	        };
			task.start();
		}
    }
}
