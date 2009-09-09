package makedep;

import java.io.*;
import java.util.*;

class SymbianProject extends SymbianIDEProject
{
    Database database;
    BldInf iInf;
    public SymbianProject(IDETool tool, String name,BldInf inf) 
    {
        super(tool, name,TARGET);
        iSrcHandler = new VMSourceHandler(tool,this,"i386","symbian");
        iInf = inf;
    }

    void openDatabase()  throws Exception 
    {
        Platform platform = new WinGammaPlatform();
        platform.setupFileTemplates();
        long t = platform.defaultGrandIncludeThreshold();

        database = new Database(platform, t);
        
		tool().makeDirExist(getOutputDir());
        tool().makeDirExist(getOutputDir() + File.separator + "incls");
        
		database.setOutputDir(tool().getOutputFileFullPath(getOutputDir()));

        Properties globalProps = new Properties();
        if (tool().isOptionEnabled("ENABLE_JAVA_DEBUGGER")) 
        {
            globalProps.put("ENABLE_JAVA_DEBUGGER", "true");
        }
        if (tool().isOptionEnabled("ENABLE_ROM_JAVA_DEBUGGER")) 
        {
            globalProps.put("ENABLE_ROM_JAVA_DEBUGGER", "true");
        }
        if (tool().isOptionEnabled("ENABLE_ISOLATES")) 
        {
            globalProps.put("ENABLE_ISOLATES", "true");
        }
        if (this.type == TARGET) 
        {
            if (tool().isOptionEnabled("ENABLE_MONET")) 
            {
                globalProps.put("ENABLE_MONET", "true");
            }
        }
        if (tool().isOptionEnabled("ENABLE_METHOD_TRAPS")) 
        {
            globalProps.put("ENABLE_METHOD_TRAPS", "true");
        }
        if (tool().isOptionEnabled("ENABLE_DYNUPDATE")) 
        {
            globalProps.put("ENABLE_DYNUPDATE", "true");
        }

		globalProps.put("ENABLE_ISOLATES", "true");
        
        globalProps.put("ENABLE_GENERATE_SOURCE_ROM_IMAGE", "true");
        globalProps.put("ENABLE_INTERPRETER_GENERATOR", "true");

        globalProps.put("ENABLE_GENERATE_SOURCE_ROM_IMAGE", "true");
        globalProps.put("ENABLE_INTERPRETER_GENERATOR", "true");
		
		database.get(tool().getPlatformArg(), tool().getDatabaseArg(),
                     globalProps);
        database.compute();
    }
        
    public void write() throws Exception 
    {
        openDatabase();
        for (int i=0; i < iInf.osVersions.size(); i++)
        {
			String osVersion = (String)iInf.osVersions.elementAt(i);
			if (getName().equals("cldc_vm_lib"))
			{
				openOutputFile(getName()+ "\\vm" + osVersion + ".mmp");
			}
			else
			{
				openOutputFile(getName()+ "\\" + getName() + osVersion + ".mmp");
			}
			writeHeader();
			writeTarget();
			writeDefines(osVersion);
			writeSource();
			writeInclude();
			writeLibraries();
			writeStaticLibraries();
			writeDefFileLine();
			closeOutputFile();
		}
        writeIncls();
        writeJvmConfig();
		// create out fixed def file
		createDefFile();
    }
    
    void writeDefines(String aOsVersion) throws Exception
    {
        Vector v = getBaseCppDefines();
        for (int i=0; i<v.size(); i++) 
        {
            putln("MACRO " + v.elementAt(i));
        }
        if (aOsVersion.equals("_s60v3fp0"))
        {
            putln("MACRO __DONT_USE_IN_CODE__MMP_ONLY__S60_V3_FP0__");
        }
        else if (aOsVersion.equals("_s60v2fp3"))
        {
            putln("MACRO __DONT_USE_IN_CODE__MMP_ONLY__S60_V2_FP3__");
        }
        else if (aOsVersion.equals("_s60v2fp2"))
        {
            putln("MACRO __DONT_USE_IN_CODE__MMP_ONLY__S60_V2_FP2__");
        }		
		else if (aOsVersion.equals("_uiqv3fp0"))
		{
		    putln("MACRO __DONT_USE_IN_CODE__MMP_ONLY__UIQ_V3_FP0__");
        }
    }
    
    void writeLibraries() throws Exception
    {
        Vector v = getLibraries();
        for (int i=0; i<v.size(); i++) 
        {
            putln("LIBRARY " + v.elementAt(i));
        }
    }
    
    void writeStaticLibraries() throws Exception
    {
        Vector v = getStaticLibraries();
        for (int i=0; i<v.size(); i++) 
        {
            putln("STATICLIBRARY " + v.elementAt(i));
        }
    }
    void writeDefFileLine() throws Exception
	{
		putln();
		putln("deffile .\\cldc_vm_lib_wins.def");
		putln("NOSTRICTDEF");
	}
	
	void createDefFile()  throws Exception
	{
		FileOutputStream out = new FileOutputStream(tool().getOutputFileFullPath(getOutputDir() + "\\cldc_vm_lib_wins.def"));
        out.write("EXPORTS\n".getBytes());
        out.write("?AddL@CVMArguments@@QAEXABVTDesC8@@@Z @ 1 NONAME ; void CVMArguments::AddL(class TDesC8 const &)\n".getBytes());
        out.write("?AddL@CVMProperties@@QAEXABVTDesC8@@0@Z @ 2 NONAME ; void CVMProperties::AddL(class TDesC8 const &, class TDesC8 const &)\n".getBytes());
        out.write("?Count@CVMArguments@@QAE?BHXZ @ 3 NONAME ; int const CVMArguments::Count(void)\n".getBytes());
        out.write("?Count@CVMProperties@@QAE?BHXZ @ 4 NONAME ; int const CVMProperties::Count(void)\n".getBytes());
        out.write("?ImplementationGroupProxy@@YAPBUTImplementationProxy@@AAH@Z @ 5 NONAME ; struct TImplementationProxy const * ImplementationGroupProxy(int &)\n".getBytes());
        out.write("?NewLC@CVMArguments@@SAPAV1@XZ @ 6 NONAME ; class CVMArguments * CVMArguments::NewLC(void)\n".getBytes());
        out.write("?NewLC@CVMProperties@@SAPAV1@XZ @ 7 NONAME ; class CVMProperties * CVMProperties::NewLC(void)\n".getBytes());
        out.write("?RunVMCode@@YAHPBGPADAAVCVMProperties@@AAVCVMArguments@@PAVMApplication@@@Z @ 8 NONAME ; int RunVMCode(unsigned short const *, char *, class CVMProperties &, class CVMArguments &, class MApplication *)\n".getBytes());

out.close();
	}
	
    Vector getLibraries()
    {
        Vector v = new Vector();
        v.addElement("euser.lib");
        v.addElement("estlib.lib");
        v.addElement("esock.lib");
        v.addElement("insock.lib");
        v.addElement("efsrv.lib");
        v.addElement("ECOMPLUS.lib");
		v.addElement("smcm.lib");
		v.addElement("msgs.lib");
		v.addElement("gsmu.lib");
		v.addElement("etext.lib");
		v.addElement("bafl.lib");
        return v;
    }
    
    Vector getStaticLibraries() 
    {
        Vector v = new Vector();
        v.addElement("pcsl_file.lib");
        v.addElement("pcsl_memory.lib");
        v.addElement("pcsl_network.lib");
        v.addElement("pcsl_print.lib");
        v.addElement("pcsl_string.lib");
        return v;
    }

    Vector getBaseCppDefines() 
    {
        Vector v = new Vector();
        v.addElement("DEBUG");
        v.addElement("ENABLE_PCSL");
        v.addElement("ENABLE_MULTIPLE_ISOLATES");
        v.addElement("ROMIZING");
        v.addElement("AZZERT");
        
        return v;
    }

    void writeIncls() throws Exception 
    {
		Platform platform = new SymbianPlatform(tool().getOutputFileFullPath(getOutputDir()),tool().getWorkSpaceArg());
        platform.setupFileTemplates();
        long t = platform.defaultGrandIncludeThreshold();

        Database localDatabase = new Database(platform, t);
		
		localDatabase.setResolveVpath(true);
		localDatabase.setWorkspace(tool().getWorkSpaceArg());
		
		Properties globalProps = new Properties();
        if (tool().isOptionEnabled("ENABLE_JAVA_DEBUGGER")) 
        {
            globalProps.put("ENABLE_JAVA_DEBUGGER", "true");
        }
        if (tool().isOptionEnabled("ENABLE_ROM_JAVA_DEBUGGER")) 
        {
            globalProps.put("ENABLE_ROM_JAVA_DEBUGGER", "true");
        }
        if (tool().isOptionEnabled("ENABLE_ISOLATES")) 
        {
            globalProps.put("ENABLE_ISOLATES", "true");
        }
        if (this.type == TARGET) 
        {
            if (tool().isOptionEnabled("ENABLE_MONET")) 
            {
                globalProps.put("ENABLE_MONET", "true");
            }
        }
        if (tool().isOptionEnabled("ENABLE_METHOD_TRAPS")) 
        {
            globalProps.put("ENABLE_METHOD_TRAPS", "true");
        }
        if (tool().isOptionEnabled("ENABLE_DYNUPDATE")) 
        {
            globalProps.put("ENABLE_DYNUPDATE", "true");
        }

		globalProps.put("ENABLE_ISOLATES", "true");
        
        globalProps.put("ENABLE_GENERATE_SOURCE_ROM_IMAGE", "true");
        globalProps.put("ENABLE_INTERPRETER_GENERATOR", "true");

        globalProps.put("ENABLE_GENERATE_SOURCE_ROM_IMAGE", "true");
        globalProps.put("ENABLE_INTERPRETER_GENERATOR", "true");

		localDatabase.setOutputDir(tool().getOutputFileFullPath(getOutputDir()));
		localDatabase.get(tool().getPlatformArg(), tool().getDatabaseArg(),globalProps);
		localDatabase.compute();
        localDatabase.put();
	}

    void writeHeader()
    {
        putln("// Copyright 2007 BlueWhaleSystems Ltd.");
        putln();
    }
    
    void writeTarget()
    {
        putln("TARGET\t\t" + getName() + ".dll");
        putln("TARGETTYPE\tdll");
        putln("UID\t0x1000008d 0x2000E279");
        putln("CAPABILITY\tNetworkServices ReadUserData WriteUserData ReadDeviceData WriteDeviceData");
        putln();
    }
    void addFile(Vector aElements, String aFile)
    {
        String file;
        if(aFile.startsWith(tool().getWorkSpaceArg()))
        {
            file = "..\\..\\.." + aFile.substring(tool().getWorkSpaceArg().length());
        }
        else
        {
            file = aFile;
        }
        aElements.addElement(file);
    }
    void writeSource() throws Exception 
    {
        Vector sourceFiles = new Vector();
        for (Iterator iter = database.getAllFiles().iterator();iter.hasNext(); ) 
        {
            FileList fl = (FileList) iter.next();
            String fileName = fl.getName();
            if (isFileIncludedInSources(fileName)) 
            {
                fileName = iSrcHandler.resolveFileForProject(fileName);
                addFile(sourceFiles,fileName);
            }
            if(fileName.endsWith("_symbian.hpp"))
            {
				iInf.addExport(iSrcHandler.resolveFileForProject(fileName));
            }
            if(fileName.endsWith("jvm.h"))
            {
				iInf.addExport(iSrcHandler.resolveFileForProject(fileName));
            }
            if(fileName.endsWith("kni.h"))
            {
				iInf.addExport(iSrcHandler.resolveFileForProject(fileName));
            }
            if(fileName.endsWith("kni_md.h"))
            {
				iInf.addExport(iSrcHandler.resolveFileForProject(fileName));
            }
            if(fileName.endsWith("sni.h"))
            {
				iInf.addExport(iSrcHandler.resolveFileForProject(fileName));
            }
            if(fileName.endsWith("jvmspi.h"))
            {
				iInf.addExport(iSrcHandler.resolveFileForProject(fileName));
            }
        }
        // Add extra source specific to this VM. For example, a
        // loopgen VM would add InterpreterSkeleton.cpp
        addExtraSources(sourceFiles);
        for (int i=0; i<sourceFiles.size(); i++) 
        {
            String fileName = (String)sourceFiles.elementAt(i);
            putln("SOURCE\t" + fileName);
        }
        // Write the asm files as well
		switch (this.type) {
        case TARGET:
        case ROMGEN:
            writeAsmAndOopMapsFileRules();
            break;
        }

    }
    
    void writeAsmAndOopMapsFileRules() throws Exception 
    {
        writeAsmAndOopMapsFileRules(true);
        writeAsmAndOopMapsFileRules(false);

        createDummyAsmAndOopMapsFiles(true);
        createDummyAsmAndOopMapsFiles(false);
    }

	void writeAsmAndOopMapsFileRules(boolean isAsm) 
	{
        String srcName, stem, ext;
		if (isAsm) 
        {
            stem = "Interpreter_i386";
            ext = ".c";
         } 
         else 
         {
            stem = "OopMaps";
            ext = ".cpp";
         }
         srcName = ".\\debug\\" + stem + ext;
         putln("SOURCE\t" + srcName);
    }

	void createDummyAsmAndOopMapsFiles(boolean isAsm) throws Exception 
	{
        if (isAsm) 
        {
            String asmName = "Interpreter_i386.s";
            PrintWriter writer = tool().openOutputFile(asmName);
            writer.println("\t;Dummy file.");
            writer.println("\t;Will be overwritten during compilation.");
            writer.println("\t;This file is auto-generated.");
            writer.println("\t;Do not edit.");
            writer.close();
        } 
        else 
        {
            String asmName = "OopMaps.cpp";
            PrintWriter writer = tool().openOutputFile(asmName);
            writer.println("// Dummy file.");
            writer.println("// Will be overwritten during compilation.");
            writer.println("// This file is auto-generated.");
            writer.println("// Do not edit.");
            writer.close();
        }
	}

	public void addExtraSources(Vector lst) 
	{
		super.addExtraSources(lst);
	}

    public boolean isFileIncludedInSources(String fileName) 
    {
		fileName = fileName.toLowerCase();
        if (!fileName.endsWith(".cpp") && !fileName.endsWith(".c")) 
        {
            return false;
        }

        if (fileName.endsWith("nativestable.cpp")) 
        { // this file is generated
            return false;
        }

        if (fileName.equals("romimage.cpp")) 
        { // this file is generated
            return false;
        }

        return true;
    }
    
    void writeInclude()
    {
        Vector vpaths = iSrcHandler.getVpaths();
        for (int i=0; i<vpaths.size(); i++) 
        {
            String path = (String)vpaths.elementAt(i);
            String file;
            if(getRelativeSourceFile(path).startsWith(tool().getWorkSpaceArg()))
            {
                file = "..\\..\\.." + getRelativeSourceFile(path).substring(tool().getWorkSpaceArg().length());
            }
            else
            {
                file = getRelativeSourceFile(path);
            }
            putln("USERINCLUDE \"" + file + "\" ");
        }
    putln("USERINCLUDE ..\\" + getOutputDir() + File.separator + "incls");
    putln("USERINCLUDE .");
	putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\security\\crypto\\include");
	putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\ams\\suitestore\\internal_api\\include");
	putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\ams\\suitestore\\installer_api\\include");
	putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\ams\\ams_base\\include");
	putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\ams\\ams_base_cldc\\include");
	putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\ams\\suitestore\\common_api\\include");
	putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\ams\\suitestore\\task_manager_api\\include");
	putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\ams\\suitestore\\recordstore_api\\include");
	putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\ams\\platform_request\\include");
    putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\highlevelui\\annunciator\\include");
    putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics_api\\gxapi_native\\native");
    putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\security\\file_digest\\include");
    putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\core\\suspend_resume\\sr_main\\include");
    putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\core\\suspend_resume\\sr_vm\\include");
    putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\core\\suspend_resume\\sr_port\\include");
    putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\rms\\record_store\\include");
    putln("USERINCLUDE ..\\..\\..\\..\\jsr75\\src\\cldc_application\\native\\share");
	putln("USERINCLUDE ..\\..\\..\\..\\jsr75\\src\\cldc_application\\native\\share\\inc");
	putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\core\\string\\include");
	putln("USERINCLUDE ..\\..\\..\\..\\midp\\src\\push\\push_timer\\include");
	putln("USERINCLUDE ..\\..\\..\\..\\javacall\\implementation\\symbian");
	putln("USERINCLUDE ..\\..\\..\\..\\javacall\\implementation\\share\\utils\\inc");
	putln("USERINCLUDE ..\\..\\..\\..\\javacall\\interface");
	putln("USERINCLUDE ..\\..\\..\\..\\javacall\\interface\\common");
	putln("USERINCLUDE ..\\..\\..\\..\\javacall\\interface\\jsr211_chapi");
	putln("USERINCLUDE ..\\..\\..\\..\\abstractions\\src\\cldc_application\\native\\symbian");
	putln("USERINCLUDE ..\\..\\..\\..\\abstractions\\src\\cldc_application\\native\\include");
	putln("USERINCLUDE ..\\..\\..\\..\\abstractions\\src\\share\\include");
	putln("USERINCLUDE ..\\..\\..\\..\\jsr211\\src\\share\\core\\native\\symbian");
	putln("USERINCLUDE ..\\..\\..\\..\\jsr211\\src\\share\\core\\native\\include");
	putln("USERINCLUDE ..\\..\\..\\..\\jsr211\\src\\cldc_application\\core\\native\\inc");
	putln("SYSTEMINCLUDE \\epoc32\\include \\epoc32\\include\\libc ..\\generated . \\epoc32\\include\\mmf\\plugin");
    }
    
}

