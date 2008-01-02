#!/bin/bash

if ! test -d ./man1; then
  mkdir man1
fi

./txt2man.sh -P om_console -t om_console om_console.txt > man1/om_console.man
./txt2man.sh -P om_pseudo -t om_pseudo om_pseudo.txt > man1/om_pseudo.man

