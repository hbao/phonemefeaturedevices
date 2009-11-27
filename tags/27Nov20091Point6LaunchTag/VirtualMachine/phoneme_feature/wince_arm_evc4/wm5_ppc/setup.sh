CURRENT_DIR_UNIX=`pwd`
HOME_DIR_UNIX=`cd ../../; pwd`

export VM_BUILD_SCRIPTS_DIR=`cygpath -m $CURRENT_DIR_UNIX`
export HOME=`cygpath -m $HOME_DIR_UNIX`

export COMPONENTS_DIR=$HOME
export Output=c:/wince_vm/wm5_ppc
export Log=$Output/log.txt
export CE_VERSION=500

# Quit early if error encountered
set -o errexit
set -o nounset

# Create output folder
mkdir -p $Output

# Clear log
rm -f $Log
