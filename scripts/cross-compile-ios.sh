#! /bin/sh

get_default_sdk() {
  platform=$1
  default_one=`xcrun -find -sdk ${platform} --show-sdk-path`
  name=`basename "${default_one}"`
  version=$(echo "${name}" | sed -E 's/^[^0-9]+//' | sed -E 's/[^0-9]+$//')
  echo ${platform}${version}
}

get_default_ios_sdk() {
  get_default_sdk iphoneos
}

get_default_simulator_sdk() {
  get_default_sdk iphonesimulator
}

get_default_macosx_sdk() {
  get_default_sdk macosx
}

get_default_appletvos_sdk() {
  get_default_sdk appletvos
}

get_default_appletvsimulator_sdk() {
  get_default_sdk appletvsimulator
}

get_default_watchos_sdk() {
  get_default_sdk watchos
}

get_default_watchsimulator_sdk() {
  get_default_sdk watchsimulator
}

canon_name() {
  read name
  echo "$name" | tr '.' '_'
}

