INCDIR=..\src
SOSDIR=..\..\..\SymbianOSUnit
!if "$(PLATFORM)"=="GCCE" || "$(PLATFORM)"=="ARMI"
CLEAN:
	-del $(INCDIR)\testdriver_uiq.h

!else # Platform WINS
CLEAN :
	-del $(INCDIR)\testdriver_uiq.h

!endif

MAKEFILE : $(INCDIR)\testdriver_uiq.h 



HEADERS = \
$(INCDIR)\TestCreateImplementationL.h \
$(INCDIR)\TestProperties.h \
$(INCDIR)\TestSocketEngine.h \
$(INCDIR)\TestFEP.h

$(INCDIR)\testdriver_uiq.h:  $(HEADERS) ExtraTestBuildTasks.mak
	perl -S $(SOSDIR)\SymbianOSUnit\cxxtestgen.pl -o $(INCDIR)\TestDriver_UIQ.h $(HEADERS)

# Boilerplate to stop errors:

BLD : BUILD

LIB : LIBRARY

MAKMAKE : MAKEFILE

SAVESPACE :

RELEASABLES :

BUILD CLEANEXPORT FREEZE LIBRARY LISTING MAKEFILE TARGET TIDY RESOURCE :

CLEANMAKEFILE :

FINAL :
