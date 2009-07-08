. setup.sh

export CLDC_BUILD_PLATFORM=wince_arm

rm -rf $Output/cldc
mkdir -p $Output/log

make -C $HOME/cldc/build/$CLDC_BUILD_PLATFORM \
JDK_DIR=$JDK_DIR \
USE_DEBUG=false \
VERBOSE_BUILD=true \
ENABLE_PCSL=true \
PCSL_OUTPUT_DIR=$Output/pcsl \
ENABLE_ISOLATES=true \
ENABLE_JAVA_DEBUGGER=false \
ENABLE_SYSTEM_CLASSES_DEBUG=false \
ENABLE_ROM_JAVA_DEBUGGER=false \
JVMWorkSpace=$HOME/cldc \
JVMBuildSpace=$Output/cldc \
TOOLS_DIR=$HOME/tools \
TOOLS_OUTPUT_DIR=$Output/tools \
BUILD_VERSION=$1 \
$2 2>&1 | tee $Output/log/cldc.log

if [ $? -ne 0 ]; then
echo "cldc_status=failed" >> $Log
else
echo "cldc_status=OK" >> $Log
cp $Output/cldc/wince_arm/romgen/app/romgen.exe.manifest $Output/cldc/wince_arm/dist/bin/
fi

. teardown.sh
