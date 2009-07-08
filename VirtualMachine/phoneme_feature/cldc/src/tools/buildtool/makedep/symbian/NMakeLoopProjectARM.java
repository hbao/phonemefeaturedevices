package makedep;

class NMakeLoopProjectARM extends SymbianIDEProjectARM
{
    private NMakefile iMakefile;
    public NMakeLoopProjectARM(IDETool tool,String name)
    {
        super(tool,name,LOOP);
        iMakefile = new SymbianLoopARM(tool,name,this);
        tool().makeDirExist("cldc_vm_lib");
        tool().makeDirExist("cldc_vm_lib\\debug");
        tool().makeDirExist("cldc_vm_lib\\release");
        tool().makeDirExist("cldc_vm_lib\\product");
        tool().makeDirExist("romgen");
        tool().makeDirExist("romgen\\release");
    }

    public void write() throws Exception
    {
        iMakefile.write();
    }
}
