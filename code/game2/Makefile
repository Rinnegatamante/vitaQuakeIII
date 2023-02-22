TARGET   := qagame
SOURCES  := .
CPPSOURCES := 
INCLUDES := ../renderercommon ../renderergl1 ../qcommon ../botlib ../client ../server ../psp2 ../sys ../game
SOURCE_EXTRA := ../psp2/psp2_dll_hacks.c

LIBS = -lc -lm -lSceNet_stub_weak -lSceNetCtl_stub_weak \
       -lSceSysmem_stub_weak -lSceSysmodule_stub_weak -lSceIofilemgr_stub_weak \
       -lSceLibKernel_stub_weak -lSceKernelThreadMgr_stub_weak -lSceKernelModulemgr_stub_weak

CFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c)) $(SOURCE_EXTRA)
CPPFILES   := $(foreach dir,$(CPPSOURCES), $(wildcard $(dir)/*.cpp))
BINFILES := $(foreach dir,$(DATA), $(wildcard $(dir)/*.bin))
OBJS     := $(addsuffix .o,$(BINFILES)) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(dir))

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = $(INCLUDE) -D__PSP2__ -D__FLOAT_WORD_ORDER=1 -D__GNU__ -DRELEASE \
        -DUSE_ICON -DARCH_STRING=\"arm\" -DUSE_CODEC_VORBIS \
        -fno-lto -DQAGAME -DMISSIONPACK \
        -DPRODUCT_VERSION=\"1.36_GIT_ba68b99c-2018-01-23\" -DHAVE_VM_COMPILED=true \
        -mfpu=neon -mcpu=cortex-a9 -fsigned-char -nostartfiles \
        -Wl,-q -O3 -g -ffast-math -fno-short-enums -include "../psp2/psp2_dll_defs.h"
CXXFLAGS  = $(CFLAGS) -fno-exceptions -std=gnu++11 -fpermissive
ASFLAGS = $(CFLAGS)

all: $(TARGET).suprx

$(TARGET).suprx: $(TARGET).velf
	vita-make-fself -s $< $@

%.velf: %.elf
	cp $< $<.unstripped.elf
	$(PREFIX)-strip -g $<
	vita-elf-create -e $(<:.elf=.yml) $< $@

$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).velf $(TARGET).elf $(OBJS) $(TARGET).elf.unstripped.elf $(TARGET).suprx
