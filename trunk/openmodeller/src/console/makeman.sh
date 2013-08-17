#!/bin/bash

./txt2man.sh -P om_console -t om_console om_console.txt > man/om_console.1
./txt2man.sh -P om_algorithm -t om_algorithm om_algorithm.txt > man/om_algorithm.1
./txt2man.sh -P om_points -t om_points om_points.txt > man/om_points.1
./txt2man.sh -P om_pseudo -t om_pseudo om_pseudo.txt > man/om_pseudo.1
./txt2man.sh -P om_sampler -t om_sampler om_sampler.txt > man/om_sampler.1
./txt2man.sh -P om_model -t om_model om_model.txt > man/om_model.1
./txt2man.sh -P om_test -t om_test om_test.txt > man/om_test.1
./txt2man.sh -P om_project -t om_project om_project.txt > man/om_project.1
./txt2man.sh -P om_niche -t om_niche om_niche.txt > man/om_niche.1
./txt2man.sh -P om_evaluate -t om_evaluate om_evaluate.txt > man/om_evaluate.1
./txt2man.sh -P om_viewer -t om_viewer om_viewer.txt > man/om_viewer.1
