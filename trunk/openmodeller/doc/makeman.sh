#!/bin/bash

# Console documentation
./txt2man.sh -P om_console -t om_console om_console.txt > ../src/console/man/om_console.1
./txt2man.sh -P om_pseudo -t om_pseudo om_pseudo.txt > ../src/console/man/om_pseudo.1
