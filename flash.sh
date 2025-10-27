#!/bin/bash
set -euo pipefail
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

binary="$DIR/build/projectThing.elf"



openocd  -f interface/cmsis-dap.cfg -c'transport select swd' -c 'adapter speed 5000' -c 'set USE_CORE 0' -f target/rp2040.cfg -c "program ${binary/elf/hex} verify reset" -c "shutdown"

