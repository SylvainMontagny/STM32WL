#!/bin/bash
# This container needs to be run from the STM32WL/STM32WL-standalone folder
sudo docker run --rm -v "$PWD/":/workspace montagny/arm-compiler:1.0  /bin/bash -c "make clean && make"

