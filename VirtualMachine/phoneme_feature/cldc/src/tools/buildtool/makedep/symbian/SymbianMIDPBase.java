package makedep;
import java.util.*;

abstract class SymbianMIDPBase extends NMakefile
{
    IDEProject iProj;
    public SymbianMIDPBase(IDETool tool,String name,IDEProject proj)
    {
        super(tool,name + "\\" + name + ".mak");
        iProj = proj;
    }
    
    public void writeContents() throws Exception
    {
        writeHeader();
        writeSymbianRules();
        writeRules();
    }
    
    void writeHeader() throws Exception
    {
        putln("# Copyright 2007-2008 BlueWhaleSystems");
        putln();
        putln();
        putln();
    }

    void writeSymbianRules() throws Exception
    {
        putln("MAKMAKE FINAL FREEZE LIB CLEANALL CLEANLIB RESOURCE RELEASABLES SAVESPACE : ");
        putln();
        putln("BLD : all");
        putln();
    }

	public SymbianMIDPBase(IDETool tool,String name)
	{
		super(tool,name);
	}

	abstract void addPlatformSpecificJava(Vector aSource);
	
	void writeJavaSource()  throws Exception
	{
		Vector jpp = new Vector();
		Vector jav = new Vector();
		Vector dir = new Vector();
		jpp.addElement("$(MIDP_SRC_DIR)\\rms\\rms_api\\reference\\classes\\com\\sun\\midp\\rms\\RecordStoreFile.jpp");
		jpp.addElement("$(MIDP_SRC_DIR)\\ams\\ams_util\\mvm\\classes\\com\\sun\\midp\\main\\IsolateUtil.jpp");
		jpp.addElement("$(MIDP_SRC_DIR)\\ams\\ams_jsr_interface\\reference\\classes\\com\\sun\\midp\\jsr\\JSRInitializer.jpp");
		jpp.addElement("$(MIDP_SRC_DIR)\\ams\\suitestore\\internal_api\\reference\\classes\\com\\sun\\midp\\midletsuite\\MIDletSuiteImpl.jpp");
		jpp.addElement("$(MIDP_SRC_DIR)\\security\\internal_api_protection\\reference\\classes\\com\\sun\\midp\\security\\SecurityInitializer.jpp");
		jpp.addElement("$(MIDP_SRC_DIR)\\protocol\\gcf\\reference\\classes\\com\\sun\\midp\\io\\ConnectionBaseAdapter.jpp");
		
		jpp.addElement("$(MIDP_SRC_DIR)\\protocol\\file\\reference\\classes\\com\\sun\\midp\\io\\j2me\\storage\\RandomAccessStream.jpp");
		jpp.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\SkinResources.jpp");
		jpp.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\symbian\\Display.jpp");
		jpp.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\GraphicsAccessImpl.jpp");
		jpp.addElement("$(MIDP_SRC_DIR)\\events\\eventqueue\\reference\\classes\\com\\sun\\midp\\events\\EventQueue.jpp");
		jpp.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\GraphicsAccess.jpp");
		
		// jsr 75 file system support
		jpp.addElement("$(JSR75_DIR)\\share\\classes\\com\\sun\\cdc\\io\\j2me\\file\\symbian\\DefaultFileHandler.jpp");
        
        // jsr 120 support
        jpp.addElement("$(JSR120_DIR)\\share\\core\\common\\classes\\javax\\wireless\\messaging\\MessageConnection.jpp");
			
		jav.addElement("$(GENERATED_DIR)\\rms\\rms_api\\reference\\classes\\com\\sun\\midp\\rms\\RecordStoreFile.java");
		jav.addElement("$(GENERATED_DIR)\\ams\\ams_util\\mvm\\classes\\com\\sun\\midp\\main\\IsolateUtil.java");
		jav.addElement("$(GENERATED_DIR)\\ams\\ams_jsr_interface\\reference\\classes\\com\\sun\\midp\\jsr\\JSRInitializer.java");
		jav.addElement("$(GENERATED_DIR)\\ams\\suitestore\\internal_api\\reference\\classes\\com\\sun\\midp\\midletsuite\\MIDletSuiteImpl.java");
		jav.addElement("$(GENERATED_DIR)\\security\\internal_api_protection\\reference\\classes\\com\\sun\\midp\\security\\SecurityInitializer.java");
		jav.addElement("$(GENERATED_DIR)\\protocol\\gcf\\reference\\classes\\com\\sun\\midp\\io\\ConnectionBaseAdapter.java");
        jav.addElement("$(GENERATED_DIR)\\protocol\\file\\reference\\classes\\com\\sun\\midp\\io\\j2me\\storage\\RandomAccessStream.java");
		jav.addElement("$(GENERATED_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\SkinResources.java");
		jav.addElement("$(GENERATED_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\symbian\\Display.java");
		jav.addElement("$(GENERATED_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\GraphicsAccessImpl.java");
		jav.addElement("$(GENERATED_DIR)\\events\\eventqueue\\reference\\classes\\com\\sun\\midp\\events\\EventQueue.java");
		jav.addElement("$(GENERATED_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\GraphicsAccess.java");
		
		// jsr 75 file system support
		jav.addElement("$(GENERATED_DIR)\\share\\classes\\com\\sun\\cdc\\io\\j2me\\file\\symbian\\DefaultFileHandler.java");
		
        // jsr 120 support
        jav.addElement("$(GENERATED_DIR)\\share\\core\\common\\classes\\javax\\wireless\\messaging\\MessageConnection.java");
        
		dir.addElement("$(GENERATED_DIR)\\rms\\rms_api\\reference\\classes\\com\\sun\\midp\\rms");
		dir.addElement("$(GENERATED_DIR)\\ams\\ams_util\\mvm\\classes\\com\\sun\\midp\\main");
		dir.addElement("$(GENERATED_DIR)\\ams\\ams_jsr_interface\\reference\\classes\\com\\sun\\midp\\jsr");
		dir.addElement("$(GENERATED_DIR)\\ams\\suitestore\\internal_api\\reference\\classes\\com\\sun\\midp\\midletsuite");
		dir.addElement("$(GENERATED_DIR)\\security\\internal_api_protection\\reference\\classes\\com\\sun\\midp\\security");
		dir.addElement("$(GENERATED_DIR)\\protocol\\gcf\\reference\\classes\\com\\sun\\midp\\io");
        dir.addElement("$(GENERATED_DIR)\\protocol\\file\\reference\\classes\\com\\sun\\midp\\io\\j2me\\storage");
		dir.addElement("$(GENERATED_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources");
		dir.addElement("$(GENERATED_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\symbian");
		dir.addElement("$(GENERATED_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui");
		dir.addElement("$(GENERATED_DIR)\\events\\eventqueue\\reference\\classes\\com\\sun\\midp\\events");
		dir.addElement("$(GENERATED_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui");
		
        // jsr 75 file system support
		dir.addElement("$(GENERATED_DIR)\\share\\classes\\com\\sun\\cdc\\io\\j2me\\file\\symbian");
		
        // jsr 120 support
        dir.addElement("$(GENERATED_DIR)\\share\\core\\common\\classes\\javax\\wireless\\messaging");
		
        for (int i=0; i<jpp.size(); i++) 
        {
            putln((String)jav.elementAt(i) + " : " + (String)jpp.elementAt(i));
            puttabln("@echo Preproccesing " + (String)jpp.elementAt(i));
			puttabln("$(JAVA6) -classpath ..\\buildtool Jpp " + (String)jpp.elementAt(i) + " -DENABLE_PUBLICKEYSTORE -DENABLE_CHAMELEON -DENABLE_JSR_75 -DENABLE_JSR_211 -d " + (String)dir.elementAt(i));
			putln();
		}
        putln();
        putln();
        
		Vector v = new Vector();
		// Sun AMS manager
		v.addElement("$(MIDP_SRC_DIR)\\ams\\appmanager_ui\\reference\\classes\\com\\sun\\midp\\appmanager\\RunningMIDletSuiteInfo.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\jams\\classes\\com\\sun\\midp\\appmanager\\ApplicationManager.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\jams\\mvm\\classes\\com\\sun\\midp\\appmanager\\MVMManager.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\on_device_debug_stubs\\reference\\classes\\com\\sun\\midp\\appmanager\\ODTControllerEventConsumer.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\on_device_debug_stubs\\reference\\classes\\com\\sun\\midp\\appmanager\\ODTControllerEventListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\appmanager_ui\\reference\\classes\\com\\sun\\midp\\appmanager\\AppManagerUI.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\appmanager_ui\\reference\\classes\\com\\sun\\midp\\appmanager\\DisplayError.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\appmanager_ui\\reference\\classes\\com\\sun\\midp\\appmanager\\MIDletSwitcher.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\appmanager_ui\\reference\\classes\\com\\sun\\midp\\appmanager\\FolderList.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\appmanager_ui\\reference\\classes\\com\\sun\\midp\\appmanager\\SplashScreen.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\appmanager_ui\\reference\\classes\\com\\sun\\midp\\appmanager\\AppInfo.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\appmanager_ui\\reference\\classes\\com\\sun\\midp\\appmanager\\AppSettings.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\appmanager_ui\\reference\\classes\\com\\sun\\midp\\appmanager\\MIDletSelector.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_folders\\reference\\classes\\com\\sun\\midp\\appmanager\\FolderManager.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_folders\\reference\\classes\\com\\sun\\midp\\appmanager\\Folder.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ota\\reference\\classes\\com\\sun\\midp\\installer\\Symbian\\GraphicalInstaller.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base\\reference\\classes\\com\\sun\\midp\\main\\TrustedMIDletIcon.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\mvm\\reference\\classes\\com\\sun\\midp\\main\\MVMDisplayController.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\mvm\\reference\\classes\\com\\sun\\midp\\main\\IndicatorManager.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\mvm\\reference\\classes\\com\\sun\\midp\\main\\AppIsolateMIDletSuiteLoader.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_jsr_interface\\reference\\classes\\com\\sun\\midp\\payment\\PAPICleanUp.java");
		
		// Sun RMS stuff
		v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_base\\classes\\com\\sun\\midp\\rms\\AbstractRecordStoreFile.java");
		v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_base\\classes\\com\\sun\\midp\\rms\\AbstractRecordStoreImpl.java");
        v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_base\\reference\\classes\\com\\sun\\midp\\rms\\RecordStoreUtil.java");
		v.addElement("$(GENERATED_DIR)\\rms\\rms_api\\reference\\classes\\com\\sun\\midp\\rms\\RecordStoreFile.java");
		v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_api\\reference\\classes\\com\\sun\\midp\\rms\\RecordStoreFactory.java");
		v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_api\\reference\\classes\\javax\\microedition\\rms\\RecordStore.java");
		v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_api\\reference\\classes\\javax\\microedition\\rms\\RecordEnumerationImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_exc\\reference\\classes\\javax\\microedition\\rms\\RecordStoreException.java");
        v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_exc\\reference\\classes\\javax\\microedition\\rms\\RecordStoreFullException.java");
        v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_exc\\reference\\classes\\javax\\microedition\\rms\\RecordStoreNotFoundException.java");
        v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_exc\\reference\\classes\\javax\\microedition\\rms\\RecordStoreNotOpenException.java");
        v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_exc\\reference\\classes\\javax\\microedition\\rms\\InvalidRecordIDException.java");
        v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_api\\reference\\classes\\javax\\microedition\\rms\\RecordListener.java");
        v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_api\\reference\\classes\\javax\\microedition\\rms\\RecordFilter.java");
        v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_api\\reference\\classes\\javax\\microedition\\rms\\RecordComparator.java");
        v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_api\\reference\\classes\\javax\\microedition\\rms\\RecordEnumeration.java");
        v.addElement("$(MIDP_SRC_DIR)\\rms\\rms_api\\reference\\classes\\com\\sun\\midp\\rms\\RecordStoreImpl.java");
        v.addElement("$(MIDP_SRC_DIR)\\rms\\record_index\\tree_index\\classes\\com\\sun\\midp\\rms\\RecordStoreIndex.java");
        // extra crypto
		v.addElement("$(RESTRICTED_CRYPTO_DIR)\\restricted_crypto\\reference\\classes\\com\\sun\\midp\\crypto\\RSA.java");
		v.addElement("$(RESTRICTED_CRYPTO_DIR)\\restricted_crypto\\reference\\classes\\com\\sun\\midp\\crypto\\RsaShaSig.java");
		v.addElement("$(RESTRICTED_CRYPTO_DIR)\\restricted_crypto\\reference\\classes\\com\\sun\\midp\\crypto\\RsaSig.java");
		// Sun AMS stuff
		v.addElement("$(GENERATED_DIR)\\ams\\ams_util\\mvm\\classes\\com\\sun\\midp\\main\\IsolateUtil.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\MIDletSuiteLoader.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\ForegroundEventProducer.java");
		v.addElement("$(GENERATED_DIR)\\ams\\ams_jsr_interface\\reference\\classes\\com\\sun\\midp\\jsr\\JSRInitializer.java");
		v.addElement("$(MIDP_SRC_DIR)\\automation\\reference\\classes\\com\\sun\\midp\\automation\\AutomationInitializer.java");
		v.addElement("$(MIDP_SRC_DIR)\\push\\push_api\\javapush\\classes\\com\\sun\\midp\\io\\j2me\\push\\PushRegistryInternal.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_jsr_interface\\reference\\classes\\com\\sun\\midp\\content\\CHManager.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_jsr_interface\\reference\\classes\\com\\sun\\midp\\wma\\WMACleanupMonitor.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\CldcMIDletSuiteLoader.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\suitestore\\internal_api\\reference\\classes\\com\\sun\\midp\\installer\\InternalMIDletSuiteImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base\\reference\\classes\\com\\sun\\midp\\main\\AbstractMIDletSuiteLoader.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\com\\sun\\midp\\midlet\\MIDletSuiteExceptionListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\com\\sun\\midp\\midlet\\MIDletSuite.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\CommandState.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\com\\sun\\midp\\midlet\\MIDletEventProducer.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\MIDletProxyList.java");
		v.addElement("$(MIDP_SRC_DIR)\\core\\suspend_resume\\sr_main\\default\\classes\\com\\sun\\midp\\suspend\\AbstractSubsystem.java");
		v.addElement("$(MIDP_SRC_DIR)\\core\\suspend_resume\\sr_main\\default\\classes\\com\\sun\\midp\\suspend\\SuspendSystem.java");
		v.addElement("$(MIDP_SRC_DIR)\\core\\suspend_resume\\sr_main\\default\\classes\\com\\sun\\midp\\suspend\\SuspendSystemListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\core\\suspend_resume\\sr_main\\default\\classes\\com\\sun\\midp\\suspend\\SuspendDependency.java");
		v.addElement("$(MIDP_SRC_DIR)\\core\\suspend_resume\\sr_main\\default\\classes\\com\\sun\\midp\\suspend\\Subsystem.java");
		v.addElement("$(MIDP_SRC_DIR)\\core\\suspend_resume\\sr_main\\default\\classes\\com\\sun\\midp\\suspend\\StateTransitionException.java");
		v.addElement("$(MIDP_SRC_DIR)\\core\\suspend_resume\\sr_main\\default\\classes\\com\\sun\\midp\\suspend\\NetworkSubsystem.java");
		v.addElement("$(MIDP_SRC_DIR)\\core\\suspend_resume\\sr_main\\default\\classes\\com\\sun\\midp\\suspend\\SuspendTimer.java");
		v.addElement("$(MIDP_SRC_DIR)\\core\\suspend_resume\\sr_main\\default\\classes\\com\\sun\\midp\\suspend\\SuspendResumeUI.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\MIDletControllerEventConsumer.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\MIDletControllerEventProducer.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\com\\sun\\midp\\midlet\\MIDletEventListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\LCDUIEnvironment.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\RuntimeInfo.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\MIDletProxy.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\MIDletProxyListListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\DisplayController.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\DisplayControllerListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\access_controller_cldc_port\\classes\\com\\sun\\j2me\\security\\AccessControlContext.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\installer\\reference\\classes\\com\\sun\\midp\\installer\\HttpInstaller.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\installer\\reference\\classes\\com\\sun\\midp\\installer\\FileInstaller.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\installer\\reference\\classes\\com\\sun\\midp\\installer\\Installer.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\installer\\reference\\classes\\com\\sun\\midp\\installer\\InstallState.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\installer\\reference\\classes\\com\\sun\\midp\\installer\\InstallListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\secure_random\\reference\\classes\\com\\sun\\midp\\crypto\\SecureRandom.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\secure_random\\reference\\classes\\com\\sun\\midp\\crypto\\PRand.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\publickeystore\\reference\\classes\\com\\sun\\midp\\publickeystore\\PublicKeyStore.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\pki\\reference\\classes\\com\\sun\\midp\\pki\\Utils.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\pki\\reference\\classes\\com\\sun\\midp\\pki\\SubjectAlternativeName.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\pki\\reference\\classes\\com\\sun\\midp\\pki\\CertStore.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\pki\\reference\\classes\\com\\sun\\midp\\pki\\X509Certificate.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\pki\\reference\\classes\\javax\\microedition\\pki\\Certificate.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\pki\\reference\\classes\\javax\\microedition\\pki\\CertificateException.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\publickeystore\\reference\\classes\\com\\sun\\midp\\publickeystore\\WebPublicKeyStore.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\publickeystore\\reference\\classes\\com\\sun\\midp\\publickeystore\\PublicKeyInfo.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_util\\mvm\\classes\\com\\sun\\midp\\main\\MIDletSuiteVerifier.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_util\\mvm\\classes\\com\\sun\\midp\\main\\AmsUtil.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\app_image_gen\\reference\\classes\\com\\sun\\midp\\main\\MIDletAppImageGenerator.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\verifier\\classes\\com\\sun\\midp\\installer\\Verifier.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\suitestore\\internal_api\\reference\\classes\\com\\sun\\midp\\midletsuite\\InstallInfo.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\suitestore\\internal_api\\reference\\classes\\com\\sun\\midp\\midletsuite\\SuiteSettings.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\permission_dialog\\reference\\classes\\com\\sun\\midp\\security\\SecurityHandler.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\suitestore\\internal_api\\reference\\classes\\com\\sun\\midp\\midletsuite\\MIDletSuiteStorage.java");
		v.addElement("$(GENERATED_DIR)\\ams\\suitestore\\internal_api\\reference\\classes\\com\\sun\\midp\\midletsuite\\MIDletSuiteImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\suitestore\\internal_api\\reference\\classes\\com\\sun\\midp\\midletsuite\\SuiteProperties.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\suitestore\\internal_api\\reference\\classes\\com\\sun\\midp\\midletsuite\\MIDletSuiteLockedException.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\suitestore\\internal_api\\reference\\classes\\com\\sun\\midp\\midletsuite\\MIDletSuiteCorruptedException.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\midlet_suite_info\\reference\\classes\\com\\sun\\midp\\midletsuite\\MIDletSuiteInfo.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\access_controller_cldc_port\\classes\\com\\sun\\j2me\\security\\AccessControlContextAdapter.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\publickeystore\\reference\\classes\\com\\sun\\midp\\publickeystore\\InputStorage.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\publickeystore\\reference\\classes\\com\\sun\\midp\\publickeystore\\Storage.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\RSAPublicKey.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\RSAPrivateKey.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\RSAKey.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\PublicKey.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\PrivateKey.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\Key.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\MessageDigest.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\MD2.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\MD5.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\SHA.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\Signature.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\SignatureException.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\NoSuchAlgorithmException.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\DigestException.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\GeneralSecurityException.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\Cipher.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\NoSuchPaddingException.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\InvalidKeyException.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\ShortBufferException.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\IllegalBlockSizeException.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\Util.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\CryptoParameter.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\InvalidAlgorithmParameterException.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\crypto\\reference\\classes\\com\\sun\\midp\\crypto\\BadPaddingException.java");
		v.addElement("$(GENERATED_DIR)\\security\\internal_api_protection\\reference\\classes\\com\\sun\\midp\\security\\SecurityInitializer.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\internal_api_protection\\reference\\classes\\com\\sun\\midp\\security\\SecurityInitializerImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\internal_api_protection\\reference\\classes\\com\\sun\\midp\\security\\SecurityToken.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\internal_api_protection\\reference\\classes\\com\\sun\\midp\\security\\ImplicitlyTrustedClass.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\com\\sun\\midp\\midlet\\MIDletStateHandler.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\com\\sun\\midp\\midlet\\MIDletLoader.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\com\\sun\\midp\\midlet\\MIDletPeer.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\com\\sun\\midp\\midlet\\MIDletStateListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\com\\sun\\midp\\midlet\\MIDletEventConsumer.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\com\\sun\\midp\\midlet\\MIDletTunnel.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\com\\sun\\midp\\midlet\\PlatformRequest.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\javax\\microedition\\midlet\\MIDlet.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\javax\\microedition\\midlet\\MIDletTunnelImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\MIDletSuiteUtils.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\CldcForegroundController.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\CldcMIDletLoader.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\CldcPlatformRequest.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\CldcAccessControlContext.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\CldcMIDletStateListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ota\\reference\\classes\\com\\sun\\midp\\installer\\OtaNotifier.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\mvm\\reference\\classes\\com\\sun\\midp\\main\\MIDletExecuteEventProducer.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\ExecuteMIDletEventListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_debug\\stubs\\classes\\com\\sun\\midp\\main\\DebugUtil.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\access_controller_cldc_port\\classes\\com\\sun\\j2me\\security\\AccessController.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\MIDletControllerEventListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_util\\mvm\\classes\\com\\sun\\midp\\main\\MIDletProxyUtils.java");
		v.addElement("$(MIDP_SRC_DIR)\\push\\push_api\\javapush\\classes\\com\\sun\\midp\\io\\j2me\\push\\ConnectionRegistry.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\midp_permissions\\reference\\classes\\com\\sun\\midp\\security\\Permissions.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\midp_permissions\\reference\\classes\\com\\sun\\midp\\security\\PermissionGroup.java");
		v.addElement("$(MIDP_SRC_DIR)\\push\\push_api\\javapush\\classes\\com\\sun\\midp\\io\\j2me\\push\\PushRegistryImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\verifier\\with_crypto\\classes\\com\\sun\\midp\\installer\\VerifierImpl.java");
		v.addElement("$(JSR75_DIR)\\cldc_application\\classes\\share\\com\\sun\\midp\\jsr075\\Initializer.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\publickeystore\\reference\\classes\\com\\sun\\midp\\publickeystore\\PublicKeyStoreBuilderBase.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\publickeystore\\reference\\classes\\com\\sun\\midp\\publickeystore\\OutputStorage.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\mvm\\reference\\classes\\com\\sun\\midp\\main\\IsolateMonitor.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\mvm\\reference\\classes\\com\\sun\\midp\\main\\StartMIDletMonitor.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\mvm\\reference\\classes\\com\\sun\\midp\\main\\IsolateMonitorListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\app_image_gen_base\\mvm\\classes\\com\\sun\\midp\\main\\MIDletAppImageGeneratorBase.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_base_cldc\\reference\\classes\\com\\sun\\midp\\main\\HeadlessAlert.java");
		v.addElement("$(MIDP_SRC_DIR)\\push\\push_api\\javapush\\classes\\com\\sun\\midp\\io\\j2me\\push\\ProtocolPush.java");
		
		
		// New bluewhaleplatform midlet
		v.addElement("$(MIDP_SRC_DIR)\\core\\javautil\\reference\\classes\\java\\lang\\IllegalStateException.java");
        v.addElement("$(MIDP_SRC_DIR)\\core\\javautil\\reference\\classes\\java\\util\\Timer.java");
        v.addElement("$(MIDP_SRC_DIR)\\core\\javautil\\reference\\classes\\java\\util\\TimerTask.java");
		v.addElement("$(MIDP_SRC_DIR)\\security\\access_controller_cldc_port\\classes\\com\\sun\\j2me\\security\\InterruptedSecurityException.java");
		v.addElement("$(MIDP_SRC_DIR)\\protocol\\socket\\reference\\classes\\com\\sun\\midp\\io\\j2me\\socket\\Protocol.java");
		v.addElement("$(MIDP_SRC_DIR)\\protocol\\https\\classes\\javax\\microedition\\io\\HttpsConnection.java");
		v.addElement("$(MIDP_SRC_DIR)\\protocol\\ssl\\classes\\javax\\microedition\\io\\SecurityInfo.java");
  		v.addElement("$(GENERATED_DIR)\\protocol\\gcf\\reference\\classes\\com\\sun\\midp\\io\\ConnectionBaseAdapter.java");
        v.addElement("$(GENERATED_DIR)\\Constants.java");
        v.addElement("$(GENERATED_DIR)\\LogChannels.java");
		v.addElement("$(GENERATED_DIR)\\AlertTypeConstants.java");
		v.addElement("$(GENERATED_DIR)\\SkinPropertiesIDs.java");
		v.addElement("$(GENERATED_DIR)\\PermissionsTable.java");
        v.addElement("$(GENERATED_DIR)\\RMSConfig.java");
		v.addElement("$(GENERATED_DIR)\\ResourceConstants.java");
		v.addElement("$(GENERATED_DIR)\\LocalizedStringsBase.java");
		v.addElement("$(GENERATED_DIR)\\protocol\\file\\reference\\classes\\com\\sun\\midp\\io\\j2me\\storage\\RandomAccessStream.java");
		v.addElement("$(GENERATED_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\SkinResources.java");
		v.addElement("$(GENERATED_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\symbian\\Display.java");
		v.addElement("$(GENERATED_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\GraphicsAccessImpl.java");
		v.addElement("$(GENERATED_DIR)\\events\\eventqueue\\reference\\classes\\com\\sun\\midp\\events\\EventQueue.java");
		v.addElement("$(GENERATED_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\GraphicsAccess.java");
		// jsr75 support
		v.addElement("$(GENERATED_DIR)\\share\\classes\\com\\sun\\cdc\\io\\j2me\\file\\symbian\\DefaultFileHandler.java");
		v.addElement("$(MIDP_SRC_DIR)\\core\\javautil\\reference\\classes\\com\\sun\\midp\\util\\DateParser.java");
        v.addElement("$(MIDP_SRC_DIR)\\core\\javautil\\reference\\classes\\com\\sun\\midp\\util\\Properties.java");
        v.addElement("$(MIDP_SRC_DIR)\\core\\log\\static\\classes\\com\\sun\\midp\\log\\Logging.java");
        v.addElement("$(MIDP_SRC_DIR)\\core\\log_base\\reference\\classes\\com\\sun\\midp\\log\\LoggingBase.java");
        v.addElement("$(MIDP_SRC_DIR)\\protocol\\gcf\\reference\\classes\\com\\sun\\midp\\io\\HttpUrl.java");
        v.addElement("$(MIDP_SRC_DIR)\\protocol\\gcf\\reference\\classes\\com\\sun\\midp\\io\\Util.java");
		v.addElement("$(MIDP_SRC_DIR)\\protocol\\gcf\\reference\\classes\\com\\sun\\midp\\io\\Base64.java");
		v.addElement("$(MIDP_SRC_DIR)\\protocol\\gcf\\reference\\classes\\com\\sun\\midp\\io\\NetworkConnectionBase.java");
		v.addElement("$(MIDP_SRC_DIR)\\protocol\\gcf\\reference\\classes\\com\\sun\\midp\\io\\BufferedConnectionAdapter.java");
  		v.addElement("$(MIDP_SRC_DIR)\\protocol\\socket\\classes\\javax\\microedition\\io\\SocketConnection.java");
  		v.addElement("$(MIDP_SRC_DIR)\\protocol\\socket\\classes\\com\\sun\\midp\\io\\j2me\\socket\\ServerSocket.java");
  		v.addElement("$(MIDP_SRC_DIR)\\protocol\\http\\classes\\javax\\microedition\\io\\HttpConnection.java");
  		v.addElement("$(MIDP_SRC_DIR)\\protocol\\http\\reference\\classes\\com\\sun\\midp\\io\\j2me\\http\\symbian\\Protocol.java");
  		v.addElement("$(MIDP_SRC_DIR)\\protocol\\http\\reference\\classes\\com\\sun\\midp\\io\\j2me\\http\\StreamConnectionElement.java");
  		v.addElement("$(MIDP_SRC_DIR)\\protocol\\http\\reference\\classes\\com\\sun\\midp\\io\\j2me\\http\\StreamConnectionPool.java");
  		v.addElement("$(MIDP_SRC_DIR)\\configuration\\properties\\reference\\classes\\com\\sun\\midp\\main\\Configuration.java");
		v.addElement("$(MIDP_SRC_DIR)\\core\\jarutil\\reference\\classes\\com\\sun\\midp\\jarutil\\JarReader.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\installer\\reference\\classes\\com\\sun\\midp\\installer\\InvalidJadException.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\installer\\reference\\classes\\com\\sun\\midp\\installer\\JadProperties.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\installer\\reference\\classes\\com\\sun\\midp\\installer\\ManifestProperties.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\suitestore\\internal_api\\reference\\classes\\com\\sun\\midp\\midletsuite\\MIDletInfo.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\javax\\microedition\\midlet\\MIDletStateChangeException.java");
		// low level graphics code
		v.addElement("$(MIDP_SRC_DIR)\\lowlevelui\\image\\classes\\javax\\microedition\\lcdui\\AbstractImageData.java");
		v.addElement("$(MIDP_SRC_DIR)\\lowlevelui\\image\\classes\\javax\\microedition\\lcdui\\AbstractImageDataFactory.java");
		v.addElement("$(MIDP_SRC_DIR)\\lowlevelui\\image\\img_putpixel\\classes\\javax\\microedition\\lcdui\\ImageData.java");
		v.addElement("$(MIDP_SRC_DIR)\\lowlevelui\\image\\img_putpixel\\classes\\javax\\microedition\\lcdui\\symbian\\ImageDataFactory.java");
		v.addElement("$(MIDP_SRC_DIR)\\lowlevelui\\graphics_api\\gxapi_native\\classes\\javax\\microedition\\lcdui\\Font.java");
		v.addElement("$(MIDP_SRC_DIR)\\lowlevelui\\graphics_api\\gxapi_native\\classes\\javax\\microedition\\lcdui\\Graphics.java");
		v.addElement("$(MIDP_SRC_DIR)\\lowlevelui\\image_api\\reference\\classes\\javax\\microedition\\lcdui\\Image.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\game\\Sprite.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\game\\TiledLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\game\\Layer.java");
		// high level graphics code
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\game\\LayerManager.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\game\\GameCanvas.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\game\\GameAccessImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\Canvas.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\Displayable.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\Item.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\ImageItem.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\ItemCommandListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\Screen.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\Alert.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\AlertType.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\Gauge.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\Ticker.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\Command.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\CommandListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\KeyConverter.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\Form.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\ItemStateListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\StringItem.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\CustomItem.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\TextBox.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\symbian\\TextField.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\LayoutManager.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\Choice.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\DisplayEventHandlerImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\ChoiceGroup.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\List.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\Spacer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\javax\\microedition\\lcdui\\DateField.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\LFFactory.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\LFTimer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\CanvasLF.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\DisplayableLF.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\AlertLF.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\GaugeLF.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\TickerLF.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\ItemLF.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\ImageItemLF.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\FormLF.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\StringItemLF.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\CustomItemLF.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\TextFieldLF.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\ChoiceGroupLF.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\SpacerLF.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\classes\\javax\\microedition\\lcdui\\DateFieldLF.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\LFFactoryImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\AlertLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\ScreenLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\symbian\\DisplayableLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\CanvasLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\TickerLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\GaugeLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\symbian\\ItemLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\ImageItemLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\symbian\\FormLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\symbian\\TextFieldLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\CustomItemLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\StringItemLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\symbian\\TextBoxLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\symbian\\ChoiceGroupLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\ChoiceGroupPopupLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\SpacerLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\DateFieldLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\DateEditor.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\javax\\microedition\\lcdui\\DEPopupLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\CommandAccess.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\DisplayStaticAccess.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\DisplayAccess.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\DisplayFactory.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\GameMap.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\GameAccess.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\GameCanvasLFImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\DisplayEventConsumer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\ForegroundEventConsumer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\ForegroundEventListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\EventConstants.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\DynamicCharacterArray.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\PhoneDial.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\TextPolicy.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\ForegroundController.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\DisplayContainer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\DisplayEventHandler.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\DisplayEventProducer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\RepaintEventProducer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\ItemEventConsumer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\DisplayDeviceAccess.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\RepaintEvent.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\DisplayEventHandlerFactory.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\SystemAlert.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\LCDUIEvent.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\DisplayEventListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdui\\reference\\classes\\com\\sun\\midp\\lcdui\\LCDUIEventListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\CLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\CWindow.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\CLayerList.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\CLayerElement.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\CGraphicsQ.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\ScrollListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\symbian\\MIDPWindow.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\CGraphicsUtil.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\ChamDisplayTunnel.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\SubMenuCommand.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\AlertSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\ScreenSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\ScrollIndSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\StringItemSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\TextFieldSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\GaugeSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\ImageItemSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\ProgressBarSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\BusyCursorSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\UpdateBarSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\SoftButtonSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\TitleSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\TickerSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\MenuSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\PTISkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\ChoiceGroupSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\DateEditorSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\DateFieldSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\InputModeSkin.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\symbian\\AlertResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\ScrollIndResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\ScrollIndResourcesConstants.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\GaugeResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\imageItemResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\ProgressBarResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\UpdateBarResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\BusyCursorResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\MenuResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\TextFieldResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\PTIResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\StringItemResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\SkinResourcesConstants.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\FontResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\FontResourcesConstants.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\ScreenResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\symbian\\SoftButtonResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\TickerResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\symbian\\TitleResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\ChoiceGroupResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\DateEditorResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\DateFieldResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\skins\\resources\\InputModeResources.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\BackgroundLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\Symbian\\BodyLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\AlertLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\ScrollBarLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\ScrollIndLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\PopupLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\symbian\\TitleLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\TickerLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\symbian\\SoftButtonLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\MenuLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\ScrollablePopupLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\CascadeMenuLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\Symbian\\PTILayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\Symbian\\WashLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\InputModeLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\layers\\ScrollArrowLayer.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\TextInputSession.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\symbian\\TextInputComponent.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\InputMode.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\symbian\\InputModeMediator.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\symbian\\BasicTextInputSession.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\InputModeFactory.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\SymbolInputMode.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\KeyboardInputMode.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\NumericInputMode.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\AlphaNumericInputMode.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\PredictiveTextInputMode.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\NativeInputMode.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\BasicInputMode.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\PTIterator.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\PTDictionaryFactory.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\chameleon\\input\\PTDictionary.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\pti_api\\javapti\\classes\\com\\sun\\midp\\chameleon\\input\\PTDictionaryImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\pti_api\\javapti\\classes\\com\\sun\\midp\\chameleon\\input\\PTIteratorImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\lcdui\\symbian\\Text.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\lcdui\\TextCursor.java");
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes\\com\\sun\\midp\\lcdui\\TextInfo.java");
		v.addElement("$(MIDP_SRC_DIR)\\mmapi\\classes\\com\\sun\\mmedia\\MMHelper.java");
		v.addElement("$(MIDP_SRC_DIR)\\mmapi\\classes\\javax\\microedition\\lcdui\\MMHelperImpl.java");
		v.addElement("$(MIDP_SRC_DIR)\\mmapi\\classes\\com\\sun\\mmedia\\MIDPVideoPainter.java");
		v.addElement("$(MIDP_SRC_DIR)\\core\\resource_handler\\file_based\\classes\\com\\sun\\midp\\util\\ResourceHandler.java");
		v.addElement("$(MIDP_SRC_DIR)\\protocol\\file\\reference\\classes\\com\\sun\\midp\\io\\j2me\\storage\\File.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\ams_api\\reference\\classes\\com\\sun\\midp\\midlet\\Scheduler.java");
		v.addElement("$(MIDP_SRC_DIR)\\i18n\\i18n_main\\reference\\classes\\com\\sun\\midp\\i18n\\Resource.java");
		v.addElement("$(MIDP_SRC_DIR)\\i18n\\i18n_main\\reference\\classes\\com\\sun\\midp\\l10n\\LocalizedStrings.java");
		v.addElement("$(MIDP_SRC_DIR)\\i18n\\i18n_main\\classes\\com\\sun\\midp\\i18n\\ResourceBundle.java");
		v.addElement("$(MIDP_SRC_DIR)\\events\\eventqueue\\reference\\classes\\com\\sun\\midp\\events\\Event.java");
		v.addElement("$(MIDP_SRC_DIR)\\events\\eventqueue\\reference\\classes\\com\\sun\\midp\\events\\NativeEvent.java");
		v.addElement("$(MIDP_SRC_DIR)\\events\\eventqueue\\classes\\com\\sun\\midp\\events\\EventTypes.java");
		v.addElement("$(MIDP_SRC_DIR)\\events\\eventqueue\\classes\\com\\sun\\midp\\events\\EventListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\i18n\\i18n_main\\reference\\classes\\com\\sun\\cldc\\i18n\\j2me\\UTF_8_Reader.java");
		v.addElement("$(MIDP_SRC_DIR)\\i18n\\i18n_main\\reference\\classes\\com\\sun\\cldc\\i18n\\j2me\\UTF_8_Writer.java");

		// audio player
		v.addElement("$(MIDP_SRC_DIR)\\media\\reference\\classes\\javax\\microedition\\media\\Player.java");
		v.addElement("$(MIDP_SRC_DIR)\\media\\reference\\classes\\javax\\microedition\\media\\Controllable.java");
		v.addElement("$(MIDP_SRC_DIR)\\media\\reference\\classes\\javax\\microedition\\media\\MediaException.java");
		v.addElement("$(MIDP_SRC_DIR)\\media\\reference\\classes\\javax\\microedition\\media\\PlayerListener.java");
		v.addElement("$(MIDP_SRC_DIR)\\media\\reference\\classes\\javax\\microedition\\media\\Control.java");
		v.addElement("$(MIDP_SRC_DIR)\\media\\reference\\classes\\javax\\microedition\\media\\control\\ToneControl.java");
		v.addElement("$(MIDP_SRC_DIR)\\media\\reference\\classes\\javax\\microedition\\media\\Symbian\\Manager.java");
		v.addElement("$(MIDP_SRC_DIR)\\media\\reference\\classes\\javax\\microedition\\media\\Symbian\\MyPlayer.java");

		// native menu support
		v.addElement("$(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfplatform\\classes\\javax\\microedition\\lcdui\\NativeMenu.java");
        
		addPlatformSpecificJava(v);
        
        // push registry support
		v.addElement("$(MIDP_SRC_DIR)\\push\\push_api\\javapush\\classes\\javax\\microedition\\io\\PushRegistry.java");
        
		// jsr 75 file system support
		v.addElement("$(JSR75_DIR)\\share\\classes\\javax\\microedition\\io\\file\\FileSystemRegistry.java");
		v.addElement("$(JSR75_DIR)\\share\\classes\\javax\\microedition\\io\\file\\FileSystemListener.java");
		v.addElement("$(JSR75_DIR)\\share\\classes\\com\\sun\\cdc\\io\\j2me\\file\\RootCache.java");
		v.addElement("$(JSR75_DIR)\\share\\classes\\com\\sun\\cdc\\io\\j2me\\file\\Protocol.java");
		v.addElement("$(JSR75_DIR)\\share\\classes\\com\\sun\\cdc\\io\\j2me\\file\\BaseFileHandler.java");
		v.addElement("$(JSR75_DIR)\\share\\classes\\javax\\microedition\\io\\file\\FileConnection.java");
		v.addElement("$(JSR75_DIR)\\share\\classes\\com\\sun\\j2me\\jsr75\\StringUtil.java");
		v.addElement("$(ABSTRACTIONS_DIR)\\cldc_application\\classes\\com\\sun\\j2me\\security\\FileConnectionPermission.java");
		v.addElement("$(ABSTRACTIONS_DIR)\\cldc_application\\classes\\com\\sun\\j2me\\io\\ConnectionBaseInterface.java");
		v.addElement("$(ABSTRACTIONS_DIR)\\cldc_application\\classes\\com\\sun\\j2me\\io\\ConnectionBaseAdapter.java");
		v.addElement("$(ABSTRACTIONS_DIR)\\cldc_application\\classes\\com\\sun\\j2me\\security\\Token.java");
		v.addElement("$(ABSTRACTIONS_DIR)\\cldc_application\\classes\\com\\sun\\j2me\\security\\Permission.java");
		v.addElement("$(ABSTRACTIONS_DIR)\\cldc_application\\classes\\com\\sun\\j2me\\main\\Configuration.java");
		v.addElement("$(ABSTRACTIONS_DIR)\\cldc_application\\classes\\com\\sun\\j2me\\app\\AppPackage.java");
		v.addElement("$(JSR75_DIR)\\cldc_application\\classes\\share\\javax\\microedition\\io\\file\\FileSystemEventHandlerBase.java");
		v.addElement("$(JSR75_DIR)\\cldc_application\\classes\\stubs\\javax\\microedition\\io\\file\\FileSystemEventHandler.java");
		v.addElement("$(JSR75_DIR)\\share\\classes\\javax\\microedition\\io\\file\\ConnectionClosedException.java");
		v.addElement("$(JSR75_DIR)\\share\\classes\\javax\\microedition\\io\\file\\IllegalModeException.java");
		v.addElement("$(JSR75_DIR)\\share\\classes\\com\\sun\\midp\\io\\j2me\\file\\Protocol.java");
		
        // Bluewhale systems AMS code
        v.addElement("$(MIDP_SRC_DIR)\\ams\\example\\jams\\classes\\com\\bluewhalesystems\\midp\\Launcher.java");
        v.addElement("$(MIDP_SRC_DIR)\\ams\\example\\jams\\classes\\com\\bluewhalesystems\\midp\\LauncherCustomItem.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\example\\jams\\classes\\com\\bluewhalesystems\\midp\\BWMDisplayController.java");
        v.addElement("$(MIDP_SRC_DIR)\\ams\\example\\jams\\classes\\com\\bluewhalesystems\\midp\\PlatformRequestListener.java");
        v.addElement("$(MIDP_SRC_DIR)\\ams\\example\\jams\\classes\\com\\bluewhalesystems\\midp\\Autostart.java");
		v.addElement("$(MIDP_SRC_DIR)\\ams\\example\\jams\\classes\\com\\bluewhalesystems\\midp\\SMSTextReader.java");

        // jsr 120 support
        v.addElement("$(GENERATED_DIR)\\share\\core\\common\\classes\\javax\\wireless\\messaging\\MessageConnection.java");
        v.addElement("$(JSR120_DIR)\\share\\core\\common\\classes\\javax\\wireless\\messaging\\Message.java");
        v.addElement("$(JSR120_DIR)\\share\\core\\common\\classes\\javax\\wireless\\messaging\\TextMessage.java");
        v.addElement("$(JSR120_DIR)\\share\\core\\common\\classes\\javax\\wireless\\messaging\\MessageListener.java");

        // jsr 211 support
		v.addElement("$(ABSTRACTIONS_DIR)\\cldc_application\\classes\\com\\sun\\j2me\\security\\TrustedClass.java");

        v.addElement("$(JSR211_DIR)\\cldc_application\\classes\\com\\sun\\jsr211\\security\\SecurityInitializer.java");
        v.addElement("$(JSR211_DIR)\\cldc_application\\classes\\com\\sun\\j2me\\content\\AppProxy.java"); 
        v.addElement("$(JSR211_DIR)\\cldc_application\\classes\\com\\sun\\j2me\\content\\InvocationStore.java");
        v.addElement("$(JSR211_DIR)\\cldc_application\\classes\\com\\sun\\j2me\\content\\Invoker.java");

        v.addElement("$(JSR211_DIR)\\share\\classes\\com\\sun\\j2me\\content\\ContentHandlerImpl.java");
        v.addElement("$(JSR211_DIR)\\share\\classes\\com\\sun\\j2me\\content\\ContentReader.java");   
        v.addElement("$(JSR211_DIR)\\share\\classes\\com\\sun\\j2me\\content\\InvocationImpl.java");
        v.addElement("$(JSR211_DIR)\\share\\classes\\com\\sun\\j2me\\content\\InvocationStoreProxy.java");
        v.addElement("$(JSR211_DIR)\\share\\classes\\com\\sun\\j2me\\content\\RegistryImpl.java");
        v.addElement("$(JSR211_DIR)\\share\\classes\\com\\sun\\j2me\\content\\RegistryStore.java");
        v.addElement("$(JSR211_DIR)\\share\\classes\\com\\sun\\j2me\\content\\RequestListenerImpl.java");
        v.addElement("$(JSR211_DIR)\\share\\classes\\com\\sun\\j2me\\content\\ResponseListenerImpl.java");

        v.addElement("$(JSR211_DIR)\\share\\classes\\javax\\microedition\\content\\ActionNameMap.java");
        v.addElement("$(JSR211_DIR)\\share\\classes\\javax\\microedition\\content\\ContentHandler.java");
        v.addElement("$(JSR211_DIR)\\share\\classes\\javax\\microedition\\content\\ContentHandlerException.java");
        v.addElement("$(JSR211_DIR)\\share\\classes\\javax\\microedition\\content\\ContentHandlerServer.java");
        v.addElement("$(JSR211_DIR)\\share\\classes\\javax\\microedition\\content\\ContentHandlerServerImpl.java");
        v.addElement("$(JSR211_DIR)\\share\\classes\\javax\\microedition\\content\\Invocation.java");
        v.addElement("$(JSR211_DIR)\\share\\classes\\javax\\microedition\\content\\Registry.java");
        v.addElement("$(JSR211_DIR)\\share\\classes\\javax\\microedition\\content\\RequestListener.java");
        v.addElement("$(JSR211_DIR)\\share\\classes\\javax\\microedition\\content\\ResponseListener.java");        

        
		put("MIDP_SOURCES =");
        for (int i=0; i<v.size(); i++) 
        {
            putln(" \\");
            put("  " + (String)v.elementAt(i));
        }
        putln();
		putln();
        
	}

void writeRules() throws Exception
    {
        putln();
        putln();
        putln("JDK_DIR = $(JAVA_1_4_2_HOME)");
		putln("JDK6_DIR = $(JAVA_HOME)");
        putln();
        putln("JAVAC = $(JDK_DIR)\\bin\\javac.exe");
        putln("JAVAC6 = $(JDK6_DIR)\\bin\\javac.exe");
        putln("JAR = $(JDK_DIR)\\bin\\jar.exe");
        putln("JAVA = $(JDK_DIR)\\bin\\java.exe");
        putln("JAVA6 = $(JDK6_DIR)\\bin\\java.exe");
        putln();
        putln("ROOT_DIR="  + tool().getWorkSpaceArg() + "\\..");
		putln("CLDC_TOOLS=..\\javaapi");
        putln("MIDP_TOOLS=.\\tools");
        putln("PREVERIFY = $(ROOT_DIR)\\cldc\\build\\share\\bin\\win32_i386\\preverify");
        putln("ROMGEN = ..\\romgen\\romgen.exe");
        putln("JSR75_DIR=$(ROOT_DIR)\\jsr75\\src");
        putln("JSR120_DIR=$(ROOT_DIR)\\jsr120\\src");
        putln("JSR211_DIR=$(ROOT_DIR)\\jsr211\\src");
		putln("RESTRICTED_CRYPTO_DIR=$(ROOT_DIR)\\restricted_crypto\\src");
		putln("ABSTRACTIONS_DIR=$(ROOT_DIR)\\abstractions\\src");
        putln("MIDP_SRC_DIR="  + tool().getWorkSpaceArg() + "\\..\\midp\\src");
        putln("CLDC_SRC_DIR="  + tool().getWorkSpaceArg() + "\\..\\cldc\\src\\javaapi\\cldc1.1");
        putln();
        putln("GENERATED_DIR=preproc");
        putln();
		putln("INT_IMAGEUTIL_DIR = $(MIDP_SRC_DIR)\\tool\\imageutil\\classes\\com\\sun\\midp\\imageutil");
		putln("INT_SKINROMIZATION_DIR = $(MIDP_SRC_DIR)\\tool\\chameleon\\classes\\com\\sun\\midp\\skinromization");
		putln("INT_ROMIZER_DIR = $(MIDP_SRC_DIR)\\tool\\romizer\\classes\\com\\sun\\midp\\romization");
		putln("INT_LCDLF_J_DIR = $(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\classes");
		putln("INT_L10N_GENERATOR_DIR = $(MIDP_SRC_DIR)\\tool\\l10ngen\\classes\\com\\sun\\midp\\l10n\\generator");
		putln("INT_KEYTOOL_DIR = $(MIDP_SRC_DIR)\\tool\\mekeytool\\classes\\com\\sun\\midp\\mekeytool");
		putln("INT_KEYSTORE_DIR = $(MIDP_SRC_DIR)\\security\\publickeystore\\reference\\classes\\com\\sun\\midp\\publickeystore");
		putln();
		putln("MODULE_TOOL_KEYTOOL_JAVA_FILES = \\");
		puttabln("$(INT_KEYTOOL_DIR)\\MEKeyTool.java \\");
		puttabln("$(INT_KEYSTORE_DIR)\\PublicKeyStoreBuilderBase.java \\");
		puttabln("$(INT_KEYSTORE_DIR)\\OutputStorage.java \\");
		puttabln("$(INT_KEYSTORE_DIR)\\PublicKeyStore.java \\");
		puttabln("$(INT_KEYSTORE_DIR)\\PublicKeyInfo.java \\");
		puttabln("$(INT_KEYSTORE_DIR)\\InputStorage.java \\");
		puttabln("$(INT_KEYSTORE_DIR)\\Storage.java");
		putln();
		putln("ME_KEYTOOL_CLASS = com.sun.midp.mekeytool.MEKeyTool");
		putln("KEYSTORE = ..\\generated\\_main.ks");
		putln("J2SE_KEYSTORE = $(ROOT_DIR)\\midp-keystore\\main.ks");
		putln();
		putln("MODULE_TOOL_IMAGE2RAW_JAVA_FILES = \\");
		puttabln("$(INT_IMAGEUTIL_DIR)\\ImageToRawConverter.java \\");
		puttabln("$(INT_IMAGEUTIL_DIR)\\ImageToRawTool.java");
		putln();
		putln("MODULE_TOOL_SKINROMIZATION_JAVA_FILES = \\");
		puttabln("$(INT_ROMIZER_DIR)\\RomUtil.java \\");
		puttabln("$(INT_ROMIZER_DIR)\\BinaryOutputStream.java \\");
		puttabln("$(INT_ROMIZER_DIR)\\RomizedByteArray.java \\");
		puttabln("$(INT_IMAGEUTIL_DIR)\\ImageToRawConverter.java \\");
		puttabln("$(INT_SKINROMIZATION_DIR)\\SkinRomizationTool.java \\");
		puttabln("$(INT_LCDLF_J_DIR)\\com\\sun\\midp\\chameleon\\skins\\resources\\FontResourcesConstants.java \\");
	    puttabln("$(INT_LCDLF_J_DIR)\\com\\sun\\midp\\chameleon\\skins\\resources\\SkinResourcesConstants.java \\");
	    puttabln("$(INT_LCDLF_J_DIR)\\com\\sun\\midp\\chameleon\\skins\\resources\\ScrollIndResourcesConstants.java");
		putln();
		putln("MODULE_TOOL_L10N_GENERATOR_JAVA_FILES = \\");
		puttabln("$(INT_L10N_GENERATOR_DIR)\\LocalizedStringsGenerator.java");
		putln();
		
		putln("INT_ROMIZE_SKIN = \\");
		puttabln("$(JAVA6) -Djava.awt.headless=true -classpath $(MIDP_TOOLS) com.sun.midp.skinromization.SkinRomizationTool");
		putln();
		
		putln("INT_LOCALIZE_STRINGS = \\");
		puttabln("$(JAVA6) -Djava.awt.headless=true -classpath $(MIDP_TOOLS) com.sun.midp.l10n.generator.LocalizedStringsGenerator");
		putln();
	
		putln("SUBSYSTEM_LCDLF_ROMIZED_SKIN_BIN_FILE = .\\skin.bin");
		putln();
	
		putln("SYMBIAN_SDK_VM_ROOT_PATH = \\");
		puttabln("$(EPOCROOT)\\Epoc32\\winscw\\c\\private\\2000E27A");
		putln("SYMBIAN_SDK_VM_RESOURCES_PATH = \\");
		puttabln("$(SYMBIAN_SDK_VM_ROOT_PATH)\\vm\\resources\\");
		putln();
		putln("SYMBIAN_SDK_VM_APP_PATH = \\");
		puttabln("$(SYMBIAN_SDK_VM_ROOT_PATH)\\app\\");
		putln();
		
        putln(".PHONY : all");
        putln();
        putln("all: $(KEYSTORE) ..\\generated\\lfj_image_rom.c RomImageGenerated.hpp NativesTable.cpp $(SYMBIAN_SDK_VM_RESOURCES_PATH) $(SYMBIAN_SDK_VM_APP_PATH)");
        puttabln("-@copy *.cpp ..\\generated");
		puttabln("-@copy *.c ..\\generated");
        puttabln("-@copy *.hpp ..\\generated");
        puttabln("-@copy $(SUBSYSTEM_LCDLF_ROMIZED_SKIN_BIN_FILE) $(SYMBIAN_SDK_VM_RESOURCES_PATH)");
		puttabln("-@copy $(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\resource\\skin\\*.png $(SYMBIAN_SDK_VM_RESOURCES_PATH)"); 
		puttabln("-@copy $(KEYSTORE) $(SYMBIAN_SDK_VM_APP_PATH)");
        puttabln("-@copy $(MIDP_SRC_DIR)\\ams\\appmanager_ui_resources\\share\\*.png $(SYMBIAN_SDK_VM_APP_PATH)");
		puttabln("-@copy $(MIDP_SRC_DIR)\\ams\\ota\\reference\\classes\\com\\sun\\midp\\installer\\resources\\*.png $(SYMBIAN_SDK_VM_APP_PATH)");
        putln();
		putln("$(SYMBIAN_SDK_VM_ROOT_PATH) : ");
		puttabln("-@mkdir $(SYMBIAN_SDK_VM_ROOT_PATH)");
		putln();
		putln("$(SYMBIAN_SDK_VM_RESOURCES_PATH) : $(SYMBIAN_SDK_VM_ROOT_PATH)");
		puttabln("-@mkdir $(SYMBIAN_SDK_VM_RESOURCES_PATH)");
		putln();
        putln("$(SYMBIAN_SDK_VM_APP_PATH) : $(SYMBIAN_SDK_VM_ROOT_PATH)");
		puttabln("-@mkdir $(SYMBIAN_SDK_VM_APP_PATH)");
		putln();
        putln("clean:");
        puttabln("-@rmdir /s /q tmpclasses");
        puttabln("-@rmdir /s /q bin");
        puttabln("-@rmdir /s /q $(GENERATED_DIR)");
		puttabln("-@rmdir /s /q tools");
	    puttabln("-@del *.zip");
	    puttabln("-@del *.cpp");
	    puttabln("-@del *.java");
	    puttabln("-@del *.c");
	    puttabln("-@del *.hpp");
		puttabln("-@del *.txt");
	    puttabln("-@del *skin.bin");
        puttabln("-@del $(KEYSTORE)");
        puttabln("-@del ..\\generated\\*.cpp");
		puttabln("-@del ..\\generated\\*.c");
        puttabln("-@del ..\\generated\\*.hpp");
        puttabln("-@del ..\\generated\\*.h");
        puttabln("-@del ..\\generated\\merged.xml");
        puttabln("-@rmdir /s /q $(SYMBIAN_SDK_VM_ROOT_PATH)");
		putln();
        putln("tmpclassesDir:");
        puttabln("-@mkdir tmpclasses > NUL 2> NUL");
        putln();
        putln("binDir:");
        puttabln("-@mkdir bin > NUL 2> NUL");
        putln();
		putln("SUBSYSTEM_CONFIGURATION_MERGED_FILE = ..\\generated\\merged.xml");
		putln("SUBSYSTEM_LCDLF_SKIN_RESOURCES_DIR = $(MIDP_SRC_DIR)\\highlevelui\\lcdlf\\lfjava\\resource\\skin");
		putln("INT_COMMON_XML_DIR = ..\\..\\..\\..\\midp\\src\\configuration\\configuration_xml\\share");
		putln("INT_SYMBIAN_XML_DIR = ..\\..\\..\\..\\midp\\src\\configuration\\configuration_xml\\symbian");
		putln();
		putln("INT_INPUT_FILES_FIXED = \\");
		puttabln("$(JSR75_DIR)\\cldc_application\\config\\share\\properties_jsr75_fc.xml \\");
		puttabln("$(JSR75_DIR)\\cldc_application\\config\\win32\\properties_jsr75_fc.xml \\");
		puttabln("$(INT_SYMBIAN_XML_DIR)\\constants.xml \\");
		puttabln("$(INT_SYMBIAN_XML_DIR)\\properties.xml \\");
		puttabln("$(INT_SYMBIAN_XML_DIR)\\rawimage.xml \\");
		puttabln("$(INT_SYMBIAN_XML_DIR)\\constants_open.xml \\");
		puttabln("$(INT_SYMBIAN_XML_DIR)\\skin.xml \\");
		puttabln("$(INT_COMMON_XML_DIR)\\constants.xml \\");
		puttabln("$(INT_COMMON_XML_DIR)\\i18n_constants.xml \\");
		puttabln("$(INT_COMMON_XML_DIR)\\l10n\\en-US.xml \\");
		puttabln("$(INT_COMMON_XML_DIR)\\chameleon\\skin_constants.xml \\");
		puttabln("$(INT_COMMON_XML_DIR)\\suitestore_constants.xml \\");
		puttabln("$(INT_COMMON_XML_DIR)\\alert_type_constants.xml \\");
		puttabln("$(INT_COMMON_XML_DIR)\\ams_cldc\\storage_constants.xml \\");
		puttabln("$(INT_COMMON_XML_DIR)\\l10n\\project_en-US.xml \\");
		puttabln("$(INT_COMMON_XML_DIR)\\monet_disabled.xml \\");
		puttabln("$(INT_COMMON_XML_DIR)\\verify_once_disabled.xml \\");
		puttabln("$(INT_COMMON_XML_DIR)\\properties.xml");
		putln();
		
        putln("$(KEYSTORE) : $(SYMBIAN_SDK_VM_ROOT_PATH) $(MIDP_TOOLS) $(J2SE_KEYSTORE)");
		puttabln("@echo Making ME keystore $(KEYSTORE)");
		puttabln("@$(JAVA6) -cp .\\tools $(ME_KEYTOOL_CLASS) -import -alias rootCA -domain trusted -keystore $(J2SE_KEYSTORE) -MEkeystore $(KEYSTORE) -storepass 3edcvgy76tfcxsw2");
		puttabln("@$(JAVA6) -cp .\\tools $(ME_KEYTOOL_CLASS) -import -alias manufacturerCA -domain manufacturer -keystore $(J2SE_KEYSTORE) -MEkeystore $(KEYSTORE) -storepass 3edcvgy76tfcxsw2");
		puttabln("@$(JAVA6) -cp .\\tools $(ME_KEYTOOL_CLASS) -list -MEkeystore $(KEYSTORE)");

		putln();
		putln("..\\buildtool\\com\\sun\\xml\\transform\\CodeTransformer.class :");
		puttabln("@$(JAVAC) -d ..\\buildtool ..\\..\\..\\..\\tools\\configurator\\classes\\com\\sun\\xml\\transform\\*.java");
		putln();
		putln("$(SUBSYSTEM_CONFIGURATION_MERGED_FILE) : ..\\buildtool\\com\\sun\\xml\\transform\\CodeTransformer.class");
		puttabln("@echo Merging config files");
		puttabln("@$(JAVA) -classpath ..\\buildtool com.sun.xml.transform.CodeTransformer \\");
		puttabln("-xml $(MIDP_SRC_DIR)/configuration/configuration_xml/share/empty.xml \\");
		puttabln("-xsl $(ROOT_DIR)/tools/configurator/xsl/share/merge.xsl \\");
		puttabln("-params filesList \"$(INT_INPUT_FILES_FIXED)\" \\");
		puttabln("-out $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
		
		putln();
		putln("..\\generated\\lfj_image_rom.c : $(MIDP_TOOLS) $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
		puttabln("$(INT_ROMIZE_SKIN) -xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) \\");
		puttabln("-imagedir $(SUBSYSTEM_LCDLF_SKIN_RESOURCES_DIR) \\");
		puttabln("-outbin $(SUBSYSTEM_LCDLF_ROMIZED_SKIN_BIN_FILE) \\");
		puttabln("-outc ..\\generated\\lfj_image_rom.c");
		putln();
		
        putln("..\\generated\\LocalizedStringsBase.c : $(MIDP_TOOLS) $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
		puttabln("@echo making localized string files");
		puttabln("$(INT_LOCALIZE_STRINGS) -xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) -out .");
        puttabln("@move LocalizedStringsBase.c ..\\generated\\LocalizedStringsBase.c");
		puttabln("@move LocalizedStringsBase.java preproc\\LocalizedStringsBase.java");
		putln();
		
		putln("$(MIDP_TOOLS):");
		puttabln("-@mkdir $(MIDP_TOOLS)");
		puttabln("$(JAVAC6) -classpath $(MIDP_TOOLS) -d $(MIDP_TOOLS) $(MODULE_TOOL_IMAGE2RAW_JAVA_FILES)");
		puttabln("$(JAVAC6) -classpath $(MIDP_TOOLS) -d $(MIDP_TOOLS) $(MODULE_TOOL_SKINROMIZATION_JAVA_FILES)");
		puttabln("$(JAVAC6) -classpath $(MIDP_TOOLS) -d $(MIDP_TOOLS) $(MODULE_TOOL_L10N_GENERATOR_JAVA_FILES)");
		puttabln("$(JAVAC6) -classpath $(MIDP_TOOLS) -d $(MIDP_TOOLS) $(MODULE_TOOL_KEYTOOL_JAVA_FILES)");
		putln();
		
        putln("$(GENERATED_DIR):");
        puttabln("-@mkdir $(GENERATED_DIR) > NUL 2> NUL");
        putln();

		writeJavaSource();
		
	    putln("..\\generated\\midp_constants_data.h : $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
        puttabln("@$(JAVA) -classpath ..\\buildtool com.sun.xml.transform.CodeTransformer -xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) -xsl ..\\..\\..\\..\\tools\\configurator\\xsl\\cldc\\constantsNative.xsl -out ..\\generated\\midp_constants_data.h"); 
		puttabln("@copy ..\\generated\\midp_constants_data.h $(EPOCROOT)\\epoc32\\include\\midp_constants_data.h");
        putln();

	    putln("..\\generated\\midp_constants_data.cpp : $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
        puttabln("@$(JAVA) -classpath ..\\buildtool com.sun.xml.transform.CodeTransformer -xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) -xsl ..\\..\\..\\..\\tools\\configurator\\xsl\\symbian\\stringConstantsNative.xsl -out ..\\generated\\midp_constants_data.cpp"); 
        putln();

        putln("..\\generated\\midp_property_callouts.h : $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
        puttabln("@$(JAVA) -classpath ..\\buildtool com.sun.xml.transform.CodeTransformer -xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) -xsl ..\\..\\..\\..\\tools\\configurator\\xsl\\cldc\\propertyCallouts.xsl -out ..\\generated\\midp_property_callouts.h"); 
        putln();
        
        putln("..\\generated\\midp_properties_static_data.h : $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
        puttabln("@$(JAVA) -classpath ..\\buildtool com.sun.xml.transform.CodeTransformer -xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) -xsl ..\\..\\..\\..\\tools\\configurator\\xsl\\cldc\\propertiesNative.xsl -out ..\\generated\\midp_properties_static_data.h"); 
		putln();
	
		putln("..\\generated\\properties_static_data.c : $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
        puttabln("@$(JAVA) -classpath ..\\buildtool com.sun.xml.transform.CodeTransformer -xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) -xsl ..\\..\\..\\..\\tools\\configurator\\xsl\\cldc\\propertiesNative.xsl -params arrayNamePrefix midp -out ..\\generated\\properties_static_data.c"); 
		putln();
        
        putln("preproc\\Constants.java : $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
        puttabln("@$(JAVA) -classpath ..\\buildtool com.sun.xml.transform.CodeTransformer -xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) -xsl ..\\..\\..\\..\\tools\\configurator\\xsl\\cldc\\constantsJava.xsl -params packageName com.sun.midp.configurator.Constants -out preproc\\Constants.java"); 
		putln();
        
        putln("preproc\\LogChannels.java : $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
        puttabln("@$(JAVA) -classpath ..\\buildtool com.sun.xml.transform.CodeTransformer -xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) -xsl ..\\..\\..\\..\\tools\\configurator\\xsl\\cldc\\constantsJava.xsl -params packageName com.sun.midp.log.LogChannels -out preproc\\LogChannels.java"); 
		putln();
        
		putln("preproc\\AlertTypeConstants.java : $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
        puttabln("@$(JAVA) -classpath ..\\buildtool com.sun.xml.transform.CodeTransformer -xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) -xsl ..\\..\\..\\..\\tools\\configurator\\xsl\\cldc\\constantsJava.xsl -params packageName com.sun.midp.configurator.AlertTypeConstants -out preproc\\AlertTypeConstants.java"); 
		putln();
        
		putln("preproc\\SkinPropertiesIDs.java : $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
        puttabln("@$(JAVA) -classpath ..\\buildtool com.sun.xml.transform.CodeTransformer -xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) -xsl ..\\..\\..\\..\\tools\\configurator\\xsl\\cldc\\constantsJava.xsl -params packageName com.sun.midp.chameleon.skins.SkinPropertiesIDs -out preproc\\SkinPropertiesIDs.java"); 
		putln();

        putln("preproc\\RMSConfig.java : $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
        puttabln("@$(JAVA) -classpath ..\\buildtool com.sun.xml.transform.CodeTransformer -xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) -xsl ..\\..\\..\\..\\tools\\configurator\\xsl\\cldc\\constantsJava.xsl -params packageName com.sun.midp.rms.RMSConfig -out preproc\\RMSConfig.java"); 
		putln();

        putln("preproc\\ResourceConstants.java : $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
        puttabln("@$(JAVA) -classpath ..\\buildtool com.sun.xml.transform.CodeTransformer -xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) -xsl ..\\..\\..\\..\\tools\\configurator\\xsl\\cldc\\constantsJava.xsl -params packageName com.sun.midp.i18n.ResourceConstants -out preproc\\ResourceConstants.java"); 
        putln();
        
        putln("preproc\\PermissionsTable.java : $(SUBSYSTEM_CONFIGURATION_MERGED_FILE)");
		puttabln("@$(JAVA) -classpath ..\\buildtool com.sun.xml.transform.CodeTransformer \\");
		puttabln("-xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) \\");
		puttabln("-xsl ..\\..\\..\\..\\tools\\configurator\\xsl\\cldc\\permissions.xsl \\");
		puttabln("-params output java \\");
		puttabln("-out preproc\\PermissionsTable.java \\");
		puttabln("-xml $(SUBSYSTEM_CONFIGURATION_MERGED_FILE) \\");
		puttabln("-xsl ..\\..\\..\\..\\tools\\configurator\\xsl\\cldc\\permissions.xsl \\");
		puttabln("-params output native \\");
		puttabln("-out ..\\generated\\perm_number.h");
        
        putln("Configconstants : ..\\generated\\midp_constants_data.h ..\\generated\\midp_constants_data.cpp ..\\generated\\midp_property_callouts.h ..\\generated\\properties_static_data.c ..\\generated\\midp_properties_static_data.h preproc\\Constants.java preproc\\LogChannels.java preproc\\AlertTypeConstants.java preproc\\SkinPropertiesIDs.java preproc\\PermissionsTable.java preproc\\RMSConfig.java preproc\\ResourceConstants.java ..\\generated\\LocalizedStringsBase.c preproc\\LocalizedStringsBase.java");
        putln();
        
        putln();
        putln("tmpclasses: Configconstants tmpclassesDir $(MIDP_SOURCES)");
        puttabln("@echo Creating MIDP classes");
        puttabln("@\"$(JAVAC)\" -classpath tmpclasses;..\\javaapi\\classes.zip -d tmpclasses @<<");
        putln("$(MIDP_SOURCES)");
        putln("<<");
        putln();
        
        putln("bin: binDir tmpclasses $(MIDP_TMPS)");
        puttabln("@echo Creating MIDP bins");
        puttabln("@$(PREVERIFY) -classpath tmpclasses;..\\javaapi\\classes.zip -d bin tmpclasses");
        putln();
        
                
        putln("classes.zip : midp.mak bin");
        puttabln("@copy ..\\javaapi\\classes.zip");
        puttabln("$(JAR) -ufM0 classes.zip -C ./bin .");
        putln();
        putln("RomImageGenerated.hpp : classes.zip");
        puttabln("$(ROMGEN) -cp classes.zip =HeapCapacity16M -romize -romconfig " + tool().getWorkSpaceArg() + "\\src\\vm\\cldc_rom.cfg");
        putln();
        putln("NativesTable.cpp : classes.zip");
        puttabln("$(JAVA) -jar $(CLDC_TOOLS)/jcc.jar -writer CLDC_HI_Natives -o NativesTable.cpp classes.zip");
        putln();
    }
 }

