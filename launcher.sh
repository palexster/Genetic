#!/bin/bash
date>tst1
./dist/Release/GNU-Linux-x86/genetic pieces_10x10.txt out10x10_1.txt 157 150000
date>tend1
sleep 360
date>tst2
./dist/Release/GNU-Linux-x86/genetic pieces_10x10.txt out10x10_2.txt 157 150000
date>tend2

