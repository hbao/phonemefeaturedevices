package makedep;

import java.io.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import config.Configurator;

public class SymbianProjectMakeWINSCW extends SymbianIDETool
{
    public static void main(String args[]) 
    {
        SymbianProjectMakeWINSCW tool = new SymbianProjectMakeWINSCW();
        try 
        {
            tool.execute(args);
        } 
        catch (Exception e) 
        {
            e.printStackTrace();
            System.exit(-1);
        }
    }
	
	protected void createProjects(BldInf aInf)  throws Exception
	{
	   // (1) API project
        NMakeJavaAPIProject apiProject = new NMakeJavaAPIProject(this,"javaapi");

        // (2) Loop generator project
        NMakeLoopgenProject loopgenProject = new NMakeLoopgenProject(this,"loopgen");
        
        // (3) Interpreter Loop project
        NMakeLoopProject loopProject = new NMakeLoopProject(this,"loop");
        
        // (4) Romgen project
        NMakeRomgenProject romgenProject = new NMakeRomgenProject(this,"romgen");
        
        // (5) MIDP profile project
        NMakeMIDPProjectWINSCW MIDPProject = new NMakeMIDPProjectWINSCW(this,"midp");
        
        // (6) Rom project
        NMakeRomProject romimageProject = new NMakeRomProject(this,"rom");
        
        // (7) lib project
        SymbianProject lib = new SymbianProject(this,"cldc_vm_lib",aInf);

        aInf.addProject(apiProject,"nmakefile ",".mak");
        aInf.addProject(loopgenProject,"nmakefile ",".mak");
        aInf.addProject(loopProject,"nmakefile ",".mak");
        aInf.addProject(romgenProject,"nmakefile ",".mak");
        aInf.addProject(MIDPProject,"nmakefile ",".mak");
        aInf.addProject(romimageProject,"nmakefile ",".mak"); // build romimange after making the midp libs
        aInf.addProject(lib,"",".mmp");
	}
    
    // IDETool 
    public String getExecutablePath(String name) 
    {
        Object path = getUserSettings().get(name);
        if (path != null && (path instanceof String)) 
        {
            String result = getAbsolutePath((String)path);
            //System.out.println(name + " = " + result);
            return result;
        } 
        else 
        {
            return name;
        }
    }
    String getOutputFileFullPath(String f) 
    {
        return buildspace + File.separator + "sym_i386" + File.separator + f;
    }

}
