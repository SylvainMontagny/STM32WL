#!/bin/bash
# This container needs to be run from the STM32WL/STM32WL-standalone folder
sudo make clean
sudo docker run --rm -v "$PWD/":/workspace montagny/arm-compiler 

