package makedep;

class NMakeMIDPProjectWINSCW extends SymbianIDEProject
{
    private NMakefile iMakefile;
    public NMakeMIDPProjectWINSCW(IDETool tool,String name)
    {
        super(tool,name,MIDP);
        System.out.println("NMakeMIDPProjectWINSCW");
        iMakefile = new SymbianMIDPWINSCW(tool,name,this);
    }
    public void write() throws Exception
    {
        iMakefile.write();
    }
}
