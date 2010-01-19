package makedep;

class NMakeJavaAPIProject extends SymbianIDEProject
{
    private NMakefile iMakefile;
    public NMakeJavaAPIProject(IDETool tool,String name)
    {
        super(tool,name,JAVAAPI);
        tool().makeDirExist(getOutputDir());
        tool().makeDirExist("generated");

        iMakefile = new SymbianJavaApi(tool,name,this);
    }
    public void write() throws Exception
    {
    iMakefile.write();
    }
}
