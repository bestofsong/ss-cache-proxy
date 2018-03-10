#! /bin/sh

function normalize {
  local rc=0
  local esed
  local ret

  if [ "x`uname -o | grep -i 'darwin'`" != "x" ] ; then
    esed="sed -E"
  else
    esed="sed -r"
  fi


  if [ $# -gt 0 ] ; then
    # invalid
    if [ "x`echo $1 | grep -E -e '^/\.\.'`" != "x" ] ; then
      echo $1
      return -1
    fi

    # convert to absolute path
    if [ "x`echo $1 | grep -E -e '^\/'`" == "x" ] ; then
      normalize "`pwd`/$1"
      return $?
    fi

    ret=`echo $1 | $esed 's;/\.($|/);/;g' | $esed 's;/[^/]*[^/.]+[^/]*/\.\.($|/);/;g'`
  else
    read line
    normalize "$line"
    return $?
  fi

  if [ "x`echo $ret | grep -E -e '/\.(/|$)'`" != "x" ] ; then
    ret=`normalize "$ret"`
    rc=$?
  fi


  if [ "x`echo $ret | grep -E -e '/\.\.(/|$)'`" != "x" ] ; then
    ret=`normalize "$ret"`
    rc=$?
  fi

  echo "$ret"
  return $rc
}


