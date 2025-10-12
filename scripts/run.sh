#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Compiling OpenGL Classroom Project - Week 2 Enhanced...${NC}"

# Navigate to project root directory
cd "$(dirname "$0")/.."

# Compile the project with all utility files
g++ -o main src/main.cpp \
    src/utils/shaderclass.cpp \
    src/utils/VBO.cpp \
    src/utils/VAO.cpp \
    src/utils/EBO.cpp \
    src/utils/Texture.cpp \
    src/models/Model.cpp \
    -Iinclude \
    -lglfw \
    -lGL \
    -lGLEW \
    -ldl \
    -std=c++17

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo -e "${GREEN}Compilation successful!${NC}"
    
    ./main
else
    echo -e "${RED}Compilation failed!${NC}"
    echo -e "${YELLOW}Make sure you have the required libraries installed:${NC}"
    echo "sudo apt install libglfw3-dev libgl1-mesa-dev libglew-dev libglm-dev"
    exit 1
fi