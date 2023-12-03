#!/bin/bash

cmake . -Bbuild
cd build
make -j
cd ..
