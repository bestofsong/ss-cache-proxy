#! /bin/sh

script_dir=`dirname $0`
src_path="${script_dir}/../src/cacheproxy/CacheProxy.h"
dest_path="${script_dir}/../src/cacheproxy/CacheProxy.h.in"

egrep -e 'string\& Version\(\) \{' "$src_path" 1>/dev/null
if [ $? -eq 0 ] ; then
  sed -re 's/^[[:space:]]*string\& Version\(\) \{.*$/    string\& Version() { static string v{"@PACKAGE_VERSION@"}; return v; }/' "${src_path}" > "${dest_path}"
else
  sed -re 's/^[[:space:]]+class CacheProxy[[:space:]]\{[[:space:]]*$/&\n  public:\n    string\& Version() { static string v{"@PACKAGE_VERSION@"}; return v; }\n/' "${src_path}" > "${dest_path}"
fi

