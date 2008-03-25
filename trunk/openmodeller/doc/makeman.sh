#!/bin/bash

# Console documentation
./txt2man.sh -P om_console -t om_console om_console.txt > ../src/console/man/om_console.1
./txt2man.sh -P om_create -t om_create om_create.txt > ../src/console/man/om_create.1
./txt2man.sh -P om_project -t om_project om_project.txt > ../src/console/man/om_project.1
./txt2man.sh -P om_pseudo -t om_pseudo om_pseudo.txt > ../src/console/man/om_pseudo.1

