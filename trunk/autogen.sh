#!/bin/sh

RED="\\e[31m"
GREEN="\\e[32m"
YELLOW="\\e[33m"
BLACK="\\e[0m"

_3rdParty="3rdparty"
currentDir=`pwd`
hpdir="http-parser-2.1"
ssldir="openssl-1.0.1c"
conhashdir="libconhash"

function prepareST()
{
    if ! test -f $_3rdParty/st-1.9/obj/libst.a ;then
        cd $_3rdParty && unzip st-1.9.zip && cd st-1.9/ && make linux-debug
    fi
    if test $? -ne 0;then
        echo -e $RED"...\t\tFailed!"$BLACK
        exit -1
    fi
}

# check http parser
function prepareHttpParser()
{
    if ! test -f $_3rdParty/$hpdir/libhttp_parser.a; then
        if test -d $_3rdParty/$hpdir; then
            rm -rf $_3rdParty/$hpdir
        fi
        cd $_3rdParty && unzip http-parser-2.1.zip && cd $hpdir && make package
        if test $? -ne 0; then
            exit -1
        fi
    fi
}


function prepareOpenSSL()
{
    if ! test -f $_3rdParty/$ssldir/libcrypto.a; then
        if test -d $_3rdParty/$ssldir; then
            rm -rf $_3rdParty/$ssldir
        fi
        cd $_3rdParty && unzip openssl-1.0.1c.zip && cd $ssldir && ./config && make
        if test $? -ne 0; then
            exit -1
        fi
    fi
}

function prepareConHash()
{
    if ! test -f $_3rdParty/$conhashdir/bin/libconhash.a; then
        if test -d $_3rdParty/$conhashdir; then
            rm -rf $_3rdParty/$conhashdir
        fi
        cd $_3rdParty && unzip libconhash.zip && cd $conhashdir && make
        if test $? -ne 0; then
            exit -1
        fi
    fi
}

function runCommand()
{
	echo -n -e $YELLOW"run $1"$BLACK
	$*
	if test $? -ne 0;then
		echo -e $RED"\t............................\t\tFailed!"$BLACK
		exit -1	
	fi
	echo -e $YELLOW"\t\t............................\t\tOK"$BLACK
}

function checkFile()
{
	if ! test -f $1;then
		echo -e $YELLOW"creating...\t\t$1"$BLACK
		touch $1
	fi
}

runCommand prepareST
cd $currentDir
runCommand prepareHttpParser
cd $currentDir
runCommand prepareOpenSSL
cd $currentDir
runCommand prepareConHash
cd $currentDir

checkFile NEWS
checkFile README
checkFile AUTHORS
checkFile ChangeLog

runCommand autoheader
runCommand aclocal
runCommand autoconf
runCommand automake --add-missing --foreign
runCommand ./configure --prefix `pwd`/bls

echo -e $RED"Now run make & make install to gen binary. "$BLACK
