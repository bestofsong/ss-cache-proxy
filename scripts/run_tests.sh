#! /bin/sh
scripts_dir=`dirname "$0"`
test_sub_proj="${scripts_dir}/../test"

cd "$test_sub_proj"
[ ! -d node_modules ] && yarn install
[ ! -d build ] && mkdir build
cd build
cmake ../..
make
cd "${test_sub_proj}"
npm test
