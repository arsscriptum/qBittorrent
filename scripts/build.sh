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
ASYNC_OPT=0
CLEAN_OPT=0
DEBUG_OPT=0
RUN_OPT=0
INCREMENTAL_OPT=0

UPDATE_VERSION_OPT=0
DEPLOYIMAGE_OPT=0
GETVERSION_OPT=0

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

$VERSION_SCRIPT

pushd $ROOT_DIR > /dev/null

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


if ! which cmake; then
    log_error "missing cmake"
fi


cmake -B build -DCMAKE_BUILD_TYPE=Release -DGUI=OFF

if [[ $? -ne 0 ]]; then
    log_error "cmake error"
fi

cmake --build build --parallel $(nproc)
if [[ $? -ne 0 ]]; then
    log_error "cmake error"
fi

popd > /dev/null

log_info "SUCCESS!"

if [[ -f "$BIN_OUT" ]]; then
    log_info "Binary output: $BIN_OUT"
fi
