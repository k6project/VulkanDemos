@echo off
if not exist "%~dp0Project" mkdir %~dp0Project
pushd %~dp0Project
cmake -G "Visual Studio 14 2015 Win64" ..
popd
