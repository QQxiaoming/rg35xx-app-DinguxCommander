SRCS_DIR := /src
BUILD_DIR := /out

TARGET = main
C_SOURCES =  \
		${wildcard $(SRCS_DIR)/*.c}
CPP_SOURCES =  \
		${wildcard $(SRCS_DIR)/*.cpp}

CHAINPREFIX= /opt/miyoo
CROSS_COMPILE=$(CHAINPREFIX)/bin/arm-linux-

CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
STRIP = $(CROSS_COMPILE)strip
SYSROOT     := $(shell $(CC) --print-sysroot)
SDL_CFLAGS  := $(shell $(SYSROOT)/usr/bin/sdl-config --cflags)
SDL_LIBS    := $(shell $(SYSROOT)/usr/bin/sdl-config --libs)

OBJ_DIR := $(BUILD_DIR)/obj

DEFINES = -DHAVE_STDINT_H -DVERSION_BITTBOY
INCLUDES = -Iinclude $(SDL_CFLAGS)
OPT_FLAGS  = -Ofast -fdata-sections -fdata-sections -fno-common -fno-PIC -flto
EXTRA_LDFLAGS = -lasound -Wl,--as-needed -Wl,--gc-sections -flto -s

CFLAGS = $(DEFINES) $(INCLUDES) $(OPT_FLAGS) -std=gnu11 
CXXFLAGS = $(DEFINES) $(INCLUDES) $(OPT_FLAGS) -std=gnu++11 
LDFLAGS = -Wl,--start-group -lSDL -lSDL_image -lpng -ljpeg -lSDL_mixer -lfreetype -lSDL_ttf -logg -lvorbisidec -lm -pthread -lz -lstdc++ $(EXTRA_LDFLAGS) -Wl,--end-group

all: $(BUILD_DIR)/$(TARGET).elf

OBJECTS = $(addprefix $(OBJ_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(OBJ_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

$(OBJ_DIR)/%.o: %.c Makefile | $(OBJ_DIR)
	@echo CC $(notdir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: %.cpp Makefile | $(OBJ_DIR)
	@echo CXX $(notdir $@)
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	@echo LD $(notdir $@)
	@$(CC) $(OBJECTS) $(CFLAGS) $(LDFLAGS) -o $@

$(BUILD_DIR):
	mkdir $@

ifeq ($(OBJ_DIR), $(wildcard $(OBJ_DIR)))
else
$(OBJ_DIR):$(BUILD_DIR)
	mkdir $@
endif

clean:
	-rm -fR $(BUILD_DIR)
