package makedep;


class SymbianRom extends NMakefile
{
    IDEProject iProj;
    public SymbianRom(IDETool tool,String name,IDEProject proj)
    {
        super(tool,name + "\\" + name + ".mak");
        iProj = proj;
    }
    
    public void writeContents() throws Exception
    {
        writeHeader();
        writeSymbianRules();
        writeRules();
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
        putln("all : ROMImage.cpp");
        putln();
        putln("MAKMAKE FINAL FREEZE LIB CLEAN CLEANALL CLEANLIB RESOURCE RELEASABLES SAVESPACE : ");
        putln();
        putln("BLD : all");
        putln();
    }
    
    void writeRules() throws Exception
    {
        putln("ROMGEN = ..\\romgen\\romgen.exe");
        putln("CLASSES_ZIP = ..\\MIDP\\classes.zip");
        putln();
        putln("ROMImage.cpp: $(ROMGEN) $(CLASSES_ZIP)");
        puttabln("$(ROMGEN) -cp $(CLASSES_ZIP) \\");
        puttabln("=HeapCapacity16M \\");
        puttabln("-romconfig \"" + tool().getWorkSpaceArg() + "\\src\\vm\\symbian_rom.cfg\" \\");
        puttabln("-romincludepath \"" + tool().getWorkSpaceArg() + "\\src\\vm\" \\");
        puttabln("+EnableAllROMOptimizations \\");
        //puttabln("+GenerateGNUCode \\");
        puttabln("-romize \\");
        puttabln("+GenerateROMStructs");
        puttabln("-@copy *.cpp ..\\generated");
        puttabln("-@copy *.h ..\\generated");
        puttabln("-@copy *.hpp ..\\generated");
        puttabln("-@copy ROMStructs.h $(EPOCROOT)\\Epoc32\\include");
        putln();
		putln();
        putln("clean:");
        puttabln("-@del *.cpp");
	    puttabln("-@del *.hpp");
		puttabln("-@del *.h");
	    puttabln("-@del *.txt");
	    putln();
    }
    
}