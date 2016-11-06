TARGET      := xbright
OBJDIR      := obj
SRC         := $(wildcard *.c)
OBJ         := $(addprefix $(OBJDIR)/,$(notdir $(SRC:.c=.o)))
CC          ?= cc
BUILD_HOST  := build_host.h

INSTALL = install
INSTALL_ARGS = -o root -g wheel -m 4755  # Installs with SUID set
INSTALL_DIR = /usr/local/bin/

INCLUDES    :=
LIBS        :=

CFLAGS      := -Wall $(INCLUDES)
LFLAGS      := $(LIBS)

ifeq ($(CC), $(filter $(CC), clang gcc cc))
	CFLAGS += -std=c99 -pedantic
endif

# Autoconfiguration
BRIGHTNESSFILE=`find /sys/devices/|grep "video0/brightness" | tail -1`
MAXVALUE=`find /sys/devices/ | grep "video0/max_brightness" | tail -1 | xargs tail -1`

all: debug

debug: CFLAGS += -g -DDEBUG
debug: LFLAGS += -g
debug: build

release: CFLAGS +=-Os
release: clean build
	strip $(TARGET)

build: $(OBJDIR) $(BUILD_HOST) $(TARGET)

$(BUILD_HOST):
	@echo "#define BUILD_HOST \"`hostname`\""             > $(BUILD_HOST)
	@echo "#define BUILD_OS \"`uname`\""                 >> $(BUILD_HOST)
	@echo "#define BUILD_PLATFORM \"`uname -m`\""        >> $(BUILD_HOST)
	@echo "#define BUILD_KERNEL \"`uname -r`\""          >> $(BUILD_HOST)
	@echo "#define BRIGHTNESSFILE \"${BRIGHTNESSFILE}\"" >> $(BUILD_HOST)
	@echo "#define MAXVALUE ${MAXVALUE}"                 >> $(BUILD_HOST)

$(TARGET): $(BUILD_HOST) $(OBJ)
	$(CC) $(LFLAGS) -o $@ $(OBJ)

$(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $?

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	-rm -f *.core
	-rm -f $(BUILD_HOST)
	-rm -f $(TARGET)
	-rm -rf ./$(OBJDIR)

install: release
	$(INSTALL) $(INSTALL_ARGS) $(TARGET) $(INSTALL_DIR)
	@echo "DONE"

uninstall: clean
	-rm -f $(INSTALL_DIR)$(TARGET)
	@echo "DONE"

.PHONY : all debug release build run clean objdir install uninstall
