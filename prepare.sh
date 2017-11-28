#!/bin/sh
#
# compile Bonsai freshly

builddir=runtime/build
echo "Deleting old build..."
rm -fr $builddir
echo "Now building freshly..."
mkdir -p $builddir
cd $builddir

# on older gpus without 3.5 compute capability (https://developer.nvidia.com/cuda-gpus),
# e.g. my Quadro K4000, add this: -DCOMPILE_SM30=0
cmake -DUSE_MPI:BOOL=OFF -DUSE_OPENGL:BOOL=ON -DWAR_OF_GALAXIES:BOOL=ON -DUSE_THRUST:BOOL=ON ../
make
echo "Done."
