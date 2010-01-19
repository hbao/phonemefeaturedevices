package makedep;

import java.io.*;
import java.util.*;

class SymbianJavaApi extends NMakefile
{
    IDEProject iProj;
    public SymbianJavaApi(IDETool tool,String name,IDEProject proj)
    {
        super(tool,name + "\\" + name + ".mak");
        iProj = proj;
    }
    public void writeContents() throws Exception
    {
        writeHeader();
        putln("all: NativesTable.cpp");
        putln();
        writeSymbianRules();
        writeTools();
        writeJCC();
        writeClassesZip();
        writeNativesTable();
        writeClean();
    }
    
    void writeHeader() throws Exception
    {
        putln("# Copyright 2007");
        putln();
        putln();
        putln();
    }
    
    void writeSymbianRules() throws Exception
    {
        putln("MAKMAKE FINAL FREEZE LIB CLEANALL CLEANLIB RESOURCE RELEASABLES SAVESPACE : ");
        putln();
        putln("BLD : NativesTable.cpp");
        putln();

    }

    void writeTools() throws Exception
    {
        putln("JAVAC     = " + tool().getExecutablePath("javac.exe"));
        putln("JAR       = " + tool().getExecutablePath("jar.exe"));
        putln("JAVA      = " + tool().getExecutablePath("java.exe"));
        putln("PREVERIFY = " + tool().getExecutablePath("preverify.exe"));
        putln();
    }

    void writeJCC() throws Exception
    {
        // JCC_SOURCES
        Vector v = tool().getJccSources();
        
        putln("JCC_MF  = " + iProj.getRelativeSourceFile("..\\..\\src\\tools\\jcc\\MANIFEST.MF"));

        put("JCC_SOURCES =");
        for (int i=0; i<v.size(); i++) 
        {
            putln(" \\");
            put("  " + iProj.getRelativeSourceFile((String)v.elementAt(i)));
        }
        putln();
        putln();

        // jcc
        putln("jcc:");
        puttab();
        putln("-@mkdir jcc > NUL 2> NUL");
        putln();

        // jcc.jar
        putln("jcc.jar: jcc $(JCC_SOURCES)");
        puttabln("@echo Creating JCC tool");
        puttabln("@\"$(JAVAC)\" -d jcc @<<");
        putln("$(JCC_SOURCES)");
        putln("<<");
        puttabln("@copy " + iProj.getRelativeSourceFile("..\\..\\src\\tools\\jcc\\JCCMessage.properties") +" jcc");
        puttabln("@\"$(JAR)\" -cfm $@ $(JCC_MF) -C jcc .");
        putln();
    }
    void writeClassesZip() 
    {
        String api_ver;
        if (tool().isOptionEnabled("ENABLE_REFLECTION")) {
            api_ver = "CLDC 1.1 plus";
        } else if (tool().isOptionEnabled("ENABLE_CLDC_11")) {
            api_ver = "CLDC 1.1";
        } else {
            api_ver = "CLDC 1.0";
        }

        // API_SOURCES
        Vector v = tool().getAPISources();
        put("API_SOURCES =");
        for (int i=0; i<v.size(); i++) 
		{
            putln(" \\");
			put("  " + iProj.getRelativeSourceFile((String)v.elementAt(i)));
        }
        putln();
        putln();

        // tmpclasses
        putln("tmpclasses:");
        puttabln("-@mkdir tmpclasses > NUL 2> NUL");
        putln();

        // classes
        putln("classes:");
        puttabln("-@mkdir classes > NUL 2> NUL");
        putln();


        putln("classes.zip: tmpclasses classes $(API_SOURCES)");
        puttabln("@echo Compiling " +  api_ver + " API classes");
        puttabln("-@rmdir /Q /S tmpclasses > NUL 2> NUL");
        puttabln("-@rmdir /Q /S classes > NUL 2> NUL");
        puttabln("-@mkdir tmpclasses > NUL 2> NUL");
        puttabln("-@mkdir classes > NUL 2> NUL");
        puttabln("@\"$(JAVAC)\" -d tmpclasses @<<");
        putln("$(API_SOURCES)");
        putln("<<");
        puttabln("@echo Preverifying classes");
        puttabln("@\"$(PREVERIFY)\" -classpath classes -d classes tmpclasses");
        puttabln("@echo Creating $@");
        puttabln("@\"$(JAR)\" -cf $@ -C classes .");
        putln();
    }
    
    void writeNativesTable() {
        putln("NativesTable.cpp: classes.zip jcc.jar");
        puttabln("@echo Creating $@");
        puttabln("@\"$(JAVA)\" -cp jcc.jar JavaCodeCompact " +
                 "-writer CLDC_HI_Natives -o $@ classes.zip");
        puttabln("@copy NativesTable.cpp ..\\generated");
        putln();
    }

    void writeClean() {
        putln("CLEAN :");
        puttabln("@echo Cleaning output files");
        puttabln("-@rmdir /Q /S jcc          > NUL 2> NUL");
        puttabln("-@rmdir /Q /S tmpclasses   > NUL 2> NUL");
        puttabln("-@rmdir /Q /S classes      > NUL 2> NUL");
        puttabln("-@del /Q jcc.jar           > NUL 2> NUL");
        puttabln("-@del /Q classes.zip       > NUL 2> NUL");
        puttabln("-@del /Q NativesTable.cpp  > NUL 2> NUL");
    }
}

