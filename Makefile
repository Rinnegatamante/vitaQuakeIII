TARGET		:= vitaQuakeIII
TITLE		:= QUAK00003
GIT_VERSION := $(shell git describe --abbrev=6 --dirty --always --tags)

SOURCES  := code/rendercommon code/qcommon code/botlib code/client code/server code/renderergl1 code/psp2
INCLUDES := code/rendercommon code/qcommon code/botlib code/client code/server code/renderergl1 code/psp2
EXTRA_FILES := code/sys/con_log.c

LIBS = -lvitaGL -lvorbisfile -lvorbis -logg  -lspeexdsp -lmpg123 \
	-lc -lSceCommonDialog_stub -lSceAudio_stub -lSceLibKernel_stub \
	-lSceNet_stub -lSceNetCtl_stub -lpng -lz -lSceDisplay_stub -lSceGxm_stub \
	-lSceSysmodule_stub -lSceCtrl_stub -lSceTouch_stub -lSceMotion_stub -lm -lSceAppMgr_stub \
	-lSceAppUtil_stub -lScePgf_stub -ljpeg -lSceRtc_stub -lScePower_stub	

CFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c)) $(EXTRA_FILES)
CPPFILES   := $(foreach dir,$(CPPSOURCES), $(wildcard $(dir)/*.cpp))
BINFILES := $(foreach dir,$(DATA), $(wildcard $(dir)/*.bin))
OBJS     := $(addsuffix .o,$(BINFILES)) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o)

export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(dir))

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = $(INCLUDE) -D__PSP2__ -D__FLOAT_WORD_ORDER=1 -D__GNU__ \
        -DUSE_ICON -DARCH_STRING=\"arm\" -DBOTLIB -DUSE_CODEC_VORBIS \
        -DNO_VM_COMPILED -DDEFAULT_BASEDIR=\"ux0:/data/ioq3\" \
        -DPRODUCT_VERSION=\"1.36_GIT_ba68b99c-2018-01-23\" \
        -mfpu=neon -mcpu=cortex-a9 -march=armv7-a -mfloat-abi=hard -ffast-math \
        -fno-asynchronous-unwind-tables -funroll-loops \
        -mword-relocations -fno-unwind-tables -fno-optimize-sibling-calls \
        -mvectorize-with-neon-quad -funsafe-math-optimizations \
        -mlittle-endian -munaligned-access \
        -fsingle-precision-constant \
        -fno-strict-aliasing -Wimplicit -Wstrict-prototypes -pipe \
        -Wformat=2 -Wno-format-zero-length -Wformat-security -Wno-format-nonliteral -Wstrict-aliasing=2 \
        -Wmissing-format-attribute -Wdisabled-optimization -Werror-implicit-function-declaration \
		 -Wno-unused-variable -Wno-unused-but-set-variable
CXXFLAGS  = $(CFLAGS) -fno-exceptions -std=gnu++11
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

$(TARGET).vpk: $(TARGET).velf
	vita-make-fself -s $< build/eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE) "$(TARGET)" param.sfo
	cp -f param.sfo build/sce_sys/param.sfo

	#------------ Comment this if you don't have 7zip ------------------
	7z a -tzip ./$(TARGET).vpk -r ./build/sce_sys ./build/eboot.bin ./build/shaders
	#-------------------------------------------------------------------

%.velf: %.elf
	cp $< $<.unstripped.elf
	$(PREFIX)-strip -g $<
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).velf $(TARGET).elf $(OBJS) $(TARGET).elf.unstripped.elf $(TARGET).vpk build/eboot.bin build/sce_sys/param.sfo ./param.sfo
