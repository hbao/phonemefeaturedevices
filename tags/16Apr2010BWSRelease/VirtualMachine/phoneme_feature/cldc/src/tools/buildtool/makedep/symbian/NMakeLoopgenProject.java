package makedep;

import java.io.*;
import java.util.*;

class NMakeLoopgenProject extends SymbianIDEProject
{
    private NMakefile iMakefile;
    VMSourceHandler vmSourceHandler;
    Database database;
    public NMakeLoopgenProject(IDETool tool,String name)
    {
        super(tool,name,LOOPGEN);
        System.out.println("NMakeLoopgenProject");
        type = LOOPGEN;
        vmSourceHandler = new VMSourceHandler(tool,this,"i386","win32");
        iMakefile = new SymbianLoopgen(tool,name,this);     
    }
    
    public VMSourceHandler getSourceHandler()
    {
        return vmSourceHandler;
    }
    public Database getDatabase()
    {
        return database;
    }
    public void write() throws Exception
    {
        openDatabase();
        iMakefile.write();
        writeJvmConfig();
    }
    void openDatabase()  throws Exception 
    {
		Platform platform = new SymbianNativePlatform();
        platform.setupFileTemplates();
        long t = platform.defaultGrandIncludeThreshold();

        database = new Database(platform, t);

		tool().makeDirExist(getOutputDir());
        tool().makeDirExist(getOutputDir() + File.separator + "incls");
        database.setOutputDir(tool().getOutputFileFullPath(getOutputDir()));


        Properties globalProps = new Properties();
        if (tool().isOptionEnabled("ENABLE_JAVA_DEBUGGER")) {
            globalProps.put("ENABLE_JAVA_DEBUGGER", "true");
        }
        if (tool().isOptionEnabled("ENABLE_ROM_JAVA_DEBUGGER")) {
            globalProps.put("ENABLE_ROM_JAVA_DEBUGGER", "true");
        }
        if (tool().isOptionEnabled("ENABLE_ISOLATES")) {
            globalProps.put("ENABLE_ISOLATES", "true");
        }
        if (tool().isOptionEnabled("ENABLE_METHOD_TRAPS")) {
            globalProps.put("ENABLE_METHOD_TRAPS", "true");
        }
        if (tool().isOptionEnabled("ENABLE_DYNUPDATE")) {
            globalProps.put("ENABLE_DYNUPDATE", "true");
        }
        globalProps.put("ENABLE_ISOLATES", "true");
        globalProps.put("ENABLE_GENERATE_SOURCE_ROM_IMAGE", "true");
        globalProps.put("ENABLE_INTERPRETER_GENERATOR", "true");
        
        database.get(tool().getOutputFileFullPath("platformwin32.cfg"), tool().getDatabaseArg(),
                     globalProps);

        database.setResolveVpath(true);
        database.setWorkspace(tool().getWorkSpaceArg());
  
        database.compute();
    }
}