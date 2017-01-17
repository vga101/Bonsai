#!/bin/sh
#
# compile Bonsai

mkdir -p runtime/build
cd runtime/build
cmake -DUSE_MPI:BOOL=OFF -DUSE_OPENGL:BOOL=ON -DWAR_OF_GALAXIES:BOOL=ON -DUSE_THRUST:BOOL=ON ../
make

