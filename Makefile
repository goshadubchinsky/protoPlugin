# If RACK_DIR is not defined when calling the Makefile, default to two directories above
# RACK_DIR ?= C:/msys64/home/baylarovmurad/Rack
# RACK_DIR ?= C:/msys64/home/beyla/Rack

# export RACK_DIR=C:/msys64/home/baylarovmurad/Rack

# Check if the default RACK_DIR exists, otherwise use the second option
ifneq ("$(wildcard $(RACK_DIR))","")
    RACK_DIR = C:/msys64/home/baylarovmurad/Rack
else
    RACK_DIR = C:/msys64/home/beyla/Rack
endif

# FLAGS will be passed to both the C and C++ compiler
FLAGS +=
CFLAGS +=
CXXFLAGS += -std=c++17 -IC:/Program\ Files\ \(x86\)/xsimd/include

# Careful about linking to shared libraries, since you can't assume much about the user's environment and library search path.
# Static libraries are fine, but they should be added to this plugin's build system.
LDFLAGS +=

# Add .cpp files to the build
SOURCES += $(wildcard src/*.cpp)

# Add files to the ZIP package when running `make dist`
# The compiled plugin and "plugin.json" are automatically added.
DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)
DISTRIBUTABLES += $(wildcard presets)

# Include the Rack plugin Makefile framework
include $(RACK_DIR)/plugin.mk
