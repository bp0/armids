#!/bin/bash
# note: this script might require bash 4.0+

CPU_MODEL_NAME=""
CPU_IMPLEMENTER=""
CPU_PART=""

if [[ $# -eq 0 ]]
then
    CPUINFO_CORE0=`cat /proc/cpuinfo | sed -e '/^$/,$d'`
    #echo "core0=$CPUINFO_CORE0"

    IFS=$'\n'       # make newlines the only separator
    for line in $(echo "$CPUINFO_CORE0")
    do
        #echo "line=$line"
        key=`echo $line | cut -d ':' -f 1 | xargs`
        value=`echo $line | cut -d ':' -f 2- | xargs`
        #echo "key=$key; value=$value"

        case "$key" in
            "model name")
                CPU_MODEL_NAME="$value"
                ;;
            "CPU implementer")
                CPU_IMPLEMENTER="$value"
                ;;
            "CPU part")
                CPU_PART="$value"
                ;;
            *)
                # ignored
                ;;
        esac
    done
else
    if [[ "$1" == "-h" || "$1" == "--help" || "$1" == "-?" || $# -gt 2 ]]
    then
        echo "command line help - invocation variants are:"
        echo "  -h/--help/-?        show this help"
        echo "  <no-args>           run with data from /proc/cpuinfo"
        echo "  implementer [part]  evaluate for the provided data (adding 0x-prefix if needed)"
        exit 0
    fi

    CPU_MODEL_NAME="ARM (default)"
    CPU_IMPLEMENTER="$1"
    if [[ $# -ge 2 ]]
    then
        CPU_PART="$2"
    fi
fi

if [[ "$CPU_IMPLEMENTER" != "" ]]
then
    if [[ "${CPU_IMPLEMENTER,,}" != 0x* ]] # note: wildcard string must lack quotes
    then
        CPU_IMPLEMENTER="0x$CPU_IMPLEMENTER"
    fi
fi

if [[ "$CPU_PART" != "" ]]
then
    if [[ "${CPU_PART,,}" != 0x* ]] # note: wildcard string must lack quotes
    then
        CPU_PART="0x$CPU_PART"
    fi
fi

echo "CPU_MODEL_NAME=$CPU_MODEL_NAME"
echo "CPU_IMPLEMENTER=$CPU_IMPLEMENTER"
echo "CPU_PART=$CPU_PART"

CPU_MODEL_NAME_IS_ARM=`echo $CPU_MODEL_NAME | grep -c -i ARM`

if [[ $CPU_MODEL_NAME_IS_ARM -gt 0 ]]
then
    echo "processor seems to be an ARM..."
    if [[ "$CPU_IMPLEMENTER" != "" ]]
    then
        cd util
        if [[ ! -x ./armid ]]
        then
            make
        fi
        if [[ "$CPU_PART" != "" ]]
        then
            ./armid -d ../arm.ids $CPU_IMPLEMENTER $CPU_PART
        else
            ./armid -d ../arm.ids $CPU_IMPLEMENTER
        fi
        cd ..
    else
        echo "ERROR: missing a value for CPU_IMPLEMENTER"
        exit 1
    fi
else
    echo "ERROR: processor is not an ARM."
    exit 1
fi
# done.
