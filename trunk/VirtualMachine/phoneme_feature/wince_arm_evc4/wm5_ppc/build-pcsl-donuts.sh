. setup.sh

export PCSL_BUILD_PLATFORM=wince_arm_vc

rm -rf $Output/pcsl/wince_arm/bin/donuts.exe

make -C $HOME/pcsl \
PCSL_PLATFORM=$PCSL_BUILD_PLATFORM \
PCSL_OUTPUT_DIR=$Output/pcsl \
TOOLS_DIR=$HOME/tools \
TOOLS_OUTPUT_DIR=$Output/tools \
donuts

if [ $? -ne 0 ]; then
echo "build-pcsl-donuts=failed" >> $Log
else
echo "build-pcsl-donuts=OK" >> $Log
fi

. teardown.sh