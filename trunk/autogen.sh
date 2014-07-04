#!/bin/sh

RED="\\e[31m"
GREEN="\\e[32m"
YELLOW="\\e[33m"
BLACK="\\e[0m"

_3rdParty="3rdparty"
currentDir=`pwd`

function prepareST()
{
    if ! test -f $_3rdParty/st-1.9/obj/libst.a ;then
        cd $_3rdParty && unzip st-1.9.zip && cd st-1.9 && make linux-debug
    fi
    if test $? -ne 0;then
        echo -e $RED"...\t\tFailed!"$BLACK
        exit -1
    fi
}

function prepareOpenSSL()
{
    exit 0   
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
