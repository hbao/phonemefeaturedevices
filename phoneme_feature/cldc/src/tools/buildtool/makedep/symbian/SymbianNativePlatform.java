package makedep;

import java.io.*;
import java.util.*;

public class SymbianNativePlatform extends Platform 
{
    private static String[] suffixes = { ".cpp", ".c" };

    public String[] outerSuffixes() 
    {
      return suffixes;
    }

    public String fileSeparator() 
    {
      return "\\";
    }

    public String objFileSuffix() 
    {
      return ".obj";
    }

    public String asmFileSuffix() 
    {
      return ".s";
    }

    public String dependentPrefix() 
    {
      return "";
    }

    public boolean includeGIInEachIncl() 
    {
      return false;
    }

    public boolean fileNameStringEquality(String s1, String s2) 
    {
      return s1.equalsIgnoreCase(s2);
    }


    
    public void addPerFileLine(Hashtable table,String fileName,String line) 
    {
      Vector v = (Vector) table.get(fileName);
      if (v != null) {
          v.add(line);
      } else {
          v = new Vector();
          v.add(line);
          table.put(fileName, v);
      }
    }
                         
    public boolean findString(Vector v, String s) 
    {
      for (Iterator iter = v.iterator(); iter.hasNext(); ) {
          if (((String) iter.next()).equals(s)) {
            return true;
          }
      }

      return false;
    }

}
