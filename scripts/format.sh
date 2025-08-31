#!/bin/bash
# Format source code using clang-format
echo "Formatting source code..."
find src -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) -exec dos2unix {} \;
find src -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.c" -o -name "*.h" \) -exec clang-format -style=file -i {} \;
echo "Code formatting complete!"
