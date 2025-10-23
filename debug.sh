#!/bin/bash
set -euo pipefail
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

binary="$DIR/build/dev_hid_composite.elf"


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
gdb-multiarch --quiet -ex "target extended-remote | openocd -c 'gdb_port pipe' -f interface/cmsis-dap.cfg -c'transport select swd' -c 'adapter speed 5000' -f target/rp2040.cfg -c 'rp2040.core1 configure -gdb-port 3334'"  -x gdbConfig "$binary"
