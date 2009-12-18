package makedep;

import java.util.*;

class SymbianLoopgenARM extends SymbianLoopgen
{
	public SymbianLoopgenARM(IDETool tool,String name,IDEProject proj)
	{
		super(tool,name,proj);
	}
	
	public void writeContents() throws Exception
	{
		writeHeader();
		putln();
		writeSymbianRules();
		writeTools();
		writeProjectBlock();
		writeSourceFileRules();
        writeHeaderFileRules();

	}
	
	void writeSymbianRules() throws Exception
	{
		putln("all : " + exeName);
		puttabln("-@del *.obj");
		puttabln("-@del *.sbr");
		putln();
		putln("MAKMAKE FINAL FREEZE LIB CLEAN CLEANALL CLEANLIB RESOURCE RELEASABLES SAVESPACE : ");
		putln();
		putln("BLD : all");
		putln();
	
	}
	
	void writeSourceFileRules() throws Exception
	{
	    Vector sourceFiles = new Vector();

        for (Iterator iter = ((NMakeLoopgenProjectARM)iProj).getDatabase().getOuterFiles().iterator();iter.hasNext(); ) 
        {
            FileList fl = (FileList) iter.next();
            String fileName = fl.getName();
            if (isFileIncludedInSources(fileName))
            {
				String stemName = removeSuffixFrom(fileName);
            	put(stemName + ".obj : ");
            	fileName = ((NMakeLoopgenProjectARM)iProj).getSourceHandler().resolveFileForProject(fileName);
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
				puttabln("@copy " +((NMakeLoopgenProjectARM)iProj).getSourceHandler().resolveFileForProject("ROMSkeleton.cpp") + " ..\\generated\\ROMImage.cpp");
			}

        }
		putln();
    }

	public void addExtraSources(Vector lst) 
	{
        lst.addElement("..\\generated\\NativesTable.cpp");
        VMSourceHandler h = ((NMakeLoopgenProjectARM)iProj).getSourceHandler();

		lst.addElement(h.resolveFileForProject("InterpreterSkeleton.cpp"));
        //lst.addElement(h.resolveFileForProject("ROMSkeleton.cpp"));
        lst.addElement(h.resolveFileForProject("OopMapsSkeleton.cpp"));
		lst.addElement("..\\generated\\ROMImage.cpp");

    }

	void writeHeaderFileRules() throws Exception
	{
		System.out.println("writeHeaderFileRules");
		writeIncls();
	}

	void writeIncls() throws Exception 
	{
		((NMakeLoopgenProjectARM)iProj).getDatabase().setResolveVpath(true);
        ((NMakeLoopgenProjectARM)iProj).getDatabase().put();
    }

	public void writeProjectBlock() 
	{
		put("CPPFLAGS = ");
        putBaseCppFlags();
        putln("/FR /FD /c  -DARM=1 -DCROSS_GENERATOR=1 \\");

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
		puttabln("@echo generated `pwd`/$@");
		putln();
        
    }

    void putIncludePaths() 
    {
        Vector vpaths = ((NMakeLoopgenProjectARM)iProj).getSourceHandler().getVpaths();
        put("/I \"$(GEN_DIR)\" ");
        put("/I \"$(WINDOWS_PLATFORM_SDK)\\Include\" ");
        for (int i=0; i<vpaths.size(); i++) 
        {
            String path = (String)vpaths.elementAt(i);
            put("/I \"" + iProj.getRelativeSourceFile(path) + "\" ");
        }
    }

}

