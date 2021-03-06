#!/bin/sh
# $Id: bootstrap,v 1.6 2007/11/08 02:19:09 jemarch Exp $
# Created 2003-08-29, Karl Berry.  Public domain.

if test "x$1" = x-n; then
  chicken=true
  echo "Ok, playing chicken; not actually running any commands."
else
  chicken=
fi

echo "Bootstrapping the GNU PDF Library with:"

# This overwrites lots of files with older versions.
# I keep the newest versions of files common between distributions up to
# date in CVS (see util/srclist.txt), because it's not trivial for every
# developer to do this.
#cmd="autoreconf --verbose --force --install --include=m4"

: ${AUTORECONF=autoreconf}
: ${ACLOCAL=aclocal}
: ${LIBTOOLIZE=libtoolize}
: ${AUTOHEADER=autoheader}
: ${AUTOMAKE=automake}
: ${AUTOCONF=autoconf}

# So instead:
cmd="$AUTORECONF -i"
echo "  $cmd"
$chicken eval $cmd || exit 1

echo
echo "Now run configure with your desired options, for instance:"
echo "  ./configure CFLAGS=-g"
echo
