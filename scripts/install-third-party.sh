#! /bin/sh

script_name=`which $0`
if [ "x`echo $script_name | grep -e '^\/'`" = "x"  ] ; then
  script_path=`pwd`/"${script_name}"
else
  script_path="$script_name"
fi

script_root=`dirname "${script_path}"`
source "${script_root}/normalize.sh"

project_root=`dirname $(normalize "$script_root")`


# sqlite3cpp
echo "update submodule"
"${script_root}/install-sqlite3cpp.sh"

third_party_dir="${project_root}/src/third-party"
[ ! -d "${third_party_dir}" ] && mkdir -p "${third_party_dir}"
cd "${third_party_dir}"

boost_src='boost_1_66_0'
if [ -d "${boost_src}" ] ; then
  exit 0
else
  mkdir -p "${boost_src}"
fi

curl  -L -o boost_1_66_0.tar.bz2  'https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.bz2'
tar -C "${boost_src}" --strip-components 1 -xjf boost_1_66_0.tar.bz2
if [ $? -eq 0 ] ; then
  rm boost_1_66_0.tar.bz2
fi

