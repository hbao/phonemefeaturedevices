package makedep;

import java.util.*;

class SymbianLoopgen extends NMakefile
{
    IDEProject iProj;
    String exeName;
    public SymbianLoopgen(IDETool tool,String name,IDEProject proj)
    {
        super(tool,name + "\\" + name + ".mak");
        iProj = proj;
        exeName = new String(name + ".exe");
    }
    
    public void writeContents() throws Exception
    {
        writeHeader();
        putln();
        writeSymbianRules();
		writeRules();
        writeTools();
        writeProjectBlock();
        writeSourceFileRules();
        writeHeaderFileRules();

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
        putln("all : " + exeName);
		putln();
        putln("MAKMAKE FINAL FREEZE LIB CLEANALL CLEANLIB RESOURCE RELEASABLES : ");
        putln();
        putln("BLD : all SAVESPACE");
        putln();
    
    }
  
	void writeRules() throws Exception
    {
		putln("SAVESPACE:");
		puttabln("@echo Saving loopgen space");
		puttabln("-@del *.obj");
		puttabln("-@del *.sbr");
	    putln();
        putln("clean: SAVESPACE");
        puttabln("-@del *.idb");
	    puttabln("-@del *.pdb");
	    puttabln("-@del incls\\*.pch");
		puttabln("-@del *.exe");
	    putln();
    }
    
    void writeSourceFileRules() throws Exception
    {
        Vector sourceFiles = new Vector();

        for (Iterator iter = ((NMakeLoopgenProject)iProj).getDatabase().getOuterFiles().iterator();iter.hasNext(); ) 
        {
            FileList fl = (FileList) iter.next();
            String fileName = fl.getName();
            if (isFileIncludedInSources(fileName))
            {
                String stemName = removeSuffixFrom(fileName);
                put(stemName + ".obj : ");
                fileName = ((NMakeLoopgenProject)iProj).getSourceHandler().resolveFileForProject(fileName);
                putln(fileName);
                puttabln("$(CPP) $(CPPFLAGS) \\");
                putCppFlags(isFileIncludedInSources(fileName));
                putln(fileName);
            }
        }

        // Add extra source specific to this VM. For example, a
        // loopgen VM would add InterpreterSkeleton.cpp
        addExtraSources(sourceFiles);

        for (int i=0; i<sourceFiles.size(); i++) 
        {
            String fileName = (String)sourceFiles.elementAt(i);
            String stemName = getFileNameOnly(removeSuffixFrom(fileName));
            put(stemName + ".obj : ");
            putln(fileName);
            puttabln("$(CPP) $(CPPFLAGS) \\");
            putCppFlags(isFileIncludedInSources(fileName));
            putln(fileName);
            
            if(fileName.endsWith("ROMImage.cpp"))
            {
                putln();
                putln(fileName + " : ");
                puttabln("@copy " +((NMakeLoopgenProject)iProj).getSourceHandler().resolveFileForProject("ROMSkeleton.cpp") + " ..\\generated\\ROMImage.cpp");
            }

        }
        putln();
    }
    
    protected String removeSuffixFrom(String s) 
    {
      int idx = s.lastIndexOf('.');
      return s.substring(0, idx);
    }
    
    protected String getFileNameOnly(String s) 
    {
      int idx = s.lastIndexOf('\\');
      return s.substring(idx+1,s.length());
    }
    
    public boolean mayUsePrecompiledHeader(String fileName) 
    {
        fileName = fileName.toLowerCase();
        if (fileName.endsWith("romimage.cpp")) {
            return false;
        }
        if (fileName.endsWith("romskeleton.cpp")) {
            return false;
        }
        if (fileName.endsWith("nativestable.cpp")) {
            return false;
        }
        if (fileName.endsWith("os_win32.cpp")) {
            return false;
        }

        return true;
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

        if (fileName.endsWith("romimage.cpp")) 
        { // this file is generated
            return false;
        }

        return true;
    }
    public void addExtraSources(Vector lst) 
    {
        lst.addElement("..\\generated\\NativesTable.cpp");
        VMSourceHandler h = ((NMakeLoopgenProject)iProj).getSourceHandler();

        lst.addElement(h.resolveFileForProject("InterpreterSkeleton.cpp"));
        lst.addElement(h.resolveFileForProject("OopMapsSkeleton.cpp"));
        lst.addElement("..\\generated\\ROMImage.cpp");

    }

    void writeHeaderFileRules() throws Exception
    {
        System.out.println("SymbianLoopgen::writeHeaderFileRules");
        writeIncls();
    }

    void writeIncls() throws Exception 
    {
        ((NMakeLoopgenProject)iProj).getDatabase().setResolveVpath(true);
        ((NMakeLoopgenProject)iProj).getDatabase().put();
    }

    public void addConfig(Vector v, String configName, String outputDir,String baseName) 
    {
        IDEProjectConfig config = new IDEProjectConfig();
        config.name = iProj.getName() + " - " + configName;
        config.base = baseName;
        config.outputDir = outputDir;
        v.addElement(config);
    }
    
    public void writeProjectBlock() 
    {
        put("CPPFLAGS = ");
        putBaseCppFlags();
        putln("/FR /FD /c \\");

        putIncludePaths();
        
        
        String linkLine = "/nologo /subsystem:console /incremental:no /machine:I386";
        
        putln();
        putln("LINK_FLAGS = /LIBPATH:\"$(WINDOWS_PLATFORM_SDK)\\Lib\" "  + linkLine);
        putln("LIBS = gdi32.lib user32.lib wsock32.lib");
        putln();
        putln(exeName + " : $(Obj_Files) InterpreterSkeleton.obj OopMapsSkeleton.obj");
        puttabln("$(LINK) $(LINK_FLAGS) $(LIBS) $(Obj_Files) \\");
        puttabln("InterpreterSkeleton.obj OopMapsSkeleton.obj \\");
        puttabln("/out:$@");
        puttabln("@echo generated /$@");
        putln();
        
    }
    void putBaseCppFlags() 
    {
        put("/nologo ");
        put("/W3 /Zi /Od ");

        Vector v = getBaseCppDefines();
        for (int i=0; i<v.size(); i++) 
        {
            put("/D \"" + v.elementAt(i) + "\" ");
        }
    }
    
    Vector getBaseCppDefines() 
    {
        Vector v = new Vector();
        v.addElement("WIN32");

        v.addElement("NDEBUG");
        v.addElement("_CRT_SECURE_NO_DEPRECATE");
        return v;
    }

    void putIncludePaths() 
    {
		Vector vpaths = ((NMakeLoopgenProject)iProj).getSourceHandler().getVpaths();
        put("/I \"$(GEN_DIR)\" ");
        put("/I \"$(WINDOWS_PLATFORM_SDK)\\Include\" ");
        for (int i=0; i<vpaths.size(); i++) 
        {
            String path = (String)vpaths.elementAt(i);
            put("/I \"" + iProj.getRelativeSourceFile(path) + "\" ");
        }
    }

    void putCppFlags(boolean usePCH) 
    {
        Vector v = getCppDefines();
        for (int i=0; i<v.size(); i++) 
        {
            put("/D \"" + v.elementAt(i) + "\" ");
        }
        if (usePCH) 
        {
            put("/FR /Yc\"incls/_precompiled.incl\" /FD ");
        }
    }

    Vector getCppDefines() 
    {
        Vector v = new Vector();

        return v;
    }

    void writeTools() throws Exception
    {
        putln("CPP=cl.exe");
        putln("LINK=link.exe");
        putln("GEN_DIR = .");
        putln();
        putln("include Dependencies");
        putln();
    }
}

