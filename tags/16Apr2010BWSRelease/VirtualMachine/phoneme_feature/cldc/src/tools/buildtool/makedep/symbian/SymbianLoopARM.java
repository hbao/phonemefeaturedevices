package makedep;

class SymbianLoopARM extends SymbianLoop
{
	public SymbianLoopARM(IDETool tool,String name,IDEProject proj)
    {
        super(tool,name,proj);
    }
    
    void writeRules() throws Exception
    {
		putln("LOOPGEN = ..\\loopgen\\loopgen.exe");
        putln("CLDC_DIR = ..\\cldc_vm_lib");
        putln("ROMGEN_DIR = ..\\romgen");
        putln();
        putln("Dummy.cpp: $(LOOPGEN)");
        puttabln("@echo Creating Interpreter Loops");
		puttabln("$(LOOPGEN) -generate +GenerateGNUCode +GenerateDebugAssembly -outputdir $(CLDC_DIR)\\Debug");
		puttabln("$(LOOPGEN) +GenerateOopMaps -outputdir $(CLDC_DIR)\\Debug");
		puttabln("$(LOOPGEN) -generateoptimized +GenerateGNUCode +GenerateAssemblyCode -outputdir $(CLDC_DIR)\\Release");
		puttabln("$(LOOPGEN) +GenerateOopMaps -outputdir $(CLDC_DIR)\\Release");
		puttabln("$(LOOPGEN) -generateoptimized +GenerateGNUCode +GenerateAssemblyCode -outputdir $(CLDC_DIR)\\Product");
		puttabln("$(LOOPGEN) +GenerateOopMaps -outputdir $(CLDC_DIR)\\Product");
		putln();
	}
	
}
