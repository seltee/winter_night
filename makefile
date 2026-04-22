CXX = g++
CC = gcc
LD = g++

CXX_FLAGS = -DWNE_BUILD_DLL -Iinclude -I../vulkan/Include -Wall -std=c++20 -mfpmath=sse -g -O2

LIBRARIES = -lkernel32 -luser32 -lgdi32 -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32 -lvulkan-1

EXT = ".exe"

L_FLAGS = -shared -Wall -g -L"../vulkan/Lib"

TARGET = bin/libwne.dll
IMPLIB = lib/libwne.dll.a
COPY = xcopy /Y
MOVE = move

EXAMPLE_FLAGS = -L./ -lwne

SHADERS_DIR = shaders
SRCDIR = src
EXMDIR = examples
OBJDIR = objects
BINDIR = bin
EXDIR  = examples
EXOBJ  = objects/examples
SHADER_OUTPUT_DIR := $(BINDIR)/shaders

# Source and object files
SOURCES := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/**/*.cpp) $(wildcard $(SRCDIR)/**/**/*.cpp) $(wildcard $(SRCDIR)/**/**/**/*.cpp)
OBJ_FILES := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES)) $(STEAM_WRAPPER)

# Shaders
FRAGMENT_SHADER_SOURCES := $(wildcard $(SHADERS_DIR)/*.vert) $(wildcard $(SHADERS_DIR)/**/*.vert) $(wildcard $(SHADERS_DIR)/**/**/*.vert)
VERTEX_SHADER_SOURCES := $(wildcard $(SHADERS_DIR)/*.frag) $(wildcard $(SHADERS_DIR)/**/*.frag) $(wildcard $(SHADERS_DIR)/**/**/*.frag)

SHADER_SOURCES := $(VERTEX_SHADER_SOURCES) $(FRAGMENT_SHADER_SOURCES)
SHADER_OBJECTS := $(patsubst $(SHADERS_DIR)/%, $(SHADER_OUTPUT_DIR)/%.spv, $(SHADER_SOURCES))

# Examples
EXSOURCES := $(wildcard $(EXDIR)/*.cpp)
EXOBJECTS := $(patsubst $(EXDIR)/%.cpp,$(EXOBJ)/%.o,$(EXSOURCES))
EXBINARIES := $(patsubst $(EXDIR)/%.cpp,$(BINDIR)/%,$(EXSOURCES))

EXLFLAGS = -Wall -g -v -L"x86_64-w64-mingw32/lib" -L"lib" -L$(BINDIR)
EXLIBRARIES = -lkernel32 -luser32 -lshell32 -lwne

$(info SOURCES = $(SOURCES))
$(info OBJECTS = $(OBJ_FILES))
$(info VERT SHADERS = $(VERTEX_SHADER_SOURCES))
$(info FRAG SHADERS = $(FRAGMENT_SHADER_SOURCES))

all: engine shaders examples 

# Library
engine: $(TARGET) 

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo Compiling $<
	$(CXX) $(CXX_FLAGS) -o $@ -c $<

$(TARGET): $(OBJ_FILES)
	@echo Linking $@
	$(LD) -o $@ $^ $(LIBRARIES) $(L_FLAGS) -Wl,--out-implib,$(IMPLIB)

# Shaders
shaders: $(SHADER_OBJECTS)

$(SHADER_OUTPUT_DIR):
	@mkdir -p $(dir $@)

$(SHADER_OUTPUT_DIR)/%.spv: $(SHADERS_DIR)/% | $(SHADER_OUTPUT_DIR)
	@mkdir -p $(dir $@)          # create subdirectory if needed
	glslc $< -o $@
	@echo "Compiled shader: $< → $@"

# Examples
examples: $(EXBINARIES)

$(EXOBJ)/%.o: $(EXDIR)/%.cpp
	@mkdir -p "objects/examples"
	@$(MKDIR)
	@echo Compiling example $<
	$(CXX) -Iinclude -Wall -std=c++20 -g -o $@ -c $<

$(BINDIR)/%: $(EXOBJ)/%.o $(TARGET)
	@$(MKDIR)
	@echo Linking example $@
	$(LD) $(EXLFLAGS) -o $@ $< $(EXLIBRARIES) 