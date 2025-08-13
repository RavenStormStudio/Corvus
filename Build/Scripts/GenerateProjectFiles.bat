@echo off

pushd "%~dp0\..\..\"

xmake config
xmake project -k vsxmake2022 Build

popd