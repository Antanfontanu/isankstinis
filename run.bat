@echo off

if not exist build mkdir build
cd build

cmake ..
cmake --build . --config Release

echo.
echo Paleidziami testai...
.\tests.exe

echo.
echo Paleidziama programa...
.\Antanfontanu.exe

pause
