#!/bin/sh

wget http://www.cria.org.br/~ricardo/openModeller/acc_test_data.tar.gz
tar -xzvf acc_test_data.tar.gz
python AccTest.py $@
