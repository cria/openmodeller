#!/bin/sh

if test ! -x data ; then 
    wget http://www.cria.org.br/~ricardo/openModeller/acc_test_data.tar.gz
    tar -xzvf acc_test_data.tar.gz
fi
python run_all.py $@
