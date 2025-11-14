#!/bin/bash

# Setup script for Lab 2 dependencies
# This script downloads and sets up GLAD and ImGui

echo "Setting up Lab 2 dependencies..."

cd "$(dirname "$0")"

# Create third_party directory if it doesn't exist
mkdir -p third_party
cd third_party

# Setup GLAD
if [ ! -d "glad" ]; then
    echo "Setting up GLAD..."
    pip3 install glad -q
    python3 -m glad --profile=core --api="gl=4.6" --out-path=glad --generator=c
    echo "GLAD setup complete!"
else
    echo "GLAD already exists, skipping..."
fi

# Setup ImGui
if [ ! -d "imgui" ]; then
    echo "Setting up ImGui..."
    git clone https://github.com/ocornut/imgui.git --depth 1 -q
    echo "ImGui setup complete!"
else
    echo "ImGui already exists, skipping..."
fi

# Setup STB (already in third_party_local, create symlink if needed)
cd ..
if [ ! -d "third_party_local/stb" ]; then
    echo "Setting up STB..."
    mkdir -p third_party_local/stb
    cd third_party_local/stb
    curl -L -o stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
    curl -L -o stb_image_write.h https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h
    echo "STB setup complete!"
    cd ../..
else
    echo "STB already exists, skipping..."
fi

echo ""
echo "All dependencies setup complete!"
echo "You can now build the project:"
echo "  mkdir build && cd build"
echo "  cmake .."
echo "  cmake --build ."
