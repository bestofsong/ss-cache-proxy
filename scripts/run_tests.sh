#! /bin/sh
scripts_dir=`dirname "$0"`
test_sub_proj="${scripts_dir}/../unittests"
cd "$test_sub_proj"
npm test
cd -
