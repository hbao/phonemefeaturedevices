. setup.sh
export MIDP_BUILD_PLATFORM=wince

rm -rf $Output/midp

make -C $HOME/midp/build/$MIDP_BUILD_PLATFORM \
USE_VS2005=false \
JDK_DIR=$JDK_DIR \
TOOLS_DIR=$HOME/tools \
TOOLS_OUTPUT_DIR=$Output/tools \
CLDC_DIST_DIR=$Output/cldc/wince_arm/dist \
PCSL_OUTPUT_DIR=$Output/pcsl \
PCSL_PLATFORM=wince_arm \
MIDP_OUTPUT_DIR=$Output/midp \
TARGET_CPU=arm \
CONFIGURATION_OVERRIDE=$HOME/midp/src/configuration/configuration_xml/wince/sp176x220.xml \
USE_QT_FB=false \
USE_DIRECTFB=false \
USE_DIRECTDRAW=false \
USE_OEM_AMS= \
OEM_AMS_DIR= \
USE_OEM_PUSH= \
OEM_PUSH_DIR= \
USE_RESTRICTED_CRYPTO=false \
VERIFY_BUILD_ENV= \
USE_SSL=false USE_CONFIGURATOR=true \
VERBOSE_BUILD=true \
USE_VERBOSE_MAKE=true \
GNU_TOOLS_BINDIR= \
USE_DEBUG=false \
USE_MULTIPLE_ISOLATES=true \
JPEG_DIR=$HOME/jpeg \
ABSTRACTIONS_DIR=$HOME/abstractions \
XMLPARSER_DIR=$HOME/xmlparser \
CVM_INCLUDE_COMMCONNECTION=true \
RESTRICTED_CRYPTO_DIR=$HOME/restricted_crypto USE_SSL=true USE_RESTRICTED_CRYPTO=true
$1

if [ $? -ne 0 ]; then
echo "midp_status=failed" >> $Log
else
echo "midp_status=OK" >> $Log
fi

. teardown.sh
