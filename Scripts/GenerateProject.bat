@echo off
echo ==== Generating Project ====
echo.
pushd %~dp0\..\
CALL ThirdParty\SaffronEngine2D\ThirdParty\Premake\Bin\premake5.exe vs2019
echo.
