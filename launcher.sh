#!/bin/bash
date>tst1
./dist/Release/GNU-Linux-x86/genetic pieces_08x08.txt out10x10_1.txt 100 100000 --debug
sleep 360
date>tst2
./dist/Release/GNU-Linux-x86/genetic pieces_08x08.txt out08x08_2.txt 100 100000 --debug
sleep 360
date>tst3
./dist/Release/GNU-Linux-x86/genetic pieces_08x08.txt out08x08_3.txt 250 200000 --debug


