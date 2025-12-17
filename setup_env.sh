#!/bin/bash
# Hardcode paths using 8.3 names to avoid space issues and ensure visibility
export PATH=$PATH:/c/PROGRA~1/7-Zip:/c/PROGRA~1/CMake/bin

echo "Wrapper: PATH updated. Verifying tools..."
if command -v 7z >/dev/null 2>&1; then
    echo "  Found 7z: $(command -v 7z)"
else
    echo "  ERROR: 7z not found"
    ls -l /c/PROGRA~1/7-Zip/7z.exe
fi

if command -v cmake >/dev/null 2>&1; then
    echo "  Found cmake: $(command -v cmake)"
else
    echo "  ERROR: cmake not found"
    ls -l /c/PROGRA~1/CMake/bin/cmake.exe
fi

# Run the original script
./CI/before_script.msvc.sh "$@"
