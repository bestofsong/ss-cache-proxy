#! /bin/sh

scripts_dir=`dirname "$0"`
if [ "x`echo ${scripts_dir} | grep -e '^\/'`" = "x" ] ; then
  scripts_dir="`pwd`/${scripts_dir}"
fi

test_bin_dir=$1

${test_bin_dir}/header

