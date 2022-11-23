# List your source and build output directories:
SRC_DIR := ./src
OBJ_DIR := ../bin/obj
BIN_DIR := ../bin
DEV_DIR :=

# or . if you want it in the current directory

# Name your final target, that is, your executable:
EXE := $(BIN_DIR)/main.exe

# List your source files:
SRC := $(wildcard $(SRC_DIR)/*.cpp)
# From the source files, list the object files:
#OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
# You can also do it like that
OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

INCLUDE_PATH := -I../code/inc
LIB_DIR :=
LIB := -lws2_32

CXX := g++
# Now let's handle the flags
# CPPFLAGS := -Iinclude -MMD -MP # -I is a preprocessor flag, not a compiler flag
CPPFLAGS := $(INCLUDE_PATH) -MMD -MP -std=c++17

# Release
# CXXFLAGS := -O3 -s -DNDEBUG

# Debug
CXXFLAGS := -g -Og -Wall -Wextra -Wshadow -Wunreachable-code -Werror

# CFLAGS   := -Wall              # some warnings about bad code
# LDFLAGS  := -Llib              # -L is a linker flag
LDFLAGS  := $(LIB_DIR) 
# LDLIBS   := -lm                # Left empty if no libs are needed
LDLIBS   := $(LIB)  

.PHONY: all clean

# CPP stands for C PreProcessor here, not CPlusPlus! 
# Use CXXFLAGS for C++ flags and CXX for C++ compiler.

# The -MMD -MP flags are used to generate the header dependencies automatically. 
# We will use this later on to trigger a compilation when only a header changes.

# It is widely spread that the default target should be called all and that it should be the first target in your Makefile. 
# Its prerequisites shall be the target you want to build when writing only make on the command line:
all: $(EXE)

# One problem though is Make will think we want to actually create a file or folder named all, 
# so let's tell him this is not a real target:
# .PHONY: all

# Now list the prerequisites for building your executable, and fill its recipe to tell make what to do with these:
# $(EXE): $(OBJ)
#	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
# Note that your $(BIN_DIR) might not exist yet so the call to the compiler might fail. 
# Let's tell make that you want it to check for that first:
$(EXE): $(OBJ) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BIN_DIR):
	mkdir $(subst /,\,$(BIN_DIR))

# $(CC) is a built-in variable already containing what you need when compiling and linking in C
# To avoid linker errors, it is strongly recommended to put $(LDFLAGS) before your object files and $(LDLIBS) after
# $(CPPFLAGS) and $(CFLAGS) are useless here, the compilation phase is already over, it is the linking phase here

# Next step, since your source and object files don't share the same prefix, 
# you need to tell make exactly what to do since its built-in rules don't cover your specific case:
# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
#    $(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
# Same problem as before, your $(OBJ_DIR) might not exist yet so the call to the compiler might fail. 
# Let's update the rules:
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir $(subst /,\,$(OBJ_DIR))

# Ok, now the executable should build nicely. We want a simple rule to clean the build artifacts though:
clean:
	@rmdir /q /s $(subst /,\,$(BIN_DIR)) 
	@rmdir /q /s $(subst /,\,$(OBJ_DIR))

# The @ disables the echoing of the command

# (Again, clean is not a target that needs to be created, so add it to the .PHONY special target!)

# Last thing. Remember about the automatic dependency generation? 
# GCC and Clang will create .d files corresponding to your .o files, which contains Makefile rules for us to use, 
# so let's include that in here:
-include $(OBJ:.o=.d) # The dash is used to silence errors if the files don't exist yet