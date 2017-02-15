#!/bin/sh
#
# compile Bonsai freshly

builddir=runtime/build
echo "Deleting old build..."
rm -fr $builddir
echo "Now building freshly..."
mkdir -p $builddir
cd $builddir
cmake -DUSE_MPI:BOOL=OFF -DUSE_OPENGL:BOOL=ON -DWAR_OF_GALAXIES:BOOL=ON -DUSE_THRUST:BOOL=ON ../
make
echo "Done."
