#!/bin/bash

# OpenGL Classroom Project Run Script - Updated

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Compiling OpenGL Classroom Project...${NC}"

# Navigate to project root directory
cd "$(dirname "$0")/.."

# Compile the project with only existing utility files
g++ -o main src/main.cpp \
    src/utils/shaderclass.cpp \
    src/utils/VBO.cpp \
    src/utils/VAO.cpp \
    src/utils/EBO.cpp \
    -Iinclude \
    -lglfw \
    -lGL \
    -lGLEW \
    -ldl \
    -std=c++17

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo -e "${GREEN}Compilation successful!${NC}"
    echo -e "${YELLOW}Running OpenGL Classroom Project...${NC}"
    echo ""
    echo -e "${BLUE}=== CLASSROOM FEATURES ===${NC}"
    echo -e "  🏛️ ${GREEN}Basic Classroom Structure${NC} - walls, floor, ceiling"
    echo -e "  🎨 ${GREEN}Color-coded Surfaces${NC} - for easy spatial navigation"
    echo -e "  💡 ${GREEN}Basic Lighting System${NC} - ambient and diffuse lighting"
    echo ""
    echo -e "${PURPLE}=== CAMERA CONTROLS ===${NC}"
    echo -e "  ${GREEN}Free Movement:${NC}"
    echo -e "    WASD: Move camera horizontally"
    echo -e "    Q/E: Move camera up/down"
    echo -e "    Mouse: Look around (360° rotation)"
    echo ""
    echo -e "  ${GREEN}Special:${NC}"
    echo -e "    ESC: Exit program"
    echo ""
    echo -e "${YELLOW}🎯 Navigate the classroom using WASD and mouse controls!${NC}"
    echo ""
    ./main
else
    echo -e "${RED}Compilation failed!${NC}"
    echo -e "${YELLOW}Make sure you have the required libraries installed:${NC}"
    echo "sudo apt install libglfw3-dev libgl1-mesa-dev libglew-dev libglm-dev"
    exit 1
fi