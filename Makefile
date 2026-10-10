.SUFFIXES:

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. Set up devkitPro first.")
endif
ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. Set up devkitARM first.")
endif

GAME_TITLE := DRAGONCASTLE
GAME_CODE := DGC0
MAKER_CODE := 01
include $(DEVKITARM)/gba_rules

TARGET := dragoncastle
BUILD := build
SOURCES := src
INCLUDES :=
ARCH := -mthumb -mthumb-interwork
CFLAGS := -g -Wall -Wextra -O2 -mcpu=arm7tdmi -mtune=arm7tdmi -fomit-frame-pointer $(ARCH)
ASFLAGS := $(ARCH)
LDFLAGS = -g $(ARCH) -Wl,-Map,$(notdir $*.map)
LIBS :=
LIBDIRS :=

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)
export PATH := $(DEVKITARM)/bin:$(PATH)
CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
ifeq ($(strip $(CPPFILES)),)
export LD := $(CC)
else
export LD := $(CXX)
endif
export OFILES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) -I$(CURDIR)/$(BUILD)
export LIBPATHS :=
.PHONY: all clean
all: $(BUILD)
$(BUILD):
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
clean:
	@rm -rf $(BUILD) $(TARGET).elf $(TARGET).gba $(TARGET).map
else
DEPENDS := $(OFILES:.o=.d)
$(OUTPUT).gba: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)
-include $(DEPENDS)
endif
