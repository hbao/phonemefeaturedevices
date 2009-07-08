package makedep;

import java.io.*;
import java.util.*;
import util.*;

abstract class SymbianIDETool extends IDETool
{
	protected Vector iOsVersions;
    public static String DEBUG_LOOPGEN = "Debug loop generator";
    public static String DEBUG_ROMGEN  = "Debug ROM generator";

	abstract protected void createProjects(BldInf aInf)  throws Exception;
	
	public void execute(String args[]) throws Exception 
    {
        parseArguments(args);
        initConfigurator(); // in IDETool
        setUserSettings(queryUserSettings());
        iOsVersions = new Vector();
        iOsVersions.addElement("_s60v3fp0");
        iOsVersions.addElement("_s60v2fp3");
        iOsVersions.addElement("_s60v2fp2");
        iOsVersions.addElement("_uiqv3fp0");
        BldInf inf = new BldInf(this, iOsVersions);
		createProjects(inf);
		addExports(inf);
		inf.write();
    }
	
	protected void addExports(BldInf aInf) throws Exception
	{
		aInf.addExport("..\\cldc_vm_lib\\jvmconfig.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\protocol\\gcf\\include\\gcf_export.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\native_thread\\stubs\\include\\midpNativeThreadImpl.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\kni_util\\include\\commonKNIMacros.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\kni_util\\include\\midperror.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\push\\push_server\\include\\push_server_export.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\highlevelui\\annunciator\\include\\anc_indicators.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\highlevelui\\annunciator\\include\\anc_vibrate.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\highlevelui\\annunciator\\include\\anc_audio.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\configuration\\properties_port\\include\\midp_properties_port.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\global_status\\include\\midp_global_status.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\ams\\suitestore\\common_api\\include\\suitestore_common.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\log_base\\include\\midp_logging.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\resource_manager\\include\\midpResourceLimit.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\vm_services\\include\\midp_thread.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\libc_ext\\include\\midp_libc_ext.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\kni_util\\include\\kni_globals.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\storage\\include\\midpStorage.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\vm_services\\include\\midpServices.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\string\\include\\midpString.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\memory\\include\\midpMalloc.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\ams\\ams_base\\include\\midpMidletSuiteUtils.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\native_thread\\include\\midpNativeThread.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\kni_util\\include\\midpUtilKni.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\timezone\\include\\midpTimeZone.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\ams\\ams_base\\include\\midpInflateint.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\ams\\ams_base\\include\\midpInflatetables.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics_api\\include\\gxapi_graphics.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics\\include\\gx_font.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics\\include\\gx_image.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics\\include\\gx_graphics.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics_util\\include\\gxutl_graphics.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\events\\eventqueue\\include\\midpEventUtil.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\ams\\ams_base\\include\\midp_foreground_id.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\events\\eventqueue\\include\\midpevents.h");
        aInf.addExport("..\\..\\..\\..\\midp\\src\\events\\eventqueue_port\\include\\midpport_eventqueue.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\highlevelui\\nim_port\\include\\nim.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\highlevelui\\lcdlf\\lfjava\\include\\lfj_image_rom.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\highlevelui\\lcdlf\\include\\lcdlf_export.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\highlevelui\\lfjport\\include\\lfjport_export.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\highlevelui\\keymap\\include\\keymap_input.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\image_api\\include\\imgapi_image.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\push\\push_server\\include\\push_server_resource_mgmt.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics_api\\include\\gxapi_constants.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\image\\include\\img_errorcodes.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\image_decode\\include\\imgdcd_image_util.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\image_decode\\reference\\native\\imgdcd_intern_image_decode.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics_api\\gxapi_native\\native\\gxapi_intern_graphics.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics\\gx_putpixel\\native\\gxj_intern_graphics.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics\\gx_putpixel\\native\\gxj_intern_image.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics\\gx_putpixel\\native\\gxj_intern_putpixel.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics\\gx_putpixel\\native\\gxj_intern_font_bitmap.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\putpixel_port\\include\\gxjport_text.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\jarutil\\include\\jar.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\core\\jarutil\\include\\midpjar.h");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\ams\\ams_base_cldc\\include\\MIDPMIDLETPROXYLIST.H");
		aInf.addExport("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics\\gx_putpixel\\include\\GXJ_PUTPIXEL.H");
		// jsr75
		aInf.addExport("..\\..\\..\\..\\jsr75\\src\\cldc_application\\native\\share\\inc\\midpNativeMounts.h");
		aInf.addExport("..\\..\\..\\..\\jsr75\\src\\cldc_application\\native\\share\\inc\\midpDirSize.h");
		aInf.addExport("..\\..\\..\\..\\jsr75\\src\\cldc_application\\native\\share\\fcNotifyIsolates.h");		
        
	}
	
	Properties queryUserSettings() throws Exception 
    {
        Properties props = new Properties();

        // Put default settings
        Vector v = getBuildOptions();
        for (int i=0; i<v.size(); i++) {
            String name = (String)v.elementAt(i);
            if (name.equals("ROMIZING")) {
                props.put(v.elementAt(i), "true");
            } else {
                props.put(v.elementAt(i), "default");
            }
        }
        //props.put("ENABLE_JAVA_DEBUGGER", "true");
        //props.put("ENABLE_ROM_JAVA_DEBUGGER", "true");

		props.put("ENABLE_ISOLATES", "true");
        props.put(DEBUG_LOOPGEN, "true");
        props.put(DEBUG_ROMGEN,  "false");
        props.put("ENABLE_INLINEASM_INTERPRETER", "true");
        props.put("ENABLE_SEGMENTED_ROM_TEXT_BLOCK","true");
		
		String workspace = getWorkSpaceArg();
        String preverify = workspace + "/build/share/bin/win32_i386/" +
                           "preverify.exe";
        preverify = preverify.replace('/', File.separatorChar);
        String java14 = "";
        try
        {
            Hashtable env = Util.getenv();
            java14 =  (String)env.get("JAVA_1_4_2_HOME");
        }
        catch(Throwable err)
        {}
        props.put("javac.exe",     java14 + "\\bin\\javac.exe");
        props.put("java.exe",      java14 + "\\bin\\java.exe");
        props.put("jar.exe",       java14 + "\\bin\\jar.exe");
        props.put("ml.exe",        "");
        props.put("preverify.exe", preverify);

        String prefsFile = getOutputFileFullPath("prefs.props");
        try 
        {
            // Load from saved values
            FileInputStream in = new FileInputStream(prefsFile);
            props.load(in);
            in.close();
        } 
        catch (IOException e) 
        {
            // ignore it
        }

        //Win32IDESettingsDialog dialog = new Win32IDESettingsDialog();


        // The following call blocks until the user has finished with
        // dialog.
        
        //dialog.getUserInput(props, getBuildOptions(),
        //                    getGeneratorOptions(),
        //                    getPaths());

        try 
        {
            FileOutputStream out = new FileOutputStream(prefsFile);
            props.store(out, "");
            out.close();
        } 
        catch (IOException e) 
        {
            // ignore it
        }

        return props;
    }

	Vector getBuildOptions() 
    {
        Vector v = new Vector();
        v.addElement("ROMIZING");
        Vector enableFlags = configurator.getFlagNames();
        for (int i=0; i<enableFlags.size(); i++) 
		{
            v.addElement((String)enableFlags.elementAt(i));
        }
        return v;
    }

}

