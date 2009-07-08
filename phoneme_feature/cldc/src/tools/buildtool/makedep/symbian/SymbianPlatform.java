package makedep;

class SymbianPlatform extends WinGammaPlatform
{
	private String iPath;
	private String iWorkspace;
	public SymbianPlatform(String aPath,String aWorkspace)
	{
		iPath = aPath;
		iWorkspace = aWorkspace;
	}
	
	public String translateFileName(String name)
	{
		String newName = "..\\..\\.." + name.substring(iWorkspace.length());
		return newName;
	}
}
