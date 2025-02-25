#!/bin/bash

#+--------------------------------------------------------------------------------+
#|                                                                                |
#|   tamplate.sh                                                                  |
#|                                                                                |
#+--------------------------------------------------------------------------------+
#|   Guillaume Plante <codegp@icloud.com>                                         |
#|   Code licensed under the GNU GPL v3.0. See the LICENSE file for details.      |
#+--------------------------------------------------------------------------------+

# variables for colors
WHITE='\033[0;30m'
MAGENTA='\033[0;35m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'

# variables for cmd arguments
GUI_OPT=0
CLEAN_OPT=0
TARGET_OPT=0
INCREMENTAL_OPT=0
NOMAKE_OPT=0
BUILD_TARGET="Release"
GUI_FEATURE="OFF"
WEBUI_FEATURE="ON"
STACKTRACE_FEATURE="ON"
DBUS_FEATURE="ON"
TESTING_FEATURE="OFF"
VERBOSE_CONFIGURE_FEATURE="OFF"
SYSTEMD_FEATURE="OFF"

SCRIPT_PATH=$(realpath "$BASH_SOURCE")
SCRIPT_DIR=$(dirname "$SCRIPT_PATH")

tmp_root=$(pushd "$SCRIPT_DIR/.." | awk '{print $1}')
ROOT_DIR=$(eval echo "$tmp_root")
CMAKELIST_FILE="$ROOT_DIR/CMakeLists.txt"
ROOT_DIRECTORY="$ROOT_DIR"
SCRIPT_DIR="$ROOT_DIR/scripts"
LOGS_DIR="$ROOT_DIR/logs"
LOG_FILE="$LOGS_DIR/build.log"
VERSION_FILE=$ROOT_DIR/version.nfo
BUILD_FILE=$ROOT_DIR/build.nfo
VERSION_SCRIPT=$SCRIPT_DIR/update_version.sh
BIN_OUT="$ROOT_DIR/build/qbittorrent-nox"

if [[ ! -f "$CMAKELIST_FILE" ]]; then
   echo "[error] missing CMakeList.txt file @ \"$CMAKELIST_FILE\"!"
   exit 1
fi

# Usage function
usage() {
    echo "Usage: $0 [options]"  
    echo "  -t, --target <Debug|Release>    Build target"
    echo "  -c, --clean                     Clean"
    echo "  -g, --gui                       Enable GUI"
    echo "  -n, --nomake                    No make, just configure"
    echo "  -h, --help                      Show this help message"
    exit 0
}

# =========================================================
# function:     logs functions
# description:  log messages to fils and console
# =========================================================

log_info() {
    if [[ -f "$LOG_FILE" ]]; then
        echo "[$(date)] $1" >> "$LOG_FILE"
    fi
    echo -e "${BLUE}[log]${NC} $1"
}
log_warn() {
    if [[ -f "$LOG_FILE" ]]; then
        echo "[$(date)] $1" >> "$LOG_FILE"
    fi
    echo -e "${MAGENTA}[warn]${NC} ${WHITE}$1${NC}"
}
log_error() {
    if [[ -f "$LOG_FILE" ]]; then
        echo "[$(date)] $1" >> "$LOG_FILE"
    fi
    echo -e "${RED}[error]${NC} ${YELLOW}$1${NC}"
    popd > /dev/null
    exit 1
}


# Parse script arguments
for arg in "$@"; do
    case $arg in
        -g|--gui)
            GUI_OPT=1
            GUI_FEATURE="ON"
            ;;
        -t|--target)
            TARGET_OPT=1
            shift # Shift past the version
            if [[ $# -gt 1 ]]; then
                TMP_TARGET="$2"
                if [[ "$TMP_TARGET" == "Debug" || "$TMP_TARGET" == "Release" ]]; then
                    BUILD_TARGET="$TMP_TARGET"
                else
                    log_error "invalid build target, use debug or release"
                fi
            fi
            ;;
        -c|--clean)
            CLEAN_OPT=1
            ;;
        -n|--nomake)
            NOMAKE_OPT=1
            ;;
        -d|--debug)
            TARGET_OPT=1
            BUILD_TARGET="Debug"
            ;;
        -r|--release)
            TARGET_OPT=1
            BUILD_TARGET="Release"
            ;;
        -h|--help)
            usage
            ;;
    esac
done



pushd $ROOT_DIR > /dev/null



if ! which cmake; then
    log_error "missing cmake"
fi

log_info "======================================================"
log_info "BUILD_TARGET          $BUILD_TARGET"
log_info "GUI                   $GUI_FEATURE"
log_info "WEBUI                 $WEBUI_FEATURE"
log_info "STACKTRACE            $STACKTRACE_FEATURE"
log_info "DBUS                  $DBUS_FEATURE"
log_info "TESTING               $TESTING_FEATURE"
log_info "VERBOSE_CONFIGURE     $VERBOSE_CONFIGURE_FEATURE"
log_info "SYSTEMD               $SYSTEMD_FEATURE"
log_info "PARALLEL              $(nproc)"
log_info "======================================================"


if [[ $CLEAN_OPT -eq 1 ]]; then
    make clean
    rm -rf "$ROOT_DIR/CMakeCache.txt" "$ROOT_DIR/CMakeFiles"
    rm -rf "$ROOT_DIR/build"
    exit 0
fi


cmake -B build -DCMAKE_BUILD_TYPE=$BUILD_TARGET -DGUI=$GUI_FEATURE -DWEBUI=$WEBUI_FEATURE -DSTACKTRACE=$STACKTRACE_FEATURE -DDBUS=$DBUS_FEATURE -DTESTING=$TESTING_FEATURE -DVERBOSE_CONFIGURE=$VERBOSE_CONFIGURE_FEATURE -DSYSTEMD=$SYSTEMD_FEATURE

if [[ $? -ne 0 ]]; then
    log_error "cmake error"
fi



if [[ $NOMAKE_OPT -eq 1 ]]; then
    log_info "nomake: EXIT!"
    exit 0
fi

$VERSION_SCRIPT

cmake --build build --parallel $(nproc)
if [[ $? -ne 0 ]]; then
    log_error "cmake error"
fi

popd > /dev/null

log_info "SUCCESS!"

if [[ -f "$BIN_OUT" ]]; then
    log_info "Binary output: $BIN_OUT"
fi
