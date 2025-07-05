#!/bin/bash

SKIP=`awk '/^__TGZ_ARCHIVE__/ { print NR + 1; exit 0; }' $0`

DISABLE_SSH=0
DISABLE_RSH=0
DISABLE_PVM=0
DISABLE_XPVM=0
DISABLE_GNUPLOT=0
DISABLE_CHECK=0

usage(){

    cat <<END

----------------------------------------------------------------------
  Options:
     --disable-rsh: disables rsh checking
     --disable-ssh: disables ssh checking
     --disable-pvm: disables pvm checking
     --disable-xpvm: disables xpvm checking
     --disable-gnuplot: disables gnuplot checking
     --disable-extra: disables gnuplot,rsh,ssh,pvm and xpvm checking
     --disable-checkings: disable all checkings
     --help: shows this message
  Please report any problem to tangram2@land.ufrj.br
---------------------------------------------------------------------- 

END
    exit 1
}

for i in $@; do

    case $i in
        --disable-rsh)
            DISABLE_RSH=1
            ;;
        --disable-ssh)
            DISABLE_SSH=1
            ;;
        --disable-pvm)
            DISABLE_PVM=1
            ;;
        --disable-xpvm)
            DISABLE_XPVM=1
            ;;
        --disable-gnuplot)
            DISABLE_GNUPLOT=1
            ;;
        --disable-extra)
            DISABLE_SSH=1
            DISABLE_RSH=1
            DISABLE_PVM=1
            DISABLE_XPVM=1
            DISABLE_GNUPLOT=1
            ;;
        --disable-checkings)
            DISABLE_CHECK=1
            ;;
        --help)
            usage
            ;;
        *)
            echo "Invalid argument $i"
            exit 1
    esac
        
done

echo
echo "-----------------------------------------------"
echo "Tangram-II Tool version 2.0 Installation Script"
echo "Copyright (C) 1997-2005 LAND - UFRJ - BRAZIL"
echo "-----------------------------------------------"
echo
echo " WARNING: It is recommended that you UNINSTALL any other"
echo " previous version of Tangram-II before installing this "
echo " new version."
echo
echo " To uninstall Tangram-II version 1.8, 1.9 or 2.0 do the following:"
echo "  1. Remove the /usr/local/Tangram2 directory"
echo "  2. Remove /usr/local/Tangram2/bin from your PATH"
echo
echo " If you have any problem with the Tangram-II Tool installation,"
echo " send an e-mail to tangram2@land.ufrj.br"
echo
echo " Press ENTER to continue"
read LIXO

if [ ! $DISABLE_CHECK -eq 1 ]; then
    echo " Starting checkings ( use $0 --disable-checkings to disable checkings)"
    printf "  Looking for JAVA_HOME environment variable..."
    if [ -z $JAVA_HOME ]; then
        echo " FAILED"
        echo "------------------"
        echo " ERROR: JAVA_HOME not set."
        echo "        Please install JRE or JDK and set the JAVA_HOME environment variable"
        echo "------------------"
        exit 1
    else
        echo " FOUND"
        echo "    JAVA_HOME=$JAVA_HOME"
        JAVA="$JAVA_HOME/bin/java"
        JAVA_VERSION=`$JAVA -version 2>&1 | grep "java version"`
        if [ $? -eq 0 ]; then
            echo "    $JAVA_VERSION"
        else
            echo "Error: no java found at $JAVA_HOME"
            exit 1
        fi
    fi

    printf "  Looking for gcc ..."
    GCC=`which gcc 2>&1`
    if [ $? -eq 0 ] && [ -z "$GCC" ]; then
        echo " FAILED"
        echo "    You must install gcc (GNU C Compiler)"
        exit 1
    else
        echo " FOUND"
        echo "    `$GCC -v 2>&1 | grep version`"
    fi

    printf "  Looking for g++ ..."
    GPP=`which g++ 2>&1`
    if [ $? -eq 0 ] && [ -z "$GPP" ]; then
        echo " FAILED"
        echo "    You must install g++ (GNU C++ Compiler)"
        exit 1
    else
        echo " FOUND"
        echo "    `$GPP -v 2>&1 | grep version`"
    fi

    printf "  Looking for make ..."
    MAKE=`which make 2>&1`
    if [ $? -eq 0 ] && [ -z "$MAKE" ]; then
        echo " FAILED"
        echo "    make is necessary for Tangram-II "
        echo "    Please install it."
        exit 1
    else
        echo " FOUND"
        echo "     `make -v 2>&1 | grep version`"
    fi

    printf "  Looking for gnuplot ..."
    if [ ! $DISABLE_GNUPLOT -eq 1 ]; then
        GNUPLOT=`which gnuplot 2>&1`
        if [ $? -eq 0 ] && [ -z "$GNUPLOT" ]; then
            echo " FAILED"
            echo "    gnuplot is necessary for plotting analytical "
            echo "    and simulation results in Tangram-II."
            echo "    If you wish you may run this script with the"
            echo "    --disable-gnuplot option to pass through this test."
            exit 1
        else
            echo " FOUND"
        fi
    else
        echo " DISABLED"
    fi

    printf "  Looking for ssh ..."
    if [ ! $DISABLE_SSH -eq 1 ]; then
        SSH=`which ssh 2>&1`
        if [ $? -eq 0 ] && [ -z "$SSH" ]; then
            echo " FAILED"
            echo "    ssh is necessary in traffgen tool."
            echo "    If you wish you may run this script with the"
            echo "    --disable-ssh option to pass through this test."
            exit 1
        else
            echo " FOUND"
            echo "     `ssh -V 2>&1`"
        fi
    else
        echo "DISABLED"
    fi

    printf "  Looking for rsh ..."
    if [ ! $DISABLE_RSH -eq 1 ]; then
        RSH=`which rsh 2>&1`
        if [ $? -eq 0 ] && [ -z "$RSH" ]; then
            echo " FAILED"
            echo "    rsh is necessary in traffgen tool and parallelize runs."
            echo "    If you wish you may run this script with the"
            echo "    --disable-rsh option to pass through this test."
            exit 1
        else
            echo " FOUND"
        fi
    else
        echo " DISABLED"
    fi

    printf "  Looking for pvm ..."
    if [ ! $DISABLE_PVM -eq 1 ]; then
        PVM=`which pvm 2>&1`
        if [ $? -eq 0 ] && [ -z "$PVM" ]; then
            echo " FAILED"
            echo "    pvm is necessary in parallelize runs."
            echo "    If you wish you may run this script with the"
            echo "    --disable-pvm option to pass through this test."
            exit 1
        else
            echo " FOUND"
        fi
    else
        echo " DISABLED"
    fi

    printf "  Looking for xpvm ..."
    if [ ! $DISABLE_XPVM -eq 1 ]; then
        XPVM=`which xpvm 2>&1`
        if [ $? -eq 0 ] && [ -z "$XPVM" ]; then
            echo " FAILED"
            echo "    xpvm is necessary in parallelize runs."
            echo "    If you wish you may run this script with the"
            echo "    --disable-xpvm option to pass through this test."
            exit 1
        else
            echo " FOUND"
        fi
    else
        echo " DISABLED"
    fi
else
    echo "All checkings disabled"
fi

echo "  Where do you want to install Tangram-II? [/usr/local]"
read TANGRAM2_HOME

if [ -d $TANGRAM2_HOME/Tangram2 ]; then
    echo
    echo "ERROR:"
    echo " $TANGRAM2_HOME/Tangram2 directory already exists!"
    echo " You must either remove it or choose another location."
    echo
    exit 1
fi

if [ -z $TANGRAM2_HOME ]; then
    echo "    Using default directory [/usr/local]"
    TANGRAM2_HOME="/usr/local"
else
    echo "  Extracting Tangram-II files to $TANGRAM2_HOME"
fi
BASEDIR=`pwd`
cd `dirname $0`
TARDIR=`pwd`
cd $BASEDIR
if [ -w $TANGRAM2_HOME ]; then
    cd $TANGRAM2_HOME
    echo
    tail +$SKIP $BASEDIR/$0 | gunzip -c 2> /dev/null |tar -xvf -
    cd $BASEDIR
else
    echo "    You don't have write permissions on $TANGRAM2_HOME"
    exit 1
fi

#Generating Xdefaults
XDEFAULTS=$TANGRAM2_HOME/Tangram2/etc/Xdefaults
DOMAINS_TEMP_FILE=$TANGRAM2_HOME/Tangram2/etc/Domains.tmp

i=1;
for domains in `ls $TANGRAM2_HOME/Tangram2/Domain`;do
    echo "Tgif*DomainPath${i}:   ${domains}:$TANGRAM2_HOME/Tangram2/Domain/${domains}/:." >> $DOMAINS_TEMP_FILE
    ((i++))
done

DEFAULT_DOMAIN=`grep -n 'TANGRAM2_OBJECTS' $DOMAINS_TEMP_FILE| cut -d ':' -f 1`

echo > $XDEFAULTS
echo "Tgif*MaxDomains:    $i" >> $XDEFAULTS
echo "Tgif*DefaultDomain: $DEFAULT_DOMAIN" >> $XDEFAULTS
echo "Tgif*DomainPath0:   LOCAL:." >> $XDEFAULTS

cat $DOMAINS_TEMP_FILE >> $XDEFAULTS
 
echo >> $XDEFAULTS
echo "Tgif.ImportFromLibrary: False" >> $XDEFAULTS
echo "Tgif*Maximize_Color:    False" >> $XDEFAULTS
echo "Tgif.NoTgifIcon: True" >> $XDEFAULTS
echo "Tgif.UseWMIconPixmap: True" >> $XDEFAULTS

\rm -f $DOMAINS_TEMP_FILE

if [ $UID -eq 0 ]; then
    echo "--------------------------------------------------------------------"
    echo "Now YOU MUST do the following steps:"
    echo 
    echo "1. Include $TANGRAM2_HOME/Tangram2/bin in system search path"
    echo "2. Include the following line to your /etc/profile :"
    echo "    export TANGRAM2_HOME=$TANGRAM2_HOME/Tangram2"
    echo "3. Add the $TANGRAM2_HOME/Tangram2/etc/Xdefaults to system Xdefaults"
    echo
    echo "You can find the PDF version of the Tangram-II User's Manual"
    echo " at $TANGRAM2_HOME/Tangram2/docs"
    echo "You will need a PDF reader to browse the Tangram-II User Manual"
    echo "--------------------------------------------------------------------"
else
    echo "------------------------------------------------------------------"
    echo "Now YOU MUST do the following steps:"
    echo 
    echo "1. Include $TANGRAM2_HOME/Tangram2/bin in your search path"
    echo "   For the bash shell add the following line to ~/.bash_profile"
    echo "       export PATH=$TANGRAM2_HOME/Tangram2/bin:\$PATH"
    echo "2. Set the TANGRAM2_HOME environment variable"
    echo "   For the bash shell add the following line to ~/.bash_profile"
    echo "       export TANGRAM2_HOME=$TANGRAM2_HOME/Tangram2"
    echo "3. Add the $TANGRAM2_HOME/Tangram2/etc/Xdefaults to your Xdefaults"
    echo "   (or Xresources)"
    echo "    cat $TANGRAM2_HOME/Tangram2/etc/Xdefaults >> ~/.Xdefaults"
    echo "4. Read the new X configuration:"
    echo "    xrdb ~/.Xdefaults"
    echo
    echo "You can find the PDF version of the Tangram-II User's Manual"
    echo " at $TANGRAM2_HOME/Tangram2/docs"
    echo "You will need a PDF reader to browse the Tangram-II User Manual"
    echo "------------------------------------------------------------------"
    echo
fi
echo "Press ENTER to finish the Tangram-II Installation"
read NOTHING
echo "------------------------------------------------------------------"
echo " Thank you for installing Tangram-II Tool"
echo "------------------------------------------------------------------"

exit 0
__TGZ_ARCHIVE__
