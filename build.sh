#!/bin/bash -e

ROOT_DIR=$(dirname $(readlink -f $0))
BUILD_DIR=${ROOT_DIR}/.build-release
MACOS_DEPLOYMENT_TARGET=${MACOS_DEPLOYMENT_TARGET:-13.0}
OS_NAME="$(uname -s)"

. ${ROOT_DIR}/scripts/functions.rc

INFO "Building CodeLite"

function check_prerequistes() {
  if [[ "${OS_NAME}" == *MINGW* ]]; then
    install_prerequistes_MSW
    install_wx_config_MSW
  fi

  INFO "Checking build prerequisites"
  local missing=0
  local compiler

  if [[ "${OS_NAME}" == "Linux" ]]; then
    compiler="g++"
  else
    compiler="clang++"
  fi

  for tool in ${compiler} cmake git; do
    if command -v "${tool}" >/dev/null 2>&1; then
      INFO "Found ${tool}: $(command -v ${tool})"
    else
      ERROR "Missing required tool: ${tool}"
      missing=1
    fi
  done

  if [ "${missing}" -ne 0 ]; then
    ERROR "Please install the missing prerequisites and try again"
    exit 1
  fi
}

function install_prerequistes_MSW() {
  local marker="${BUILD_DIR}/.msys_packages_installed"
  if [ -f "${marker}" ]; then
    INFO "MSYS2 packages already installed; skipping"
    return 0
  fi

  INFO "Installing MSYS2 packages for ${MSYS_ARCH}..."
  mkdir -p ${BUILD_DIR}
  pacman -S --needed --noconfirm --quiet \
    git \
    openssh \
    mingw-w64-${MSYS_ARCH}-toolchain \
    mingw-w64-${MSYS_ARCH}-python3 \
    mingw-w64-${MSYS_ARCH}-cmake \
    mingw-w64-${MSYS_ARCH}-libffi \
    mingw-w64-${MSYS_ARCH}-jq \
    mingw-w64-${MSYS_ARCH}-libxml2 \
    mingw-w64-${MSYS_ARCH}-llvm-openmp \
    mingw-w64-${MSYS_ARCH}-ntldd \
    unzip \
    mingw-w64-${MSYS_ARCH}-zlib \
    mingw-w64-${MSYS_ARCH}-libssh \
    mingw-w64-${MSYS_ARCH}-hunspell \
    mingw-w64-${MSYS_ARCH}-openssl \
    mingw-w64-${MSYS_ARCH}-sqlite3 \
    mingw-w64-${MSYS_ARCH}-libmariadbclient \
    mingw-w64-${MSYS_ARCH}-postgresql \
    mingw-w64-${MSYS_ARCH}-ctags \
    flex bison patch 2>${BUILD_DIR}/msys_install_packages_err.log
  touch "${marker}"
}

function install_wx_config_MSW() {
  INFO "Building wx-config for Windows"
  local wx_config_install_dir=${BUILD_DIR}/wx-config-msys2-install
  if [ -f "${wx_config_install_dir}/bin/wx-config.exe" ]; then
    INFO "${wx_config_install_dir}/bin/wx-config.exe exists; skipping"
    export PATH=${wx_config_install_dir}/bin:$PATH
    return 0
  fi
  mkdir -p ${BUILD_DIR}
  cd $_
  git clone --depth 1 https://github.com/eranif/wx-config-msys2.git
  cd wx-config-msys2
  mkdir .build-release
  cd $_
  cmake .. -DCMAKE_BUILD_TYPE=Release -G"MinGW Makefiles" -DCMAKE_INSTALL_PREFIX="${wx_config_install_dir}"
  mingw32-make -j$(nproc) install
  export PATH=${wx_config_install_dir}/bin:$PATH
}

function build_wx_widgets_Linux() {
  local wx_install_dir=${BUILD_DIR}/wxWidgets-install
  if [ -x "${wx_install_dir}/bin/wx-config" ]; then
    INFO "wxWidgets already built at ${wx_install_dir}; skipping"
    export PATH="${wx_install_dir}/bin":$PATH
    return 0
  fi

  INFO "Building wxWidgets"
  mkdir -p ${BUILD_DIR}
  cd $_
  git clone --depth 1 https://github.com/wxWidgets/wxWidgets.git
  cd wxWidgets
  git submodule update --init --depth 1
  mkdir .build-release
  cd .build-release
  ../configure --disable-debug_flag --with-gtk=3 --enable-stl --prefix=${wx_install_dir}
  make -j$(nproc) install
  export PATH="${wx_install_dir}/bin":$PATH
  cd ${ROOT_DIR}
}

function build_wx_widgets_macOS() {
  local wx_install_dir=${BUILD_DIR}/wxWidgets-install
  if [ -x "${wx_install_dir}/bin/wx-config" ]; then
    INFO "wxWidgets already built at ${wx_install_dir}; skipping"
    export PATH="${wx_install_dir}/bin":$PATH
    return 0
  fi

  INFO "Building wxWidgets"
  mkdir -p ${BUILD_DIR}
  cd $_
  git clone --depth 1 https://github.com/wxWidgets/wxWidgets.git
  cd wxWidgets
  git submodule update --init --depth 1
  mkdir .build-release
  cd .build-release
  cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=${MACOS_DEPLOYMENT_TARGET} \
    -DwxBUILD_DEBUG_LEVEL=0 \
    -DwxBUILD_MONOLITHIC=1 \
    -DwxBUILD_SAMPLES=OFF \
    -DwxUSE_SYS_LIBS=OFF \
    -DwxUSE_LUNASVG=OFF \
    -DCMAKE_INSTALL_PREFIX=${BUILD_DIR}/wxWidgets-install
  make -j$(sysctl -n hw.physicalcpu) install
  export PATH="${wx_install_dir}/bin":$PATH
  cd ${ROOT_DIR}
}

function build_wx_widgets_MSW() {
  local wx_install_dir=${BUILD_DIR}/wxWidgets-install

  if ls ${wx_install_dir}/lib/clang*/wxmsw*.dll >/dev/null 2>&1; then
    INFO "wxWidgets DLLs already found in ${wx_install_dir}; skipping build"
    return 0
  fi

  INFO "Building wxWidgets"
  mkdir -p ${BUILD_DIR}
  cd $_
  rm -fr wxWidgets # in case we aborted earlier
  git clone --depth 1 https://github.com/wxWidgets/wxWidgets.git
  cd wxWidgets
  git submodule update --init --depth 1
  mkdir .build-release
  cd .build-release
  cmake .. -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release \
    -DwxBUILD_DEBUG_LEVEL=0 \
    -DwxBUILD_MONOLITHIC=1 -DwxBUILD_SAMPLES=OFF -DwxUSE_STL=ON \
    -DCMAKE_TLS_VERIFY=OFF \
    -DCMAKE_INSTALL_PREFIX=${BUILD_DIR}/wxWidgets-install

  make -j$(nproc) install
  export WXWIN="${BUILD_DIR}/wxWidgets-install"
  INFO "WXWIN is set to '${WXWIN}'"
  cd ${ROOT_DIR}
}

function build_CodeLite_Linux() {
  INFO "Building CodeLite"
  local wx_install_dir=${BUILD_DIR}/wxWidgets-install
  local wx_config=${wx_install_dir}/bin/wx-config
  if [ ! -x "${wx_config}" ]; then
    ERROR "Local wx-config not found at ${wx_config}"
    exit 1
  fi
  INFO "Using local wx-config: ${wx_config}"

  local CodeLite_build_dir=${ROOT_DIR}/.build-release
  mkdir -p ${CodeLite_build_dir}
  cd ${CodeLite_build_dir}
  if [ ! -f "${CodeLite_build_dir}/CMakeCache.txt" ] ||
    [ "${ROOT_DIR}/CMakeLists.txt" -nt "${CodeLite_build_dir}/Makefile" ]; then
    INFO "Configuring CodeLite"
    cmake ${ROOT_DIR} -DCMAKE_BUILD_TYPE=Release -DMAKE_DEB=1 -DCOPY_WX_LIBS=1 \
       -DWITH_WX_CONFIG=${wx_config}
  else
    INFO "CodeLite already configured; skipping cmake"
  fi
  make -j$(nproc) #VERBOSE=1
  INFO "CodeLite built successfully"
  cd ${ROOT_DIR}

  INFO ""
  INFO "To run CodeLite:"
  INFO "=============="
  INFO "${BUILD_DIR}/install/bin/codelite"
  INFO ""
}

function build_CodeLite_macOS() {
  INFO "Building CodeLite"
  local wx_install_dir=${BUILD_DIR}/wxWidgets-install
  local wx_config=${wx_install_dir}/bin/wx-config
  if [ ! -x "${wx_config}" ]; then
    ERROR "Local wx-config not found at ${wx_config}"
    exit 1
  fi
  INFO "Using local wx-config: ${wx_config}"

  local CodeLite_build_dir=${ROOT_DIR}/.build-release
  mkdir -p ${CodeLite_build_dir}
  cd ${CodeLite_build_dir}
  # Configure if the build tree has not been generated yet, or if
  # CMakeLists.txt is newer than the generated cache.
  if [ ! -f "${CodeLite_build_dir}/CMakeCache.txt" ] ||
    [ "${ROOT_DIR}/CMakeLists.txt" -nt "${CodeLite_build_dir}/Makefile" ] ||
    ! grep -q "^CMAKE_OSX_DEPLOYMENT_TARGET:STRING=${MACOS_DEPLOYMENT_TARGET}$" "${CodeLite_build_dir}/CMakeCache.txt"; then
    INFO "Configuring CodeLite"
    cmake ${ROOT_DIR} -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_OSX_DEPLOYMENT_TARGET=${MACOS_DEPLOYMENT_TARGET} \
      -DwxWidgets_CONFIG_EXECUTABLE=${wx_config}
  else
    INFO "CodeLite already configured; skipping cmake"
  fi
  make -j$(sysctl -n hw.physicalcpu) install
  INFO "CodeLite built successfully"
  cd ${ROOT_DIR}

  INFO ""
  INFO "To run CodeLite:"
  INFO "=============="
  INFO "open ${BUILD_DIR}/codelite.app"
  INFO ""
}

function build_CodeLite_MSW() {
  INFO "Building CodeLite"
  local CodeLite_build_dir=${ROOT_DIR}/.build-release
  mkdir -p ${CodeLite_build_dir}
  cd ${CodeLite_build_dir}
  # Configure if the build tree has not been generated yet, or if
  # CMakeLists.txt is newer than the generated cache.
  if [ ! -f "${CodeLite_build_dir}/CMakeCache.txt" ] || [ ! -d "${CodeLite_build_dir}/install" ] ||
    [ "${ROOT_DIR}/CMakeLists.txt" -nt "${CodeLite_build_dir}/Makefile" ]; then
    INFO "Configuring CodeLite"
    rm -f ${CodeLite_build_dir}/CMakeCache.txt
    cmake ${ROOT_DIR} -DCMAKE_BUILD_TYPE=Release -DWXWIN="${BUILD_DIR}/wxWidgets-install"
  else
    INFO "CodeLite already configured; skipping cmake"
  fi
  make -j$(nproc) install
  INFO "CodeLite built successfully"
  cd ${ROOT_DIR}

  INFO ""
  INFO "To run CodeLite:"
  INFO "=============="
  INFO ""
  INFO "(cd ${BUILD_DIR}/install/bin && ./codelite.exe)"
  INFO ""
}

function clean() {
  if [ ! -d "${BUILD_DIR}" ]; then
    INFO "Nothing to clean"
    return 0
  fi
  INFO "Cleaning build artifacts"
  make -C "${BUILD_DIR}" clean
  INFO "Clean complete"
}

function distclean() {
  INFO "Removing build directory: ${BUILD_DIR}"
  rm -rf "${BUILD_DIR}"
  INFO "Distclean complete"
}

function usage() {
  echo "Usage: $(basename $0) [target]"
  echo ""
  echo "Targets:"
  echo "  (none)      Build CodeLite (default)"
  echo "  clean       Remove build artifacts (make clean)"
  echo "  distclean   Remove the entire build directory"
  echo "  package     Create an installer suitable for the current platform"
  echo "  -h, --help  Show this help message"
}

function build() {
  if [[ "${OS_NAME}" == *MINGW* ]]; then
    INFO "On Windows"
    build_wx_widgets_MSW
    build_CodeLite_MSW
  elif [[ "${OS_NAME}" == "Darwin" ]]; then
    INFO "On macOS"
    build_wx_widgets_macOS
    build_CodeLite_macOS
  elif [[ "${OS_NAME}" == "Linux" ]]; then
    INFO "On Linux"
    build_wx_widgets_Linux
    build_CodeLite_Linux
  else
    ERROR "Unsupported operating system: ${OS_NAME}"
    exit 1
  fi
}

function package() {
  build
  if [[ "${OS_NAME}" == *MINGW* ]]; then
    cd ${BUILD_DIR}
    make -j$(nproc) setup/fast
  elif [[ "${OS_NAME}" == "Darwin" ]]; then
    cd ${BUILD_DIR}

    if [ ! -d "CodeLite.app" ]; then
      ERROR "CodeLite.app not found in ${BUILD_DIR}"
      exit 1
    fi

    if [ -z "$CODELITE_PASSWORD" ]; then
      ERROR "CODELITE_PASSWORD environment variable is not set"
      exit 1
    fi

    rm -f *.zip
    ../macos-sign-app.sh --notarize --password $CODELITE_PASSWORD codelite.app
  elif [[ "${OS_NAME}" == "Linux" ]]; then
    INFO "Creating Linux package"
    cd ${BUILD_DIR}
    make -j$(nproc) package
  fi
}

case "${1}" in
clean)
  clean
  exit 0
  ;;
distclean)
  distclean
  exit 0
  ;;
package)
  package
  exit 0
  ;;
-h | --help)
  usage
  exit 0
  ;;
"") ;;
*)
  ERROR "Unknown target: ${1}"
  usage
  exit 1
  ;;
esac

check_prerequistes
build
