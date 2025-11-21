#!/bin/bash

# OpenGL Classroom Project Run Script - Week 2 Enhanced with 360° View

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Compiling OpenGL Classroom Project - Final...${NC}"

# Navigate to project root directory
cd "$(dirname "$0")/.."

# Compile the project with all utility files
g++ -o main src/main.cpp \
    src/utils/shaderclass.cpp \
    src/utils/VBO.cpp \
    src/utils/VAO.cpp \
    src/utils/EBO.cpp \
    src/utils/Furniture.cpp \
    src/utils/Texture.cpp \
    src/utils/CeilingTiles.cpp \
    src/utils/LightPanels.cpp \
    src/utils/TubeLight.cpp \
    src/utils/Windows.cpp \
    src/utils/RightWallWindows.cpp \
    src/utils/GreenBoard.cpp \
    src/utils/Door.cpp \
    src/utils/ProjectorScreen.cpp \
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
    echo ""
    echo -e "${PURPLE}=== 360° CAMERA CONTROLS ===${NC}"
    echo -e "  ${GREEN}Free Movement:${NC}"
    echo -e "    WASD: Move camera horizontally"
    echo -e "    Q/E: Move camera up/down"
    echo -e "    Mouse: Look around (360° rotation)"
    echo ""
    echo -e "  ${GREEN}Preset Views (Number Keys):${NC}"
    echo -e "    ${YELLOW}1${NC}: Reset to Left Side View"
    echo -e "    ${YELLOW}2${NC}: Reset to Right Side View"
    echo -e "    ${YELLOW}3${NC}: Reset to Default View"  
    echo ""
    echo -e "  ${GREEN}R${NC}: Reset to Default View"  
    echo ""
    echo -e "  ${GREEN}Special:${NC}"
    echo -e "    ESC: Exit program"
    echo ""
    ./main
else
    echo -e "${RED}Compilation failed!${NC}"
    echo -e "${YELLOW}Make sure you have the required libraries installed:${NC}"
    echo "sudo apt install libglfw3-dev libgl1-mesa-dev libglew-dev libglm-dev"
    exit 1
fi