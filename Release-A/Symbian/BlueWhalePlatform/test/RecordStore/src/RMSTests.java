import java.util.Timer;
import java.util.TimerTask;

import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;


public class RMSTests extends MIDlet
{

    public RMSTests()
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

    private void runTests()
    {
        try
        {
            TestRMS tests = new TestRMS();
            tests.runTests();
        }
        catch(Exception e)
        {
            System.out.println("Record store test unexpected exception " + e);
        }
    }
    
    protected void startApp() throws MIDletStateChangeException
    {
        System.out.println("Starting network tests");
        Timer timer = new Timer();
        TimerTask task = null;
        
        task = new TimerTask()
        {
            public void run()
            {
                try
                {
                    runTests();
                    notifyDestroyed();
                }
                catch(Exception e)
                {}
            }
        };
        timer.schedule(task,0);
    }
}
