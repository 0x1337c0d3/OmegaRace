#!/bin/bash
find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) -exec dos2unix {} \;
find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) -exec clang-format -style=file -i {} \;
