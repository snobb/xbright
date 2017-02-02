TARGET         := xbright
OBJDIR         := obj
SRC            := $(wildcard *.c)
OBJ            := $(addprefix $(OBJDIR)/,$(notdir $(SRC:.c=.o)))
CC             ?= cc
INSTALL        := install
INSTALL_ARGS   := -o root -g root -m 4755  # Installs with SUID set
INSTALL_DIR    := /usr/local/bin/
BUILD_HOST     := build_host.h

CFLAGS         := -Wall
LFLAGS         :=

ifeq ($(CC), $(filter $(CC), clang gcc cc))
	CFLAGS += -std=c99 -pedantic
endif

# Auto-configuration
BRIGHTNESSFILE=$(shell find /sys/class/backlight/*/brightness | head -1)
MAXVALUE=$(shell find /sys/class/backlight/*/max_brightness | head -1 | xargs cat)

# version info from git
REVCNT          := $(shell git rev-list --count master 2>/dev/null)
ifeq ($(REVCNT),)
    VERSION     := devel
else
    REVHASH     := $(shell git log -1 --format="%h" 2>/dev/null)
    VERSION     := "$(REVCNT).$(REVHASH)"
endif

all: debug

debug: CFLAGS += -g -DDEBUG
debug: LFLAGS += -g
debug: build

release: CFLAGS +=-Os
release: clean build
	strip $(TARGET)

build: $(OBJDIR) $(BUILD_HOST) $(TARGET)

# now the program gets autoconfigured in Makefile via $(BUILD_HOST)
$(BUILD_HOST):
	@echo "#define BUILD_HOST \"`hostname -f`\""          > $(BUILD_HOST)
	@echo "#define BRIGHTNESSFILE \"$(BRIGHTNESSFILE)\"" >> $(BUILD_HOST)
	@echo "#define MAXVALUE $(MAXVALUE)"                 >> $(BUILD_HOST)
	@echo "#define VERSION \"$(VERSION)\""               >> $(BUILD_HOST)

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

.PHONY : all debug release build clean objdir install uninstall
