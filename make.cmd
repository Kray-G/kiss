@echo off

pushd src
nmake -f Makefile.msc %*
popd

if exist src\kiss.exe (
    copy /y src\kiss.exe .
)
