# Compiler and flags
CXX        := g++
CXXFLAGS   := -std=c++17 -Wall -Iinclude
LDFLAGS    := -lncursesw

# Directories
SRCDIR     := src
OBJDIR     := obj
BINDIR     := bin

# Target executable
TARGET     := snake

# Source and object files
SRCS       := $(wildcard $(SRCDIR)/*.cpp)
OBJS       := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

.PHONY: all clean directories

all: directories $(BINDIR)/$(TARGET)

# Link step
$(BINDIR)/$(TARGET): $(OBJS)
	@echo "Linking $@..."
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile step
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create needed directories
directories:
	@mkdir -p $(OBJDIR) $(BINDIR)

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(OBJDIR) $(BINDIR)/$(TARGET)

