package makedep;

class BldInfProjectType
{
    public IDEProject iProj;
    public String iType;
    public String iExt;
    
    
    BldInfProjectType(IDEProject aProj,String aType,String aExt)
    {
        iProj = aProj;
        iType = aType;
        iExt = aExt;
    }
}

