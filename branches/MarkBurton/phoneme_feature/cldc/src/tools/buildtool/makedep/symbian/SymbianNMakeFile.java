package makedep;

class SymbianNMakeFile extends NMakefile
{
    public SymbianNMakeFile(IDETool tool,String name)
    {
    super(tool,name + "\\" + name + ".mak");
    }
    public void writeContents() throws Exception
    {
    }
}
