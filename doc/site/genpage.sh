#!/bin/sh

files="algorithms
       developers 
       documentation 
       environmental_data 
       main
       news 
       om_desktop
       overview 
       publications
       resources
       screenshots
       title
       algorithms/ann
       algorithms/aquamaps
       algorithms/bioclim
       algorithms/csm
       algorithms/enfa
       algorithms/envelope_score
       algorithms/environmental_distance
       algorithms/garp
       algorithms/garp_best_subsets
       algorithms/random_forests
       algorithms/svm"

for f in $files; do
    txt2tags $f.t2t
done
