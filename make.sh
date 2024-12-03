#!/bin/sh
################################################################################
#
# WinJavaLauncher - Wrapper Application Build Script.
#
# @file    make.sh
# @brief   Compilation and linking script for a wrapper application
#          for Windows and Java jpackaged applications.
#
# @author  2024 autumo Ltd. Switzerland, Michael Gasche
# @date    2024-12-02
#
# @license MIT License
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# @description
# This script automates the build process for a wrapper application that serves
# as a launcher for Windows and Java jpackaged applications. It compiles the
# necessary files (miniz library, resource files, main application code), and
# links them into a Windows executable. The executable wraps Java jpackaged apps
# and provides a native Windows launcher for them.
#
# @requirements
# - A compatible MinGW toolchain (e.g., x86_64-w64-mingw32) installed.
# - The resources.rc file should be present for compilation.
# - The miniz library source (miniz.c) should be available.
# - A Windows build environment with the necessary dependencies such as
#   - appropriate system headers and libraries.
#
# @usage
# Simply run this script in a Unix-like environment with access to the
# MinGW toolchain to compile and link the application:
#
#   ./makefile.sh
#
# Ensure that all required dependencies are in place before execution.
#
################################################################################

# ==============================================================================
# Environment Check
# ==============================================================================

# Check if MinGW toolchain is available
if ! command -v x86_64-w64-mingw32-g++ &> /dev/null
then
    echo "Error: MinGW toolchain (x86_64-w64-mingw32-g++) not found. Please install it and try again."
    exit 1
fi

# ==============================================================================
# Configuration Variables
# ==============================================================================

# Define output directories
OUTPUT_DIR=./build

# Define the source directory for the project files
SOURCE_DIR=./src
MINIZ_DIR=./src/miniz

# Define wrapper executable
WRAPPER_APP_EXE="MyWrapperApp.exe"

# Debug mode (set to 1 for debugging or 0 for release)
DEBUG_MODE=0

# ==============================================================================
# Clean Argument
# ==============================================================================

if [ "$1" = "clean" ]; then
    echo "Cleaning build directory..."
    if [ -d "$OUTPUT_DIR" ]; then
        rm -rf "$OUTPUT_DIR"
        echo "Build directory '$OUTPUT_DIR' has been deleted."
    else
        echo "Build directory '$OUTPUT_DIR' does not exist."
    fi
    exit 0
fi

# ==============================================================================
# Compilation and Linking
# ==============================================================================

# Create output directories if they don't exist
mkdir -p $OUTPUT_DIR

# Define compiler and linker flags
CXX=x86_64-w64-mingw32-g++
# Add or omit the -mwindows flag based on DEBUG_MODE
if [ $DEBUG_MODE -eq 1 ]; then
    CXX_FLAGS="-static-libgcc -static-libstdc++ -static"
else
    CXX_FLAGS="-static-libgcc -static-libstdc++ -static -mwindows"
fi
INCLUDE_FLAGS="-I$MINIZ_DIR"

# Step 1: Compile miniz library
echo "Compiling miniz library..."
if ! $CXX -c $MINIZ_DIR/miniz.c -o $OUTPUT_DIR/miniz.o; then
    echo "Error: Failed to compile miniz library."
    exit 1
fi

# Step 2: Compile resources file
echo "Compiling resources..."
if ! x86_64-w64-mingw32-windres $SOURCE_DIR/resources.rc -o $OUTPUT_DIR/resources.o; then
    echo "Error: Failed to compile resources."
    exit 1
fi

# Step 3: Compile and link the main application
echo "Compiling and linking the main application..."
if ! $CXX $CXX_FLAGS -o "$OUTPUT_DIR/$WRAPPER_APP_EXE" $SOURCE_DIR/main.cpp $OUTPUT_DIR/resources.o $OUTPUT_DIR/miniz.o $INCLUDE_FLAGS; then
    echo "Error: Failed to compile and link the application."
    exit 1
fi

# Step 4: Cleanup - Remove intermediate object files
echo "Cleaning up intermediate files..."
rm -f $OUTPUT_DIR/miniz.o
rm -f $OUTPUT_DIR/resources.o

# ==============================================================================
# Final Instructions
# ==============================================================================

echo "Build complete. The executable is located in the '$OUTPUT_DIR' directory."
if [ $DEBUG_MODE -eq 1 ]; then
    echo "Debug mode is enabled. Console output is available for debugging."
else
    echo "Debug mode is disabled. Executable is built for release."
fi

# ==============================================================================
# End of Makefile
# ==============================================================================
