#!/bin/bash

# OpenGL Classroom Project Run Script - Week 2 Enhanced with 360° View

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
    echo -e "${YELLOW}Running Enhanced Classroom with 360° Viewing...${NC}"
    echo ""
    echo -e "${BLUE}=== ENHANCED CLASSROOM FEATURES ===${NC}"
    echo -e "  📚 ${GREEN}25 Custom Blender Desks${NC} - arranged in 5×5 grid (4.0m col, 3.5m row spacing - MAXIMUM SEPARATION!)"
    echo -e "  🌀 ${GREEN}6 Ceiling Fans${NC} - arranged in 2×3 grid pattern"
    echo -e "  🎯 ${GREEN}Teacher's Podium${NC} - large podium at front-left"
    echo -e "  📋 ${GREEN}Bright Green Blackboard${NC} - mounted on front wall"
    echo -e "  🏗️ ${GREEN}Realistic Ceiling Tiles${NC} - 14×20 grid with extruded borders"
    echo -e "  💡 ${GREEN}Enhanced Lighting${NC} - realistic classroom illumination"
    echo -e "  🎨 ${GREEN}Realistic Materials${NC} - shiny tile floor, rough walls, plastic ceiling"
    echo -e "  📏 ${GREEN}Perfect Classroom${NC} - 32m × 21.5m with 4.0m col spacing (NO OVERLAP!) + 4m front space!"
    echo ""
    echo -e "${PURPLE}=== 360° CAMERA CONTROLS ===${NC}"
    echo -e "  ${GREEN}Free Movement:${NC}"
    echo -e "    WASD: Move camera horizontally"
    echo -e "    Q/E: Move camera up/down"
    echo -e "    Mouse: Look around (360° rotation)"
    echo ""
    echo -e "  ${GREEN}Preset Views (Number Keys):${NC}"
    echo -e "    ${YELLOW}3${NC}: Left Side View - from left wall (WORKING)"
    echo -e "    ${YELLOW}4${NC}: Right Side View - from right wall (WORKING)"
    echo -e "    ${YELLOW}R${NC}: Reset to Left Side View"
    echo ""
    echo -e "  ${GREEN}Special:${NC}"
    echo -e "    ESC: Exit program"
    echo -e "    ${YELLOW}F${NC}: Toggle wireframe mode (debug furniture geometry)"
    echo ""
    echo -e "${YELLOW}🎯 Furniture is CLEARLY VISIBLE from side angles 3 & 4!${NC}"
    echo -e "${YELLOW}📋 Blackboard is BRIGHT GREEN and easily identifiable!${NC}"
    echo -e "${YELLOW}🔄 Use keys 3 & 4 for perfect classroom views, then navigate with WASD!${NC}"
    echo -e "${YELLOW}🔧 Press F to toggle wireframe mode if needed!${NC}"
    echo ""
    ./main
else
    echo -e "${RED}Compilation failed!${NC}"
    echo -e "${YELLOW}Make sure you have the required libraries installed:${NC}"
    echo "sudo apt install libglfw3-dev libgl1-mesa-dev libglew-dev libglm-dev"
    exit 1
fi