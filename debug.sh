#!/bin/bash
set -euo pipefail
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

binary="$DIR/build/projectThing.elf"


# https://unix.stackexchange.com/a/358101/60480
# cross-platform!
#port=$( netstat -an | awk '
#  $6 == "LISTEN" {
#    if ($4 ~ "[.:][0-9]+$") {
#      split($4, a, /[:.]/);
#      port = a[length(a)];
#      p[port] = 1
#    }
#  }
#  END {
#    for (i = 3000; i < 65000 && p[i]; i++){};
#    if (i == 65000) {exit 1};
#    print i
#  }
#')


# multicore means multiple gdb per core
# this runs gdb over a pipe for core 0, and puts conre 1 on port 3334
#gdb-multiarch --quiet -ex "target extended-remote | openocd -c 'gdb_port pipe' -f interface/cmsis-dap.cfg -c'transport select swd' -c 'adapter speed 5000' -f target/rp2040.cfg -c 'rp2040.core1 configure -gdb-port 3334'"  -x gdbConfig "$binary"


# https://github.com/raspberrypi/debugprobe/issues/45#issuecomment-1332610961
# The default configuration for openocd 0.12rc2 with an rp2040 target will debug both cores with dedicated gdb instances.
# ...
# The rp2040 timer is set up by default to stop counting when any core is stopped by a debugger and so the timer would never advance.
gdb-multiarch --quiet -ex "target extended-remote | openocd -c 'gdb_port pipe' -f interface/cmsis-dap.cfg -c'transport select swd' -c 'adapter speed 5000' -c 'set USE_CORE 0' -f target/rp2040.cfg"  -x gdbConfig "$binary"
