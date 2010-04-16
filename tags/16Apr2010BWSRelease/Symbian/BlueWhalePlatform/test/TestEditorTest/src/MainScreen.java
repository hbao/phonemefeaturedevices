import javax.microedition.io.Connection;
import javax.microedition.io.Connector;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.TextField;

import com.sun.midp.io.j2me.storage.File;
import com.sun.midp.io.j2me.storage.RandomAccessStream;


public class MainScreen extends Form implements CommandListener
{
    private TextField iText;
    private Command iCaptureTextCmd;
    private Command iClearTextCmd;
    private Command iExitCmd;
    private TestEditorTest iParent;
    public MainScreen(TestEditorTest aParent)
    {
        super("Test Editor");
        iParent = aParent;
        this.setCommandListener( this );
        iText = new TextField("TextBox","",255,TextField.ANY);
        iCaptureTextCmd = new Command("Capture",Command.ITEM,0);
        iClearTextCmd = new Command("Clear",Command.ITEM,1);
        iExitCmd = new Command("Exit",Command.ITEM,2);
        append(iText);
        addCommand(iCaptureTextCmd);
        addCommand(iClearTextCmd);
        addCommand(iExitCmd);
        writeData("<xml>");
    }
    protected void writeData(String aData)
    {
        try
        {
            RandomAccessStream fileStream = new RandomAccessStream();
            fileStream.connect("results.txt",Connector.WRITE);
            int pos = fileStream.getSizeOf();
            fileStream.setPosition(pos);
            fileStream.writeBytes(aData.getBytes(), 0, aData.getBytes().length);
            fileStream.close();
        }
        catch(Exception e)
        {}

    }
    
    protected void writeXMLData(String aTag,String aData)
    {
        try
        {
            RandomAccessStream fileStream = new RandomAccessStream();
            fileStream.connect("results.txt",Connector.WRITE);
            int pos = fileStream.getSizeOf();
            fileStream.setPosition(pos);
            String msg = "<" + aTag + ">" + aData + "</" + aTag + ">";
            fileStream.writeBytes(msg.getBytes(), 0, msg.getBytes().length);
            fileStream.close();
        }
        catch(Exception e)
        {}

    }

    public void commandAction(Command c, Displayable d)
    {
        if(c == iCaptureTextCmd)
        {
            writeXMLData("entry",iText.getString());
        }
        else if(c == iClearTextCmd)
        {
            iText.setString("");
        }
        else if(c == iExitCmd)
        {
            writeData("</xml>");
            iParent.closeDown();
        }
    }
}
