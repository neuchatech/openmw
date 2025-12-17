@echo off
echo Setting up PATH for 7-Zip and CMake...
set "PATH=%PATH%;C:\Program Files\7-Zip;C:\Program Files\CMake\bin"

echo Verifying tools...
where 7z
where cmake

echo Running CI setup script...
bash CI/before_script.msvc.sh -p Win64 -v 2022 -N -c Release -c Debug
