@echo off

pushd src
nmake -f Makefile.msc %*
popd
