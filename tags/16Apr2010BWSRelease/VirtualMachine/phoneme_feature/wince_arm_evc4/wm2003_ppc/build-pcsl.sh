. setup.sh

export PCSL_BUILD_PLATFORM=wince_arm_vc

rm -rf $Output/pcsl
mkdir -p $Output/log

make -C $HOME/pcsl \
PCSL_PLATFORM=$PCSL_BUILD_PLATFORM \
PCSL_OUTPUT_DIR=$Output/pcsl \
TOOLS_DIR=$HOME/tools \
TOOLS_OUTPUT_DIR=$Output/tools \
VERBOSE_BUILD=true \
$1 2>&1 | tee $Output/log/pcsl.log

if [ $? -ne 0 ]; then
echo "build-pcsl=failed" >> $Log
else
echo "build-pcsl=OK" >> $Log
fi

. teardown.sh
