#!/bin/bash -e

ROOT_DIR=$(dirname $(readlink -f $0))
BUILD_TARGET=Release
BUILD_DIR_NAME=.build-release
BUILD_DIR=${ROOT_DIR}/${BUILD_DIR_NAME}
OS_NAME="$(uname -s)"
# MSYS2 (MinGW) provides mingw32-make; its CMake generates "MinGW Makefiles"
if [[ "${OS_NAME}" == *MINGW* ]]; then
  MAKE_CMD=mingw32-make
  # Without this, CMake picks Ninja (when available), but we build with mingw32-make
  export CMAKE_GENERATOR="MinGW Makefiles"
else
  MAKE_CMD=make
fi
FORCE_CMAKE=0
WITH_TESTS=0
NO_BUILD=0
WX_VERSION=v3.3.3.1
WXCRAFTER_BUILD_DIR_NAME=.build-release-wxcrafter
WXCRAFTER_BUILD_DIR=${ROOT_DIR}/${WXCRAFTER_BUILD_DIR_NAME}

. ${ROOT_DIR}/scripts/functions.rc

INFO "Building CodeLite"

function check_cmake_modified() {
  local build_dir=$1
  find "${ROOT_DIR}" \
    -not -path "*/.git/*" \
    -not -path "*/.build*/*" \
    -not -path "*/.codelite*/*" \
    -not -path "*/.vscode/*" \
    -not -path "*/.idea/*" \
    -not -path "*/node_modules/*" \
    -name "CMakeLists.txt" \
    -newer "${build_dir}/Makefile" \
    -print -quit | grep -q .
}

function run_make() {
  if [ "${NO_BUILD}" -eq 1 ]; then
    INFO "--no-build was passed; skipping: make $*"
    return 0
  fi
  ${MAKE_CMD} "$@"
}

function check_prerequistes() {
  if [[ "${OS_NAME}" == *MINGW* ]]; then
    install_prerequistes_MSW
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
    mingw-w64-${MSYS_ARCH}-glew \
    flex bison patch 2>${BUILD_DIR}/msys_install_packages_err.log || {
    cat "${BUILD_DIR}/msys_install_packages_err.log" >&2
    ERROR "Failed to install the MSYS2 packages"
    exit 1
  }
  touch "${marker}"
}

function build_wx_widgets_MSW() {
  local wx_install_dir=${BUILD_DIR}/wxWidgets-install
  # Windows only: CL_WX_VERSION (a wxWidgets tag or branch, e.g. v3.2.11) overrides the pinned version
  local wx_version=${CL_WX_VERSION:-${WX_VERSION}}
  local wx_version_file=${wx_install_dir}/.wx_version

  if ls ${wx_install_dir}/lib/clang*/wxmsw*.dll >/dev/null 2>&1; then
    local built_version=""
    [ -f "${wx_version_file}" ] && built_version=$(cat "${wx_version_file}")
    if [ -z "${built_version}" ] || [ "${built_version}" == "${wx_version}" ]; then
      INFO "wxWidgets DLLs already found in ${wx_install_dir} (version: ${built_version:-unknown}); skipping build"
      return 0
    fi
    INFO "wxWidgets ${built_version} was built, but ${wx_version} was requested; rebuilding"
    rm -fr "${wx_install_dir}"
    # The CodeLite and wxCrafter build trees hold values computed from the old wxWidgets (version, DLL name, ...),
    # remove their cache so they are configured again
    rm -f "${BUILD_DIR}/CMakeCache.txt" "${WXCRAFTER_BUILD_DIR}/CMakeCache.txt"
  fi

  INFO "Building wxWidgets"
  INFO "Checking out wxWidgets version: ${wx_version}"
  mkdir -p ${BUILD_DIR}
  cd $_
  rm -fr wxWidgets # in case we aborted earlier
  git clone --depth 1 --branch ${wx_version} https://github.com/wxWidgets/wxWidgets.git
  cd wxWidgets
  git submodule update --init --depth 1
  mkdir .build-release
  cd .build-release
  cmake .. -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release \
    -DwxBUILD_DEBUG_LEVEL=0 \
    -DwxBUILD_MONOLITHIC=1 -DwxBUILD_SAMPLES=OFF -DwxUSE_STL=ON \
    -DCMAKE_TLS_VERIFY=OFF \
    -DCMAKE_INSTALL_PREFIX=${BUILD_DIR}/wxWidgets-install

  ${MAKE_CMD} -j$(nproc) install
  echo "${wx_version}" >"${wx_version_file}"
  export WXWIN="${BUILD_DIR}/wxWidgets-install"
  INFO "WXWIN is set to '${WXWIN}'"
  cd ${ROOT_DIR}
}

function build_wx_widgets_Linux() {
  local wx_install_dir=${BUILD_DIR}/wxWidgets-install
  # Do not use v3.3.3 or newer on Linux for now: wxGTK clipboard copy (Ctrl-C) crashes WSLg's Weston
  local wx_version=v3.3.2
  if [ -x "${wx_install_dir}/bin/wx-config" ]; then
    INFO "wxWidgets already built at ${wx_install_dir}; skipping"
    export PATH="${wx_install_dir}/bin":$PATH
    return 0
  fi

  INFO "Building wxWidgets"
  INFO "Checking out wxWidgets version: ${wx_version}"
  rm -fr ${BUILD_DIR}/wxWidgets
  mkdir -p ${BUILD_DIR}
  cd $_
  git clone --depth 1 --branch ${wx_version} https://github.com/wxWidgets/wxWidgets.git
  cd wxWidgets
  git submodule update --init --depth 1
  mkdir ${BUILD_DIR_NAME}
  cd ${BUILD_DIR_NAME}
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
  INFO "Checking out wxWidgets version: ${WX_VERSION}"
  rm -fr ${BUILD_DIR}/wxWidgets
  mkdir -p ${BUILD_DIR}
  cd $_
  git clone --depth 1 --branch ${WX_VERSION} https://github.com/wxWidgets/wxWidgets.git
  cd wxWidgets
  git submodule update --init --depth 1
  mkdir ${BUILD_DIR_NAME}
  cd ${BUILD_DIR_NAME}
  cmake .. -DCMAKE_BUILD_TYPE=Release \
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

function build_CodeLite_Linux() {
  INFO "Building CodeLite"
  local wx_install_dir=${BUILD_DIR}/wxWidgets-install
  local wx_config=${wx_install_dir}/bin/wx-config
  if [ ! -x "${wx_config}" ]; then
    ERROR "Local wx-config not found at ${wx_config}"
    exit 1
  fi
  INFO "Using local wx-config: ${wx_config}"

  local CodeLite_build_dir=${ROOT_DIR}/${BUILD_DIR_NAME}
  mkdir -p ${CodeLite_build_dir}
  cd ${CodeLite_build_dir}
  if [ "${FORCE_CMAKE}" -eq 1 ] || [ ! -f "${CodeLite_build_dir}/CMakeCache.txt" ] ||
    check_cmake_modified "${CodeLite_build_dir}"; then
    INFO "Configuring CodeLite"
    rm -f ${CodeLite_build_dir}/CMakeCache.txt
    local buildTests=""
    if [ "${WITH_TESTS}" -eq 1 ]; then
      buildTests="-DBUILD_TESTING=1"
      INFO "Building with UT enabled"
    fi
    local installPrefix=""
    if [ "${BUILD_TARGET}" == "Debug" ]; then
      installPrefix="-DCMAKE_INSTALL_PREFIX=${HOME}/root"
    fi
    cmake ${ROOT_DIR} -DCMAKE_BUILD_TYPE=${BUILD_TARGET} -DMAKE_DEB=1 -DCOPY_WX_LIBS=1 \
      -DWITH_WX_CONFIG=${wx_config} ${buildTests} ${installPrefix}
  else
    INFO "CodeLite already configured; skipping cmake"
  fi

  run_make -j$(nproc) #VERBOSE=1
  if [ "${NO_BUILD}" -eq 1 ]; then
    cd ${ROOT_DIR}
    return 0
  fi
  INFO "CodeLite built successfully"
  cd ${ROOT_DIR}

  INFO ""
  INFO "To run CodeLite:"
  INFO "=============="
  if [ "${BUILD_TARGET}" == "Debug" ]; then
    INFO "cd ${BUILD_DIR} && make -j$(nproc) install"
    INFO "${HOME}/root/bin/codelite"
  else
    INFO "${BUILD_DIR}/bin/codelite"
  fi
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

  local CodeLite_build_dir=${ROOT_DIR}/${BUILD_DIR_NAME}
  mkdir -p ${CodeLite_build_dir}
  cd ${CodeLite_build_dir}
  # Configure if the build tree has not been generated yet, or if
  # CMakeLists.txt is newer than the generated cache.
  if [ "${FORCE_CMAKE}" -eq 1 ] || [ ! -f "${CodeLite_build_dir}/CMakeCache.txt" ] || [ ! -d "${CodeLite_build_dir}/codelite.app" ] ||
    check_cmake_modified "${CodeLite_build_dir}" ||
    ! grep -q "^CMAKE_OSX_DEPLOYMENT_TARGET:STRING=${MACOS_DEPLOYMENT_TARGET}$" "${CodeLite_build_dir}/CMakeCache.txt"; then
    INFO "Configuring CodeLite"
    rm -f ${CodeLite_build_dir}/CMakeCache.txt
    local buildTests=""
    if [ "${WITH_TESTS}" -eq 1 ]; then
      buildTests="-DBUILD_TESTING=1"
      INFO "Building with UT enabled"
    fi

    cmake ${ROOT_DIR} -DCMAKE_BUILD_TYPE=${BUILD_TARGET} \
      -DCMAKE_OSX_DEPLOYMENT_TARGET=${MACOS_DEPLOYMENT_TARGET} \
      -DwxWidgets_CONFIG_EXECUTABLE=${wx_config} ${buildTests}
  else
    INFO "CodeLite already configured; skipping cmake"
  fi
  run_make -j$(sysctl -n hw.physicalcpu) install
  if [ "${NO_BUILD}" -eq 1 ]; then
    cd ${ROOT_DIR}
    return 0
  fi
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
  local CodeLite_build_dir=${ROOT_DIR}/${BUILD_DIR_NAME}
  mkdir -p ${CodeLite_build_dir}
  cd ${CodeLite_build_dir}
  # Configure if the build tree has not been generated yet, or if
  # CMakeLists.txt is newer than the generated cache.
  if [ "${FORCE_CMAKE}" -eq 1 ] || [ ! -f "${CodeLite_build_dir}/CMakeCache.txt" ] || [ ! -d "${CodeLite_build_dir}/install" ] ||
    check_cmake_modified "${CodeLite_build_dir}"; then
    local buildTests=""
    if [ "${WITH_TESTS}" -eq 1 ]; then
      buildTests="-DBUILD_TESTING=1"
      INFO "Building with UT enabled"
    fi
    INFO "Configuring CodeLite"
    rm -f ${CodeLite_build_dir}/CMakeCache.txt
    cmake ${ROOT_DIR} -DCMAKE_BUILD_TYPE=${BUILD_TARGET} -DWXWIN="${BUILD_DIR}/wxWidgets-install" ${buildTests}
  else
    INFO "CodeLite already configured; skipping cmake"
  fi
  run_make -j$(nproc) install
  if [ "${NO_BUILD}" -eq 1 ]; then
    cd ${ROOT_DIR}
    return 0
  fi
  INFO "CodeLite built successfully"
  cd ${ROOT_DIR}

  INFO ""
  INFO "To run CodeLite:"
  INFO "=============="
  INFO ""
  INFO "(cd ${BUILD_DIR}/install/bin && ./codelite.exe)"
  INFO ""
}

function build_wxCrafter_MSW() {
  INFO "Building wxCrafter"

  # wxCrafter must be built against the exact same wxWidgets build used for CodeLite,
  # so make sure it exists first (this reuses ${BUILD_DIR}/wxWidgets-install; it does
  # NOT build a separate copy of wxWidgets for wxCrafter).
  build_wx_widgets_MSW
  local wx_install_dir=${BUILD_DIR}/wxWidgets-install

  mkdir -p ${WXCRAFTER_BUILD_DIR}
  cd ${WXCRAFTER_BUILD_DIR}
  # Configure if the build tree has not been generated yet, or if
  # CMakeLists.txt is newer than the generated cache.
  if [ "${FORCE_CMAKE}" -eq 1 ] || [ ! -f "${WXCRAFTER_BUILD_DIR}/CMakeCache.txt" ] || [ ! -d "${WXCRAFTER_BUILD_DIR}/install" ] ||
    check_cmake_modified "${WXCRAFTER_BUILD_DIR}"; then
    INFO "Configuring wxCrafter"
    rm -f ${WXCRAFTER_BUILD_DIR}/CMakeCache.txt
    cmake ${ROOT_DIR} -DCMAKE_BUILD_TYPE=${BUILD_TARGET} -DWXC_APP=1 -DWXWIN="${wx_install_dir}"
  else
    INFO "wxCrafter already configured; skipping cmake"
  fi
  run_make -j$(nproc) install
  if [ "${NO_BUILD}" -eq 1 ]; then
    cd ${ROOT_DIR}
    return 0
  fi
  INFO "wxCrafter built successfully"
  cd ${ROOT_DIR}

  INFO ""
  INFO "To run wxCrafter:"
  INFO "================="
  INFO ""
  INFO "(cd ${WXCRAFTER_BUILD_DIR}/install/bin && ./wxcrafter.exe)"
  INFO ""
}

function package_wxCrafter_MSW() {
  build_wxCrafter_MSW
  cd ${WXCRAFTER_BUILD_DIR}
  ${MAKE_CMD} -j$(nproc) setup
  cd ${ROOT_DIR}
}

function clean() {
  if [ ! -d "${BUILD_DIR}" ]; then
    INFO "Nothing to clean"
    return 0
  fi
  INFO "Cleaning build artifacts"
  ${MAKE_CMD} -C "${BUILD_DIR}" clean
  INFO "Clean complete"
}

function distclean() {
  INFO "Removing build directory: ${BUILD_DIR}"
  rm -rf "${BUILD_DIR}"
  INFO "Distclean complete"
}

function usage() {
  echo "Usage: $(basename $0) [options] [target]"
  echo ""
  echo "Targets:"
  echo "  (none)      Build CodeLite (default, release mode)"
  echo "  debug       Build CodeLite (debug mode)"
  echo "  clean       Remove build artifacts (make clean)"
  echo "  distclean   Remove the entire build directory"
  echo "  package     Create an installer suitable for the current platform"
  echo "  wxcrafter           Build wxCrafter (Windows only)"
  echo "  package_wxcrafter   Build wxCrafter and create an installer for it (Windows only)"
  echo ""
  echo "Options:"
  echo "  --cmake     Force the cmake configure stage even if it is up to date"
  echo "  --tests     Enable Tests"
  echo "  --no-build  Run cmake only, skip the build step (make)"
  echo ""
  echo "Environment:"
  echo "  CL_WX_VERSION   Windows only: the wxWidgets tag or branch to build (e.g. v3.2.11 or master)"
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
    ${MAKE_CMD} -j$(nproc) setup/fast
  elif [[ "${OS_NAME}" == "Darwin" ]]; then
    cd ${BUILD_DIR}

    if [ ! -d "codelite.app" ]; then
      ERROR "codelite.app not found in ${BUILD_DIR}"
      exit 1
    fi

    if [ -z "$CODELITE_PASSWORD" ]; then
      ERROR "CODELITE_PASSWORD environment variable is not set"
      exit 1
    fi

    rm -f *.zip
    ${ROOT_DIR}/scripts/weekly/macos-sign-app.sh --notarize --password $CODELITE_PASSWORD codelite.app
  elif [[ "${OS_NAME}" == "Linux" ]]; then
    INFO "Creating Linux package"
    cd ${BUILD_DIR}
    make -j$(nproc) package
  fi
}

TARGET=""

while [ $# -gt 0 ]; do
  case "${1}" in
  --cmake)
    FORCE_CMAKE=1
    ;;
  --tests)
    WITH_TESTS=1
    ;;
  --no-build)
    NO_BUILD=1
    ;;
  -h | --help)
    usage
    exit 0
    ;;
  clean | distclean | package | debug | wxcrafter | package_wxcrafter)
    if [ -n "${TARGET}" ]; then
      ERROR "Multiple targets specified: '${TARGET}' and '${1}'"
      usage
      exit 1
    fi
    TARGET="${1}"
    ;;
  *)
    ERROR "Unknown argument: ${1}"
    usage
    exit 1
    ;;
  esac
  shift
done

case "${TARGET}" in
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
wxcrafter | package_wxcrafter)
  if [[ "${OS_NAME}" != *MINGW* ]]; then
    ERROR "'${TARGET}' is only supported on Windows"
    exit 1
  fi
  check_prerequistes
  if [ "${TARGET}" == "wxcrafter" ]; then
    build_wxCrafter_MSW
  else
    package_wxCrafter_MSW
  fi
  exit 0
  ;;
debug)
  BUILD_TARGET=Debug
  BUILD_DIR_NAME=.build-debug
  BUILD_DIR=${ROOT_DIR}/${BUILD_DIR_NAME}
  INFO "Building CodeLite in DEBUG mode"
  ;;
"") ;;
esac

check_prerequistes
build
