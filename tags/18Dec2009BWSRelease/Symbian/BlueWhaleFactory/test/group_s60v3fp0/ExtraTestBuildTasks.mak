INCDIR=..\src
SOSDIR=..\..\..\SymbianOSUnit
!if "$(PLATFORM)"=="GCCE" || "$(PLATFORM)"=="ARMI"
CLEAN:
	-del $(INCDIR)\testdriver.h

!else # Platform WINS
CLEAN :
	-del $(INCDIR)\testdriver.h

!endif

MAKEFILE : $(INCDIR)\testdriver.h 



HEADERS = \
$(INCDIR)\TestCreateImplementationL.h \
$(INCDIR)\TestProperties.h \
$(INCDIR)\TestSocketEngine.h \
$(INCDIR)\TestNetworkInfo.h \
$(INCDIR)\TestAPNDatabase.h \
$(INCDIR)\TestOOMTest.h \
$(INCDIR)\TestIAPInfo.h \
$(INCDIR)\TestCommDBUtil.h \
$(INCDIR)\TestAPNManager.h \
$(INCDIR)\APNMAnagerTests\TestAPNManagerWithPorts.h \
$(INCDIR)\APNMAnagerTests\TestAutoAPN.h \
$(INCDIR)\TestFEP.h

$(INCDIR)\testdriver.h:  $(HEADERS) ExtraTestBuildTasks.mak
	perl -S $(SOSDIR)\SymbianOSUnit\cxxtestgen.pl -o $(INCDIR)\TestDriver.h $(HEADERS)

# Boilerplate to stop errors:

BLD : BUILD

LIB : LIBRARY

MAKMAKE : MAKEFILE

SAVESPACE :

RELEASABLES :

BUILD CLEANEXPORT FREEZE LIBRARY LISTING MAKEFILE TARGET TIDY RESOURCE :

CLEANMAKEFILE :

FINAL :
