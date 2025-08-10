@echo off

pushd "%~dp0\..\..\"

echo [Info] Running tests in debug mode
echo.

@REM Debug
xmake f -p windows -a x64 -m debug --yes
xmake build
.\Binaries\Engine\windows-x64\debug\EngineTests.exe --no-xml

if %ERRORLEVEL% neq 0 (
  echo [Error]: Tests in debug mode failed with exit code %ERRORLEVEL%
  goto exit
)

echo [Info] Finished running tests in debug mode
echo.
echo [Info] Running tests in development mode
echo.

@REM Development
xmake f -p windows -a x64 -m development --yes
xmake build
.\Binaries\Engine\windows-x64\development\EngineTests.exe --no-xml

if %ERRORLEVEL% neq 0 (
  echo [Error]: Tests in development mode failed with exit code %ERRORLEVEL%
  goto exit
)

echo [Info] Finished running tests in development mode
echo.
echo [Info] Running tests in shipping mode
echo.

@REM Shipping
xmake f -p windows -a x64 -m shipping --yes
xmake build
.\Binaries\Engine\windows-x64\shipping\EngineTests.exe --no-xml

if %ERRORLEVEL% neq 0 (
  echo [Error]: Tests in shipping mode failed with exit code %ERRORLEVEL%
  goto exit
)

echo [Info] Finished running tests in shipping mode

:exit
popd

exit /b %ERRORLEVEL%