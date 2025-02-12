#!/bin/sh
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# This is a simple script which is meant to help developers
# better deal with the GNU autotools, specifically:
#
#   aclocal
#   autoheader
#   autoconf
#   automake
#
# The whole thing is quite complex...
#
# The idea is to run this collection of tools on a single platform,
# typically the main development platform, running a recent version of
# autoconf. In theory, if we had these tools on each platform where we
# ever expected to port the software, we would never need to checkin
# more than a few autotools configuration files. However, the whole
# idea is to generate a configure script and associated files in a way
# that is portable across platforms, so we *have* to check in a whole
# bunch of files generated by all these tools.

# The real source files are:
#
# acinclude.m4 (used by aclocal)
# configure.ac (main autoconf file)
# Makefile.am, */Makefile.am (automake config files)
#
# All the rest is auto-generated.

if [ "$1" = "clean" ]; then
    echo "Cleaning..."
    rm configure aclocal.m4
    rm m4/*
    rmdir m4
    rm config/*
    rmdir config
    find . -iname "Makefile.in" -type f -exec rm '{}' +
fi

# Prevent any errors that might result from failing to properly invoke 
# `libtoolize` or `glibtoolize,` whichever is present on your system, 
# from occurring by testing for its existence and capturing the absolute path to 
# its location for caching purposes prior to using it later on in 'Step 2:'  
if command -v libtoolize >/dev/null 2>&1; then
  LIBTOOLIZE="$(command -v libtoolize)"
elif command -v glibtoolize >/dev/null 2>&1; then
  LIBTOOLIZE="$(command -v glibtoolize)"
else
  echo "Unable to find a valid copy of libtoolize or glibtoolize in your PATH!"
  bail_out
fi

# create m4 directory if it not exists
if [ ! -d m4 ];  then
    mkdir m4
fi

bail_out()
{
    echo
    echo "  Something went wrong, bailing out!" 
    echo
    exit 1
}

# --- Step 1: Generate aclocal.m4 from:
#             . acinclude.m4 
#             . config/*.m4 (these files are referenced in acinclude.m4)

mkdir -p config

echo "Running aclocal"
aclocal -I config || bail_out

# --- Step 2:

echo "Running $LIBTOOLIZE"
$LIBTOOLIZE -f -c || bail_out
$LIBTOOLIZE --automake || bail_out

# --- Step 3: Generate config.h.in from:
#             . configure.ac (look for AM_CONFIG_HEADER tag or AC_CONFIG_HEADER tag)

echo "Running autoheader"
autoheader -f || bail_out

# --- Step 4: Generate Makefile.in, src/Makefile.in, and a whole bunch of
#             files in config (config.guess, config.sub, depcomp,
#             install-sh, missing, mkinstalldirs) plus COPYING and
#             INSTALL from:
#             . Makefile.am
#             . src/Makefile.am
#
# Using --add-missing --copy makes sure that, if these files are missing,
# they are copied from the system so they can be used in a distribution.

echo "Running automake --add-missing --copy"
automake --add-missing -c  -Wno-portability > /dev/null || bail_out

# --- Step 5: Generate configure and include/miaconfig.h from:
#             . configure.ac
#

echo "Running autoconf"
autoconf || bail_out

echo ""
echo "All done."
echo "To build the software now, do something like:"
echo ""
echo "$ ./configure [--enable-debug] [...other options]"
