package makedep;

import java.io.*;
import java.util.*;

abstract class SymbianIDEProject extends IDEProject 
{
    protected int type;
    
    public final static int LOOPGEN = 1;
    public final static int ROMGEN  = 2;
    public final static int TARGET  = 3;
    public final static int LOOP    = 4;
	public final static int ROM     = 5;
	public final static int MIDP = 6;
	public final static int JAVAAPI = 7;
    protected VMSourceHandler iSrcHandler;
    
    public SymbianIDEProject(IDETool tool, String name,int aType) 
    {
        super(tool,name);
		this.type = aType;
    }

    SymbianIDETool ideTool() 
    {
        return (SymbianIDETool)tool();
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

        switch (this.type) {
        case ROMGEN:
        case LOOPGEN:
            // the generators must have INTERPRETER and ROM generators
            env.put("ENABLE_INTERPRETER_GENERATOR",     "true");
            env.put("ENABLE_INTERPRETER_GENERATOR__BY", "idetool");
            env.put("ENABLE_ROM_GENERATOR",             "true");
            env.put("ENABLE_ROM_GENERATOR__BY",         "idetool");
        }

        String filename = getOutputDir() + File.separator + "jvmconfig.h";
        String fullpath = tool().getOutputFileFullPath(filename);
        Vector extra = new Vector();
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

	public void addExtraSources(Vector lst) 
	{
		VMSourceHandler h = this.iSrcHandler;
		lst.addElement("..\\generated\\NativesTable.cpp");
            
        switch (this.type) {
        case TARGET:
		        lst.addElement("..\\generated\\KvmNatives.cpp");
                lst.addElement("..\\generated\\ROMImage_00.cpp");
                lst.addElement("..\\generated\\ROMImage_01.cpp");
                lst.addElement("..\\generated\\ROMImage_02.cpp");
                lst.addElement("..\\generated\\ROMImage_03.cpp");
                lst.addElement("..\\generated\\ROMImage_04.cpp");
                lst.addElement("..\\generated\\ROMImage_05.cpp");
                lst.addElement("..\\generated\\ROMImage_06.cpp");
                lst.addElement("..\\generated\\ROMImage_07.cpp");
                lst.addElement("..\\generated\\ROMImage_08.cpp");
                lst.addElement("..\\generated\\ROMImage_09.cpp");
                lst.addElement("..\\generated\\ROMImage_10.cpp");
                lst.addElement("..\\generated\\ROMImage_11.cpp");
                lst.addElement("..\\generated\\ROMImage_12.cpp");
				lst.addElement("..\\generated\\properties_static_data.c");
				lst.addElement("..\\generated\\midp_constants_data.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\core\\native_thread\\stubs\\native\\midpNativethread_md.c");
				lst.addElement("..\\..\\..\\..\\restricted_crypto\\src\\restricted_crypto\\reference\\native\\nativecrypto.cpp");
				lst.addElement("..\\..\\..\\..\\restricted_crypto\\src\\restricted_crypto\\reference\\native\\bnlib.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\ams_base_cldc\\reference\\native\\SuiteLoader.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\security\\crypto\\reference\\native\\MD5.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\security\\crypto\\reference\\native\\MD2.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\security\\crypto\\reference\\native\\SHA.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\platform_request\\symbian\\native\\midp_platform_request_md.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\security\\file_digest\\reference\\native\\midpDataHash.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\annunciator\\symbian\\native\\anc_symbian_pauseresume.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\suitestore\\internal_api\\reference\\native\\suitestore_otanotifier_db.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\suitestore\\common_api\\reference\\native\\suitestore_listeners.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\suitestore\\task_manager_api\\reference\\native\\suitestore_task_manager.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\suitestore\\internal_api\\reference\\native\\suitestore_locks.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\ams_base\\reference\\native\\listeners_intern.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\core\\string\\reference\\native\\midpString.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\suitestore\\recordstore_api\\reference\\native\\suitestore_rms.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\rms\\record_store\\file_based\\native\\midp_file_cache.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\rms\\record_store\\file_based\\native\\rms.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\rms\\rms_base\\reference\\native\\recordStoreUtil.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\core\\suspend_resume\\sr_vm\\mastermode_default\\suspend_resume_vm.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\core\\suspend_resume\\sr_port\\default\\suspend_resume_port.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\core\\suspend_resume\\sr_main\\default\\native\\suspend_resume.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\rms\\rms_api\\reference\\native\\recordStoreFile.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\ams_base\\reference\\native\\midpInit.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\suitestore\\common_api\\reference\\native\\suitestore_common.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\suitestore\\internal_api\\reference\\native\\suitestore_intern.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\suitestore\\internal_api\\reference\\native\\midletsuitestorage_kni.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\suitestore\\internal_api\\reference\\native\\midletsuiteimpl_kni.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\suitestore\\installer_api\\reference\\native\\suitestore_installer.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\app_image_gen\\reference\\native\\ImageGenerator.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\ams_base_cldc\\reference\\native\\midpMIDletProxyList.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\ams_base_cldc\\reference\\native\\midpMidletSuiteVerifier.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\ams_base_cldc\\reference\\native\\midlet.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\ams_base_cldc\\reference\\native\\midpcommandState.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\ams_base_cldc\\reference\\native\\exitInternal.c");
				lst.addElement("..\\..\\..\\..\\jsr75\\src\\cldc_application\\native\\share\\jsr75_initializer_kni.c");
				lst.addElement("..\\..\\..\\..\\jsr75\\src\\cldc_application\\native\\share\\fcCleanup.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\push\\push_api\\javapush\\native\\connection_registry.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\push\\push_api\\javapush\\native\\symbian\\pushregistry.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\security\\crypto\\reference\\native\\messagedigest.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\suitestore\\internal_api\\reference\\native\\suitestore_otanotifier_kni.c");
                lst.addElement("..\\..\\..\\..\\midp\\src\\media\\reference\\classes\\javax\\microedition\\media\\Symbian\\native\\myplayer.cpp");
                lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\example\\jams\\native\\Symbian\\NativeCode.cpp");
                lst.addElement("..\\..\\..\\..\\midp\\src\\protocol\\gcf\\reference\\native\\gcf_export.c");
                lst.addElement("..\\..\\..\\..\\midp\\src\\protocol\\socket\\reference\\native\\socketProtocol.c");
                lst.addElement("..\\..\\..\\..\\midp\\src\\configuration\\properties\\reference\\native\\properties_kni.c");
                lst.addElement("..\\..\\..\\..\\midp\\src\\core\\resource_manager\\reference\\native\\midpResourceLimit.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\core\\resource_handler\\file_based\\native\\resource_handler_kni.c");
                lst.addElement("..\\..\\..\\..\\midp\\src\\core\\kni_util\\reference\\native\\midpException.c");
                lst.addElement("..\\..\\..\\..\\midp\\src\\core\\kni_util\\reference\\native\\kni_globals.c");
                lst.addElement("..\\..\\..\\..\\midp\\src\\core\\kni_util\\reference\\native\\midpUtilKni.c");
                lst.addElement("..\\..\\..\\..\\midp\\src\\core\\libc_ext\\symbian\\native\\midp_libc_ext.cpp");
                lst.addElement("..\\..\\..\\..\\midp\\src\\core\\vm_services\\cldc_vm\\native\\midp_thread.c");
                lst.addElement("..\\..\\..\\..\\midp\\src\\core\\vm_services\\cldc_vm\\native\\midpServices.c");
                lst.addElement("..\\..\\..\\..\\midp\\src\\core\\log_base\\reference\\native\\midp_logging.c");
                lst.addElement("..\\..\\..\\..\\midp\\src\\core\\log_base\\reference\\native\\midp_logging_kni.c");
                lst.addElement("..\\..\\..\\..\\midp\\src\\configuration\\properties_port\\static\\native\\midp_properties_static.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\ams_base_cldc\\reference\\native\\midpMidletSuiteUtils.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\core\\timezone\\symbian\\native\\midp_timezone_md.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\ams_base\\reference\\native\\midpInflate.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\core\\jarutil\\reference\\native\\JarReader.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\core\\jarutil\\reference\\native\\midpJar.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\core\\jarutil\\reference\\native\\Jar.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\core\\storage\\reference\\native\\midpStorage.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics_api\\gxapi_native\\native\\symbian\\gxapi_graphics_kni.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics_api\\gxapi_native\\native\\gxapi_anchor.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics\\gx_putpixel\\native\\gxj_graphics.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics\\gx_putpixel\\native\\gxj_image.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics\\gx_putpixel\\native\\gxj_putpixel.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics_api\\gxapi_native\\native\\gxapi_font_kni.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\push\\push_server\\symbian\\native\\push_server.cpp");
                lst.addElement("..\\..\\..\\..\\midp\\src\\push\\push_timer\\symbian\\native\\midp_alarm_timer_md.c");
                lst.addElement("..\\..\\..\\..\\midp\\src\\lowlevelui\\image\\img_putpixel\\native\\imgj_imagedata_kni.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\lowlevelui\\image\\img_putpixel\\native\\imgj_imagedatafactory_kni.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\lowlevelui\\putpixel_port\\symbian\\native\\gxjport_symbian_text.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\lowlevelui\\image_decode\\reference\\native\\imgdcd_image.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\lowlevelui\\image_decode\\reference\\native\\imgdcd_image_util.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\lowlevelui\\image_decode\\reference\\native\\imgdcd_image_decode.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\lowlevelui\\graphics\\gx_putpixel\\native\\gxj_graphics_asm.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\lcdlf\\lfjava\\native\\lfj_native_input_mode.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\lcdlf\\lfjava\\native\\symbian\\lfj_cskin.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\lcdlf\\lfjava\\native\\lfj_export.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\protocol\\file\\reference\\native\\storageFile.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\protocol\\file\\reference\\native\\RandomAccessStream.c");
				lst.addElement("..\\generated\\LocalizedStringsBase.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\lcdui\\reference\\native\\lcdui_input.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\lcdui\\reference\\native\\lcdui_game.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\lcdui\\reference\\native\\lcdui_indicators.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\lcdui\\reference\\native\\lcdui_vibrate.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\lcdui\\reference\\native\\symbian\\lcdui_display.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\lcdui\\reference\\native\\symbian\\lcdui_audio.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\nim_port\\symbian\\nim_impl.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\annunciator\\symbian\\native\\anc_symbian_audio.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\annunciator\\symbian\\native\\anc_symbian_indicator.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\annunciator\\symbian\\native\\anc_symbian_vibrate.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\ams\\ams_base\\reference\\native\\midp_foreground_id.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\lfjport\\symbian\\native\\lfjport_symbian_export.cpp");
				lst.addElement("..\\generated\\lfj_image_rom.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\keymap\\default\\native\\keymap_input_md.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\events\\eventqueue\\reference\\native\\midpEvents.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\events\\eventqueue\\reference\\native\\midpEventUtil.c");
				lst.addElement("..\\..\\..\\..\\midp\\src\\events\\eventqueue_port\\symbian\\native\\midp_eventqueue_md.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\lcdlf\\lfplatform\\native\\symbian\\lfp_command.cpp");
				lst.addElement("..\\..\\..\\..\\midp\\src\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\symbian\\native\\TextBoxLFImpl.cpp");
				// jsr75 code
				lst.addElement("..\\..\\..\\..\\jsr75\\src\\cldc_application\\native\\share\\midpFileConnection.cpp");
				lst.addElement("..\\..\\..\\..\\jsr75\\src\\cldc_application\\native\\share\\fcNotifyIsolates.c");
				lst.addElement("..\\..\\..\\..\\jsr75\\src\\cldc_application\\native\\symbian\\midpdirsize.cpp");
				lst.addElement("..\\..\\..\\..\\jsr75\\src\\cldc_application\\native\\symbian\\midpNativeMounts.cpp");
				// jsr211 code
				lst.addElement("..\\..\\..\\..\\abstractions\\src\\cldc_application\\native\\share\\kni_utils.c");
				lst.addElement("..\\..\\..\\..\\abstractions\\src\\cldc_application\\native\\symbian\\jsrop_suitestore.c");
				lst.addElement("..\\..\\..\\..\\javacall\\implementation\\share\\utils\\javautil_unicode.c");
				lst.addElement("..\\..\\..\\..\\javacall\\implementation\\symbian\\jsr211_chapi\\javautil_string.cpp");
				lst.addElement("..\\..\\..\\..\\javacall\\implementation\\symbian\\jsr211_chapi\\javautil_storage.c");
				lst.addElement("..\\..\\..\\..\\javacall\\implementation\\symbian\\jsr211_chapi\\chapi.c");
				lst.addElement("..\\..\\..\\..\\javacall\\implementation\\win32_emul\\common\\memory.c");
				lst.addElement("..\\..\\..\\..\\jsr211\\src\\cldc_application\\core\\native\\symbian\\kni_invoc_store.c");
				lst.addElement("..\\..\\..\\..\\jsr211\\src\\share\\core\\native\\symbian\\kni_reg_store.c");
				lst.addElement("..\\..\\..\\..\\jsr211\\src\\share\\core\\native\\symbian\\jsr211_result.cpp");
				lst.addElement("..\\..\\..\\..\\jsr211\\src\\share\\core\\native\\symbian\\jsr211_registry_impl.cpp");
				lst.addElement("..\\..\\..\\..\\jsr211\\src\\share\\core\\native\\symbian\\jsr211_deploy.c");
            break;
        case LOOPGEN:
            lst.addElement(h.resolveFileForProject("InterpreterSkeleton.cpp"));
            lst.addElement(h.resolveFileForProject("ROMSkeleton.cpp"));
            lst.addElement(h.resolveFileForProject("OopMapsSkeleton.cpp"));
            break;
        case ROMGEN:
            lst.addElement(h.resolveFileForProject("ROMSkeleton.cpp"));
            break;
        }
    }
}
