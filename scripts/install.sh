#!/bin/bash

#+--------------------------------------------------------------------------------+
#|                                                                                |
#|   install.sh                                                                  |
#|                                                                                |
#+--------------------------------------------------------------------------------+
#|   Guillaume Plante <codegp@icloud.com>                                         |
#|   Code licensed under the GNU GPL v3.0. See the LICENSE file for details.      |
#+--------------------------------------------------------------------------------+

# variables for colors
WHITE='\033[0;97m'
MAGENTA='\033[0;35m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color
GREEN='\033[0;32m'

# variables for cmd arguments
PATH_OPT=0
BIN_OPT=0
QTPATH_OPT=0

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
EXPORTLIBS_SCRIPT=$SCRIPT_DIR/export-libs.sh
QBITTORRENT_DIRECTORY="$ROOT_DIR/qBittorrent"
QBITTORRENT_BUILD_DIRECTORY="$QBITTORRENT_DIRECTORY/build"
QBITTORRENT_EXE_PATH="$QBITTORRENT_BUILD_DIRECTORY/qbittorrent-nox"


pushd $ROOT_DIR > /dev/null


VERSION_NUM=$(cat $VERSION_FILE)

# default
BIN_PATH="$ROOT_DIR/build/qbittorrent-nox"
INSTALL_PATH="$ROOT_DIR/install"
QT6_PATH="$QT6_ROOT_LINUX"

if [[ ! -f "$CMAKELIST_FILE" ]]; then
   echo "[error] missing CMakeList.txt file @ \"$CMAKELIST_FILE\"!"
   exit 1
fi

# Usage function
usage() {
    echo "Usage: $0 [options]"  
    echo "  -p, --path <install path>    Install to this path"
    echo "  -q, --qt-path <qt path>      QT PATH"
    echo "  -b, --bin <binary path>      Install this binary"
    echo "  -d, --install-ctdeploy       Install ctdeploy"
    echo "  -h, --help                   Show this help message"
    exit 0
}

# =========================================================
# function:     logs functions
# description:  log messages to fils and console
# =========================================================
log_ok() {
    echo -e " ✔️ ${WHITE}$1${NC}"
}
log_info() {
    echo -e " ➡️ ${WHITE}$1${NC}"
}
log_warn() {
    echo -e " ⚠️ ${YELLOW}$1${NC}"
}
log_error() {
    CAT="error"
    if [[ "$2" != "" ]]; then
        CAT="$2"
    fi
    echo -e " ⛔ ${RED}[$CAT]${NC} ${YELLOW}$1${NC}"
    popd > /dev/null
    exit 1
}



check_qt6_env() {
    if [[ -z "$QT6_PLUGINS_PATH" || -z "$QT6_BIN_DIR" || -z "$QT6_LIB_DIR" ]]; then
        log_info " ⛔  ⛔  ⛔  Qt6 is not installed or QT6_ROOT is not set  ⛔  ⛔  ⛔  "
        log_info "Please install Qt6 and export the following variable:"
        log_info ""
        log_info "QT_ROOT=\"\PATH/TO/YOUR/QT/LALALA/Qt\" # --- THIS IS ME, BUT USE YOUR OWN PATH"
        log_info "QT_CREATOR_PATH=\"\$QT_ROOT/Tools/QtCreator/bin\""
        log_info "QT6_VERSION=\"6.8.2\""
        log_info "QT6_ROOT_LINUX=\"\$QT_ROOT/\$QT6_VERSION/gcc_64\""
        log_info "QT6_BIN_DIR=\"\$QT6_ROOT_LINUX/bin\""
        log_info "QT6_LIB_DIR=\"\$QT6_ROOT_LINUX/lib\""
        log_info "QT6_PLUGINS_DIR=\"\$QT6_ROOT_LINUX/plugins\""
        log_info "Qt6_DIR=\"\$QT6_ROOT_LINUX/lib/cmake/Qt6\""
        log_info "Or run with '--qt-path' ./scripts/install.sh --qt-path /home/gp/Qt/6.8.2/gcc_64/"
        exit 1
    fi
}




download_and_verify() {
    local url="$1"
    local filename="/tmp/$(basename "$url")"
    local checksum_url="${url}.md5"
    local mywget=$(which wget)


    if [[ $? -ne 0 ]]; then
        echo "cannot find wget"
        exit 1
    fi
    rm -rf "$filename"

    log_info "Download the file..."
    $mywget --show-progress --max-redirect 5 --method=GET --timeout=5 --no-check-certificate --quiet -O "$filename" "$url"
    
    log_info "Get the checksum"
    local expected_checksum
    local expected_checksum=$($mywget --max-redirect 5 --method=GET --timeout=5 --no-check-certificate --quiet -O /dev/stdout "$checksum_url") || { echo "Error fetching checksum"; return 1; }

    # Verify the checksum
    local actual_checksum
    actual_checksum=$(md5sum "$filename" | awk '{print $1}')

    if [[ "$expected_checksum" != "$actual_checksum" ]]; then
        log_error "Checksum verification failed! $expected_checksum vs $actual_checksum"
    fi

    log_ok "Checksum verified successfully."

    # If the file ends with .deb and user is root, install it
    if [[ "$filename" == *.deb && "$(id -u)" -eq 0 ]]; then
        dpkg -i "$filename" || { log_error "Installation failed"; }
        log_ok "Package installed successfully."
    fi
    if [[ "$filename" == *.run && "$(id -u)" -eq 0 ]]; then
        chmod +x "$filename"
        "$filename" || { log_error "Installation failed";}
        log_ok "Package installed successfully."
    fi
}





# Parse script arguments
for arg in "$@"; do
    case $arg in
        -b|--bin)
            BIN_OPT=1
            shift # Shift past the version
            if [[ $# -ge 1 ]]; then
                TMP_BIN_PATH="$2"
                if [[ ! -f "$TMP_BIN_PATH" ]]; then
                    log_error "Binary file doesnt exists! $TMP_BIN_PATH"
                fi
                BIN_PATH="$TMP_BIN_PATH"
            fi
            ;;
        -q|--qt-path)
            log_info "QT_PATH OPTION"

            QTPATH_OPT=1
            shift # Shift past the version
            if [[ $# -ge 1 ]]; then
                TMP_QT_PATH="$1"
                log_info "QT_PATH OPTION"
                if [[ ! -e "$TMP_QT_PATH" ]]; then
                    log_error "Qt path doesnt exists!"
                else
                    export QT6_BIN_DIR="$TMP_QT_PATH/bin"
                    export QT6_LIB_DIR="$TMP_QT_PATH/lib"
                    export QT6_PLUGINS_PATH="$TMP_QT_PATH/plugins"
                    export QT6_PLUGINS_TLS_PATH="$QT_PLUGINS_PATH/tls"
                    log_info "export QT6_LIB_DIR           $QT6_LIB_DIR"
                    log_info "export QT6_BIN_DIR           $QT6_BIN_DIR"
                    log_info "export QT6_PLUGINS_PATH      $QT6_PLUGINS_PATH"
                fi
            fi
            ;;
        -p|--path)
            PATH_OPT=1
            shift # Shift past the version
            if [[ $# -ge 1 ]]; then
                TMP_PATH="$2"
                if [[ -e "$TMP_PATH" ]]; then
                    log_error "Install path already exists!"
                else
                    INSTALL_PATH="$TMP_PATH"
                fi
            fi
            ;;
        -d|--install-ctdeploy)
            if [[ "$(id -u)" -ne 0 ]]; then
                log_error "run as root"
            else
                download_and_verify "https://github.com/QuasarApp/CQtDeployer/releases/download/v1.6.2365/CQtDeployer_1.6.2365.7cce7f3_Linux_x86_64.deb"
            fi
            ;;
        -h|--help)
            usage
            ;;
    esac
done

check_qt6_env

CTDEPLOY_PATH=$(which cqtdeployer)

if [[ $? -ne 0 ]]; then
    log_error "cannot find cqtdeployer. Run again with sudo and '-d, --install-ctdeploy' "
fi

QMAKE_PATH=$(which qmake)

if [[ $? -ne 0 ]]; then
    log_error "cannot find qake"
fi



TMPLIBS_DIRECTORY="$ROOT_DIR/tmp/libs"

log_info "======================================================"
log_info "INSTALL_PATH          $INSTALL_PATH"
log_info "BIN_PATH              $BIN_PATH"
log_info "CTDEPLOY_PATH         $CTDEPLOY_PATH"
log_info "EXPORTLIBS_SCRIPT     $EXPORTLIBS_SCRIPT"
log_info "VERSION_NUM           $VERSION_NUM"
log_info "TMPLIBS_DIRECTORY     $TMPLIBS_DIRECTORY"
log_info "QT6_LIB_DIR           $QT6_LIB_DIR"
log_info "QT6_BIN_DIR           $QT6_BIN_DIR"
log_info "QT6_PLUGINS_TLS_PATH  $QT6_PLUGINS_PATH/tls"
log_info "======================================================"



rm -rf "$TMPLIBS_DIRECTORY"
rm -rf "$INSTALL_PATH"

$EXPORTLIBS_SCRIPT "$BIN_PATH" -p "$TMPLIBS_DIRECTORY"


if [[ $? -ne 0 ]]; then
    log_error "export libs"
fi

log_ok "Export libs"

log_info "DEPLOY ALL DEPS"

$CTDEPLOY_PATH -qmake "$QMAKE_PATH" -bin "$BIN_PATH" -libDir "$TMPLIBS_DIRECTORY" -extraPlugin "$QT6_PLUGINS_PATH/tls" -targetDir "$INSTALL_PATH"

popd > /dev/null


NEW_PATH=$(du -sh "$INSTALL_PATH" | awk '{ print $2 }')
SIZE=$(du -sh "$INSTALL_PATH" | awk '{ print $1 }')

log_ok "SUCCESS! $NEW_PATH $SIZE"
log_ok "You can now create a package from $INSTALL_PATH"