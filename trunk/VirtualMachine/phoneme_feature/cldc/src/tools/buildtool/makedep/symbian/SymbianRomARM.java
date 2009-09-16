package makedep;

class SymbianRomARM extends SymbianRom
{
    public SymbianRomARM(IDETool tool,String name,IDEProject proj)
    {
        super(tool,name,proj);
    }
    
    void writeRules() throws Exception
    {
        putln("ROMGEN = ..\\romgen\\romgen.exe");
        putln("CLASSES_ZIP = ..\\MIDP\\classes.zip");
        putln("FIXUP=\"..\\..\\..\\src\\tools\\buildtool\\makedep\\BlueWhale_cldcvm_fixup.py\"");
        putln();
        putln("ROMImage.cpp: $(ROMGEN) $(CLASSES_ZIP)");
        puttabln("$(ROMGEN) -cp $(CLASSES_ZIP) \\");
        puttabln("=HeapCapacity16M \\");
        puttabln("-romconfig \"" + tool().getWorkSpaceArg() + "\\src\\vm\\symbian_rom.cfg\" \\");
        puttabln("-romincludepath \"" + tool().getWorkSpaceArg() + "\\src\\vm\" \\");
        puttabln("-romincludepath \"" + tool().getWorkSpaceArg() + "\\..\\jsr75\\src\\cldc_application\\config\\share\" \\");
        puttabln("-romincludepath \"" + tool().getWorkSpaceArg() + "\\..\\jsr211\\src\\share\\config\" \\");
        puttabln("+EnableAllROMOptimizations \\");
        puttabln("-romize \\");
        puttabln("+GenerateROMStructs");
		puttabln("-@python $(FIXUP) ROMImage_12.cpp");
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

