package makedep;

import java.io.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import config.Configurator;

public class SymbianProjectMakeARM extends SymbianIDETool 
{
    public static void main(String args[]) 
    {
        SymbianProjectMakeARM tool = new SymbianProjectMakeARM();
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
		NMakeLoopgenProjectARM loopgenProject = new NMakeLoopgenProjectARM(this,"loopgen");
        
		// (3) Interpreter Loop project
		NMakeLoopProjectARM loopProject = new NMakeLoopProjectARM(this,"loop");
 
		// (4) Romgen project
		NMakeRomgenProjectARM romgenProject = new NMakeRomgenProjectARM(this,"romgen");
		
		// (5) Lightwieght profile project
		NMakeMIDPProjectARM MIDPProject = new NMakeMIDPProjectARM(this,"midp");

		// (6) Rom project
		NMakeRomProjectARM romimageProject = new NMakeRomProjectARM(this,"rom");
				
		// (7) lib project
		SymbianProjectARM lib = new SymbianProjectARM(this,"cldc_vm_lib",aInf);

		aInf.addProject(apiProject,"nmakefile ",".mak");
		aInf.addProject(loopgenProject,"nmakefile ",".mak");
		aInf.addProject(loopProject,"nmakefile ",".mak");
		aInf.addProject(romgenProject,"nmakefile ",".mak");
		aInf.addProject(MIDPProject,"nmakefile ",".mak");
		aInf.addProject(romimageProject,"nmakefile ",".mak");
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
		return buildspace + File.separator + "sym_arm" + File.separator + f;
    }

}
