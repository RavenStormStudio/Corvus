@echo off

pushd "%~dp0\.."

xmake project -k vsxmake2022 -m "debug,development,shipping" ./Engine/Build/Output

popd