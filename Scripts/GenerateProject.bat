@echo off
echo ==== Generating Project ====
echo.
pushd %~dp0\..\
CALL Deps\premake5\Bin\Premake5.exe vs2019
echo.
PAUSE