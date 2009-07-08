. setup.sh

REVISION="$1"
CURRENT_DIR_UNIX=`pwd`
CURRENT_DIR_WINDOWS=`cygpath -w $CURRENT_DIR_UNIX`
OUTPUT_DIR_WINDOWS=`cygpath -w $Output`
InfFile=$CURRENT_DIR_WINDOWS/phoneME_Feature.inf
DatFile=$CURRENT_DIR_WINDOWS/phoneME_Feature.DAT
CabFile=$CURRENT_DIR_WINDOWS/phoneME_Feature.CAB

MEKEYTOOL="c:\\wince_vm\\wm2003_ppc\\midp\\bin\\arm\\MEKeyTool.jar"
MEKEYSTORE="c:\\wince_vm\\wm2003_ppc\\midp\\appdb\\_main.ks"
SEKEYSTORE="..\\..\\..\\phoneme_feature\\midp-keystore\\main.ks"


if [ -e /cygdrive/c/cyg4me_sun/bin/upx.exe ]; then
cmd.exe /c "c:\cyg4me_sun\bin\upx --best --ultra-brute c:\wince_vm\wm2003_ppc\midp\bin\arm\runMidlet.exe"
fi

java -jar $MEKEYTOOL -import -alias manufacturerCA -domain manufacturer -keystore $SEKEYSTORE -MEkeystore $MEKEYSTORE -storepass 3edcvgy76tfcxsw2
java -jar $MEKEYTOOL -import -alias rootCA -domain trusted -keystore $SEKEYSTORE -MEkeystore $MEKEYSTORE -storepass 3edcvgy76tfcxsw2

cmd.exe /c "attrib +R +A $OUTPUT_DIR_WINDOWS\midp\*.* /S"

# Make sure the Cab Wizard does not block the build process if some files
# are missing
if [ -e "/cygdrive/c/wince_vm/wm2003_ppc/midp/bin/arm/runMidlet.exe" ]; then
    /cygdrive/c/Program\ Files/Windows\ CE\ Tools/wce420/POCKET\ PC\ 2003/Tools/Cabwiz.exe $InfFile

    if [ $? -ne 0 ]; then
        echo "cab_status=failed" >> $Log
    else
        echo "cab_status=OK" >> $Log
        rm -f $DatFile

        rm -rf "$Output/deployed/"
        mkdir -p "$Output/deployed/"
        mv $CabFile "$Output/deployed/vm_wm2003ppcarm_$REVISION.cab"
    fi
else
    echo "cab_status=failed" >> $Log
fi

. teardown.sh
