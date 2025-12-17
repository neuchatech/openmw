# Explicitly set PATH with double quotes to handle spaces correctly in bash
# Try both /c/ and /mnt/c/ prefixes to cover Git Bash and WSL scenarios
export PATH=$PATH:"/c/Program Files/7-Zip":"/c/Program Files/CMake/bin":"/c/Program Files (x86)/CMake/bin":"/mnt/c/Program Files/7-Zip":"/mnt/c/Program Files/CMake/bin"

echo "DEBUG: PATH is currently:"
echo "$PATH"

echo "Checking for tools..."
which 7z
which cmake
cmake --version

./CI/before_script.msvc.sh -p Win64 -v 2022 -N -c Release -c Debug
