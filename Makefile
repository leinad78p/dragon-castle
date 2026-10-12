# Dragon Castle — mixed C/C++ GBA build using the official devkitARM rules.
.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM to the devkitARM installation path")
endif

include $(DEVKITARM)/gba_rules

TARGET := dragoncastle
BUILD := build
SOURCES := src
INCLUDES := src

ARCH := -mcpu=arm7tdmi -mtune=arm7tdmi -mthumb-interwork -marm
COMMON_FLAGS := -g -Wall -Wextra -O2 $(ARCH) -ffunction-sections -fdata-sections
CFLAGS := $(COMMON_FLAGS) -std=gnu99
CXXFLAGS := $(COMMON_FLAGS) -std=gnu++11 -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit
LDFLAGS = -g $(ARCH) -Wl,-Map,$(notdir $@).map -Wl,--gc-sections
LIBS :=

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export PATH := $(DEVKITARM)/bin:$(PATH)
export DEPSDIR := $(CURDIR)/$(BUILD)

# Deliberately list only the new files: the retired src/main.c stays out of the build.
CFILES := gfx.c sound.c
CPPFILES := game.cpp
SFILES :=

ifeq ($(strip $(CPPFILES)),)
export LD := $(CC)
else
export LD := $(CXX)
endif

export OFILES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) -I$(CURDIR)/$(BUILD)
export LIBPATHS :=

.PHONY: all $(BUILD) clean
all: $(BUILD) $(TARGET).gba

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo "clean ..."
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).gba $(TARGET).map
else
DEPENDS := $(OFILES:.o=.d)

$(OUTPUT).gba: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

-include $(DEPENDS)
endif
