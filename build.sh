#!/bin/bash

# --- Configuration ---
# C++ compiler (g++ recommended for C++ code)
CXX="g++"
# Output directory
BUILD_DIR="jsonify/build"
# Executable name
TARGET="jsonify"
# Source files
SOURCES="main.cpp jsonparser.cpp jsonformatter.cpp jsonlinter.cpp"
# Header directory (where header files are located, . in this case)
INCLUDE_DIR="."

# Recommended compiler flags for strictness and warnings
CXXFLAGS="-std=c++11"
CXXFLAGS="${CXXFLAGS} -Wall -Werror -Wextra -pedantic -Wpedantic"
CXXFLAGS="${CXXFLAGS} -pedantic-errors" # Treat pedantic warnings as errors
CXXFLAGS="${CXXFLAGS} -I${INCLUDE_DIR}" # Include directory for headers

# Linker flags (usually none for simple programs)
LDFLAGS=""

# --- Build Process ---

echo "⚙️ Setting up build directory: ${BUILD_DIR}"
mkdir -p ${BUILD_DIR}

# 1. Compile all source files into object files (.o)
# We compile without linking (-c) and specify the output file (-o)
echo "📦 Compiling source files..."
for src_file in ${SOURCES}; do
    obj_file="${BUILD_DIR}/$(basename ${src_file} .cpp).o"
    
    # The -c flag means "compile and assemble, but do not link"
    # We use a subshell to execute the command for better error reporting if needed
    if ! ${CXX} ${CXXFLAGS} -c "${src_file}" -o "${obj_file}"; then
        echo "❌ Compilation failed for ${src_file}"
        exit 1
    fi
    echo "   -> Compiled ${src_file} to ${obj_file}"
done

# 2. Link the object files into the final executable
# We collect all .o files and link them together
OBJ_FILES="${BUILD_DIR}/*.o"
FINAL_EXEC="${BUILD_DIR}/${TARGET}"

echo "🔗 Linking object files to create ${TARGET} executable..."
if ! ${CXX} ${LDFLAGS} ${OBJ_FILES} -o "${FINAL_EXEC}"; then
    echo "❌ Linking failed"
    exit 1
fi

echo "✅ Build successful!"
echo "   Executable created at: ${FINAL_EXEC}"

# --- Execution Example ---
echo
echo "🚀 To run the executable, use: ./${FINAL_EXEC} [options] <file.json>"
