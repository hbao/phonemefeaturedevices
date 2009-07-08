package makedep;

import java.io.*;
import java.util.*;

class BldInf extends IDEOutputFile 
{
    private Vector projects;
    private Vector exports;
	public Vector osVersions;
    
    public BldInf(IDETool tool, Vector aOsVersions) 
    {
        super(tool);
        this.projects = new Vector();
        this.exports = new Vector();
        this.osVersions = new Vector(aOsVersions);
    }

    public void addProject(IDEProject proj,String aType,String aExt) 
    {
        projects.addElement(new BldInfProjectType(proj,aType,aExt));
    }
    
    public void addExport(String aFile)
    {
        String file;
        if(aFile.startsWith("c:") || aFile.startsWith("C:"))
        {
            file = aFile.substring(2);
        }
        else
        {
            file = aFile;
        }
        if (!exports.contains(file))
        {
			exports.addElement(file);
		}
    }
    
    public void write() throws Exception 
    {
        writeProjectFiles();
        writeWorkspaceFile();
    }
    
    void writeProjectFiles() throws Exception 
    {
        for (int i=0; i<projects.size(); i++) 
        {
            IDEProject proj = ((BldInfProjectType)projects.elementAt(i)).iProj;
            proj.write();
        }
    }

    void writeWorkspaceFile() throws Exception 
    {
        for (int i = 0; i < osVersions.size(); i++) 
        {
			String osVersion = (String)osVersions.elementAt(i);
			openOutputFile("group" + osVersion + "\\bld.inf");
			writeHeader();
			writeProjects(osVersion);
			writePlatforms();
			writeExports();
		    //writeFooter();
	        closeOutputFile();
		}
    }
    void writeHeader()
    {
        putln("// Copyright 2007");
        putln();
    }
    void writeProjects(String aOsVersion) throws Exception 
    {
        putln("PRJ_MMPFILES");
        for (int i=0; i<projects.size(); i++) 
        {
            BldInfProjectType proj = (BldInfProjectType)projects.elementAt(i);
            writeProject(proj, aOsVersion);
        }
        putln();
    }
    
    void writeProject(BldInfProjectType proj, String aOsVersion) throws Exception 
    {
		if (proj.iExt.equals(".mmp"))
		{
			if (proj.iProj.getName().equals("cldc_vm_lib"))
			{
			putln(proj.iType + "..\\" + proj.iProj.getName() + "\\vm" + aOsVersion + proj.iExt);
			}
			else
			{
			putln(proj.iType + "..\\" + proj.iProj.getName() + "\\" + proj.iProj.getName() + aOsVersion + proj.iExt);
			}
		}
		else
		{
			putln(proj.iType + "..\\" + proj.iProj.getName() + "\\" + proj.iProj.getName() + proj.iExt);
		}
    }   
    
    void writePlatforms()
    {
        putln("PRJ_PLATFORMS");
        putln();
    }

    void writeExports()
    {
        putln("PRJ_EXPORTS");
        for (int i=0; i<exports.size(); i++) 
        {
            putln((String)exports.elementAt(i));
        }
        putln();
    }
}
