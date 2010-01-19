import javax.microedition.lcdui.Display;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;


public class TestEditorTest extends MIDlet
{
    private MainScreen iScreen;
    public TestEditorTest()
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

    protected void startApp() throws MIDletStateChangeException
    {
        iScreen = new MainScreen(this);
        Display.getDisplay(this).setCurrent(iScreen); 
    }
    public void closeDown()
    {
        this.notifyDestroyed();
    }

}
