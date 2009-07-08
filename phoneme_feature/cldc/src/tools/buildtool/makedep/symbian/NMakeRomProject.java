package makedep;

class NMakeRomProject extends SymbianIDEProject
{
    private NMakefile iMakefile;
    public NMakeRomProject(IDETool tool,String name)
    {
        super(tool,name,ROM);
        iMakefile = new SymbianRom(tool,name,this);
    }
    public void write() throws Exception
    {
        iMakefile.write();
    }
}

