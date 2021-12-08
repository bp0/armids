# ARM CPU ID List

A list of ARM CPU IDs in a format similar to pci.ids/usb.ids.

See `arm.ids`.


Some C code to read the file can be found in util/.

Build and run it like this:

    cd util
    make
    ./armid -d ../arm.ids 41 c08

The outputs of the make and run step from aboe might be like that:

    cc -o armid armid.c
    found: implementer[41]='ARM' part[c08]='Cortex-A8'


A demonstration script that is meant to be run intuitively for bash v4 on Linux that will query the needed IDs on the local machine or take them from the command line - found as ./armid.bash.
This script is building and using the mentioned C code from above under the hood

You can run it like this:

    ./armid.bash 41 c08

The outputs of the above invocation might be like that:

    CPU_MODEL_NAME=ARM (default)
    CPU_IMPLEMENTER=0x41
    CPU_PART=0xC08
    processor seems to be an ARM...
    found: implementer[41]='ARM' part[c08]='Cortex-A8'
