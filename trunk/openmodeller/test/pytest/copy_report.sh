#!/bin/sh

echo Copying test report to $1
rm $1/*.html $1/*.css $1/algs/*.bmp $1/algs/*.xml
cp *.html *.css $1
cp algs/*.bmp algs/*.xml $1/algs
