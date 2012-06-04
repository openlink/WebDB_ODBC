#!/bin/sh
#
#  install.sh
#

umask 022

#
#  Installation directory
#
PREFIX=`pwd`

#
#  Determine hostname
#
HOST=`hostname | sed -e 's/\..*//' 2>/dev/null`
HOST=${HOST:-localhost}
echo ""
echo "Started installation on : $HOST  $PREFIX"
echo ""

echo "Copying 'OpenLink HTML5-iODBC Plugin' to /usr/mozilla/plugins/"
mkdir -pv /usr/lib/mozilla/plugins
cp libnpiodbc.so  /usr/lib/mozilla/plugins/libnpiodbc.so

echo "End of installation"
exit 0
