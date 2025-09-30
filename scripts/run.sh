#!/bin/bash

# OpenGL Classroom Project Run Script

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Compiling OpenGL Classroom Project...${NC}"

# Navigate to project root directory
cd "$(dirname "$0")/.."

# Compile the project
g++ -o main src/main.cpp \
    -Iinclude \
    -lglfw \
    -lGL \
    -ldl \
    -std=c++17

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo -e "${GREEN}Compilation successful!${NC}"
    echo -e "${YELLOW}Running the program...${NC}"
    ./main
else
    echo -e "${RED}Compilation failed!${NC}"
    echo -e "${YELLOW}Make sure you have the required libraries installed:${NC}"
    echo "sudo apt install libglfw3-dev libgl1-mesa-dev libglew-dev libglm-dev"
    exit 1
fi