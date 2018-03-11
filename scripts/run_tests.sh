#! /bin/sh
scripts_dir=`dirname "$0"`
if [ "x`echo ${scripts_dir} | grep -e '^\/'`" = "x" ] ; then
  scripts_dir="`pwd`/${scripts_dir}"
fi

test_sub_proj="${scripts_dir}/../test"

cd "$test_sub_proj"
[ ! -d node_modules ] && yarn install
[ ! -d build ] && mkdir build
cd build
cmake ../..
make
if [ $? -ne 0 ] ; then
  exit -1
fi

${scripts_dir}/native_tests.sh "${test_sub_proj}/build"

cd "${test_sub_proj}"
npm test
