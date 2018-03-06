#! /usr/bin/env bash
# This script can be used to generate fat static libraries for ios from any c/c++ build system
# The output flat libraries of lipo is in PREFIX/lipo/output
#
# Requirements for build system: configure at PROJ_ROOT
# Requirements for placement of this script: PROJ_ROOT/<any script dir name>/<name of this script>
#

set -e

cmd_name=`which $0`
if [ "x`echo $cmd_name | grep -e '^/'`" = "x" ] ; then
  script_path=`pwd`/$cmd_name
else
  script_path=$cmd_name
fi
script_dir=`dirname "${script_path}"`


PROJ_ROOT=`dirname "${script_dir}"`
BUILD_DIR=${PROJ_ROOT}/build
CONFIGURE=${PROJ_ROOT}/configure

LIPO_INPUT_DIR=${BUILD_DIR}/lipo/input
LIPO_OUTPUT_DIR=${BUILD_DIR}/lipo/output

PREFIX=$BUILD_DIR
LIB_DIR=${PREFIX}/lib

[ -d "${BUILD_DIR}" ] && rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"


do_build() {
  export CC="$(xcrun -find -sdk ${SDK} clang)"
  export CXX="$(xcrun -find -sdk ${SDK} clang)"
  export CPP="$(xcrun -find -sdk ${SDK} cpp)" #fixme: cannot put clang here?
  export CFLAGS="${HOST_FLAGS} ${OPT_FLAGS}"
  export CXXFLAGS="${HOST_FLAGS} ${OPT_FLAGS}"
  export LDFLAGS="${HOST_FLAGS}"
  "${CONFIGURE}" --host=${CHOST} --prefix=${PREFIX} --enable-static --disable-shared
  make && make install && make clean
}

do_collect_lib() {
  lipo_arch_dir=${LIPO_INPUT_DIR}/${ARCH}
  [ -d "${LIPO_INPUT_DIR}" ] || mkdir -p "${LIPO_INPUT_DIR}"
  mv "${LIB_DIR}" "${lipo_arch_dir}"
}

do_lipo() {
  arch_dir_names=(`ls "${LIPO_INPUT_DIR}"`)
  any_arch_dir=${LIPO_INPUT_DIR}/${arch_dir_names[1]}
  libs=`find "${any_arch_dir}" -name "*.a"`
  prefix_len=${#any_arch_dir}

  for lib in $libs ; do
    relative_lib_name=${lib:${prefix_len}}
    input_list=""
    for arch in ${arch_dir_names[*]} ; do
      input_list="${input_list} ${LIPO_INPUT_DIR}/${arch}/${relative_lib_name}"
    done

    output_lib="${LIPO_OUTPUT_DIR}/${relative_lib_name}" 
    output_lib_dir=`dirname "${output_lib}"`
    [ -d "${output_lib_dir}" ] || mkdir -p "${output_lib_dir}"
    lipo -create -output "${output_lib}" $input_list
  done
}

calc_cross_compile_flags() {
  echo "-arch ${ARCH} -miphoneos-version-min=8.0 -isysroot `xcrun -sdk ${SDK} --show-sdk-path`"
}


SDK_ARCH_HOSTS0=(iphoneos armv7 arm-apple-darwin)
SDK_ARCH_HOSTS1=(iphoneos armv7s arm-apple-darwin)
SDK_ARCH_HOSTS2=(iphoneos arm64 arm-apple-darwin)
SDK_ARCH_HOSTS3=(iphonesimulator i386 i386-apple-darwin)
SDK_ARCH_HOSTS4=(iphonesimulator x86_64 x86_64-apple-darwin)

cd "${BUILD_DIR}"

for ii in 0 1 2 3 4 ; do
  eval SDK=\${SDK_ARCH_HOSTS${ii}[0]}
  eval ARCH=\${SDK_ARCH_HOSTS${ii}[1]}
  eval CHOST=\${SDK_ARCH_HOSTS${ii}[2]}
  HOST_FLAGS=`calc_cross_compile_flags`
  do_build
  do_collect_lib
done
do_lipo

