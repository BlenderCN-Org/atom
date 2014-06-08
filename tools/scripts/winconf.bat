@echo off

if not defined QT4_ROOT (set QT4_ROOT="C:\Qt\qt-4.8.6")
if not defined THIRDPARTY (set THIRDPARTY="C:\dev\thirdparty")

python waf configure --msvc_version="msvc 12.0" --msvc_target="x86" -t release

if %ERRORLEVEL% NEQ 0 echo "Configuration error! Please fix it and re-run configuration script again." & EXIT /B 1

echo Configuration...Ok

python tools\scripts\fixconf.py build\c4che\_cache.py

if %ERRORLEVEL% NEQ 0 echo "Configuration fix wasn't applied!" & EXIT /B 1

echo Fixing _cache.py...Ok
