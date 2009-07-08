package makedep;

import java.io.*;
import java.util.*;

abstract class SymbianIDEProjectARM extends SymbianIDEProject 
{
    public SymbianIDEProjectARM(IDETool tool, String name,int aType) 
    {
        super(tool,name,aType);
    }

	protected void writeJvmConfig() throws Exception 
    {
        /*
         * If user has chosen non-default value for the ENABLE_XXX flags,
         * pass these values to tool.configurator, which creates jvmconfig.h.
         */

        Hashtable env = new Hashtable();
        Properties userSettings = tool().getUserSettings();
        
        for (Enumeration keys = userSettings.keys(); keys.hasMoreElements();) {
            String key = (String)keys.nextElement();
            if (key.startsWith("ENABLE")) {
                String value = (String)userSettings.get(key);
                if (key.equals("ENABLE_MONET") && this.type != TARGET) {
                    // ENABLE_MONET must be set to false for loopgen and romgen
                    value = "false";
                }
                if (value.equals("true") || value.equals("false")) {
                   env.put(key, value);
                   env.put(key + "__BY", "idetool");
                }
            }
        }

        Vector extra = new Vector();

		System.out.println("write jvmconfig for " + new Integer(this.type).toString());
        switch (this.type) {
        case ROMGEN:
            env.put("ENABLE_COMPILER", "false");	// this causes a panic on ARM
            env.put("ENABLE_COMPILER__BY", "SymbianProjectMakeARM");
            
			env.put("ENABLE_INTERPRETER_GENERATOR",     "true");
            env.put("ENABLE_INTERPRETER_GENERATOR__BY", "idetool");
            env.put("ENABLE_ROM_GENERATOR",             "true");
            env.put("ENABLE_ROM_GENERATOR__BY",         "idetool");
			extra.addElement("MSW_FIRST_FOR_DOUBLE=0");
		   	extra.addElement("TARGET_MSW_FIRST_FOR_DOUBLE=1");
		    extra.addElement("TARGET_MSW_FIRST_FOR_LONG=0");
			break;
        case LOOPGEN:
            env.put("ENABLE_COMPILER", "false");	// this causes a panic on ARM
            env.put("ENABLE_COMPILER__BY", "SymbianProjectMakeARM");
            
			// the generators must have INTERPRETER and ROM generators
            env.put("ENABLE_INTERPRETER_GENERATOR",     "true");
            env.put("ENABLE_INTERPRETER_GENERATOR__BY", "idetool");
            env.put("ENABLE_ROM_GENERATOR",             "true");
            env.put("ENABLE_ROM_GENERATOR__BY",         "idetool");
			extra.addElement("MSW_FIRST_FOR_DOUBLE=1");
            break;
        case TARGET:
			env.put("ENABLE_COMPILER", "false");	// this causes a panic on ARM
            env.put("ENABLE_COMPILER__BY", "SymbianProjectMakeARM");
            env.put("ENABLE_INTERPRETER_GENERATOR",     "false");
            env.put("ENABLE_INTERPRETER_GENERATOR__BY", "idetool");
			//ENABLE_MONET_DEBUG_DUMP
			//ENABLE_PERFORMANCE_COUNTERS
			//ENABLE_SYSTEM_ROM_OVERRIDE
			env.put("ENABLE_ROM_DEBUG_SYMBOLS",     "false");
			env.put("ENABLE_ROM_DEBUG_SYMBOLS__BY", "idetool");
			env.put("ENABLE_ROM_GENERATOR",     "false");
			env.put("ENABLE_ROM_GENERATOR__BY", "idetool");
			extra.addElement("MSW_FIRST_FOR_DOUBLE=1");
			break;
        }

        String filename = getOutputDir() + File.separator + "jvmconfig.h";
        String fullpath = tool().getOutputFileFullPath(filename);
        System.out.println("jvmfile " + fullpath);
        extra.addElement("USE_UNICODE_FOR_FILENAMES=1");
		extra.addElement("ENABLE_FILE_SYSTEM=1");
        // Append product-specific definitions
        {
            String productName = tool().configurator.getProductName();
            if (productName != null) {
                extra.addElement("JVM_NAME=\"" + productName + "\"");
            }
            String releaseVersion = tool().configurator.getReleaseVersion();
            if (releaseVersion != null) {
                extra.addElement("JVM_RELEASE_VERSION=\"" + releaseVersion + "\"");
            }
        }

        ideTool().configurator.write(fullpath, env, extra);
	    }
	}
