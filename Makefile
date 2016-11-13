XX = g++
CXXFLAGS = -O2 -g -Wall -std=c++0x

# Strict compiler options
#CXXFLAGS += -Werror -Wformat-security -Wignored-qualifiers -Winit-self \
#		-Wswitch-default -Wfloat-equal -Wshadow -Wpointer-arith \
#		-Wtype-limits -Wempty-body -Wlogical-op \
#		-Wmissing-field-initializers -Wctor-dtor-privacy \
#		-Wnon-virtual-dtor -Wstrict-null-sentinel \
#		-Woverloaded-virtual -Wsign-promo -Weffc++

# Directories with source code
SRC_DIR = src

INCLUDE_DIR += include
LIBS_DIR += libs
CXXFLAGS += -I $(INCLUDE_DIR) -I $(LIBS_DIR)

OBJ_DIR = obj
BIN_DIR = .

MAIN_DEP += $(OBJ_DIR)/main.o
MAIN_DEP += $(OBJ_DIR)/shaderprogram.o
MAIN_DEP += $(OBJ_DIR)/object.o
MAIN_DEP += $(OBJ_DIR)/shader.o
MAIN_DEP += $(OBJ_DIR)/figures.o
MAIN_DEP += $(OBJ_DIR)/textures.o
MAIN_DEP += $(OBJ_DIR)/observer.o
MAIN_DEP += $(OBJ_DIR)/maths.o
MAIN_DEP += $(OBJ_DIR)/physics.o
MAIN_DEP += $(OBJ_DIR)/planet_orbit_params.o
MAIN_DEP += $(OBJ_DIR)/material.o

LDFLAGS = -lGL -lglut -lGLEW -lconfig++

# All source files in our project (without libraries!)
CXXFILES := $(wildcard $(SRC_DIR)/*.cpp)

# Default target (make without specified target).
.DEFAULT_GOAL := all

all: $(BIN_DIR)/main

# Suppress makefile rebuilding.
Makefile: ;

# Helper macros
make_path = $(addsuffix $(1), $(basename $(subst $(2), $(3), $(4))))
# Takes path to source file and returns path to corresponding object
src_to_obj = $(call make_path,.o, $(SRC_DIR), $(OBJ_DIR), $(1))

# Rules for compiling targets
# b
$(BIN_DIR)/main: $(MAIN_DEP)
	mkdir -p $(BIN_DIR)
#	cp $(SRC_DIR)/shaders/*.vert	$(BIN_DIR)
#	cp $(SRC_DIR)/shaders/*.frag	$(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(filter %.o, $^) -o $@ $(LDFLAGS)


# Pattern for compiling object files (*.o)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $(call src_to_obj, $<) $<

# Delete all temprorary and binary files
clean:
	#rm -rf $(BIN_DIR) 
	rm -rf $(OBJ_DIR) 
