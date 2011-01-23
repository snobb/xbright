TARGET = xbright
SOURCES = $(wildcard *.c)
OBJECTS = $(subst .c$,.o,$(SOURCES))
HEADERS = $(wildcard *.h)
CC = gcc
LINK = gcc
STRIP = strip
INSTALL = install
INSTALL_ARGS = -o root -g wheel -m 4755  # Installs with SUID set
INSTALL_DIR = /usr/local/bin/

# Autoconfiguration
BRIGHTNESSFILE=`find /proc/acpi/video/ -iname brightness -exec grep -H current {} \; | awk -F ":" '{ print $$1 }'`
SRCHSTR=`head -1 $$(cat .brightness_file.tmp) | awk '{ print $$1 }'`
LEVELS=`head -1 $$(cat .brightness_file.tmp) | awk '{ print $$2", "$$3", "$$4", "$$5", "$$6", "$$7", "$$8", "$$9 }'`

-include .depend

$(TARGET): build_host.h $(OBJECTS)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBPATH) $(LIBS)
	$(STRIP) $(TARGET)

.c.o: $*.h common.h
	$(CC) -c $(CFLAGS) $(DEBUGFLAGS) $(INCPATH) -o $@ $<


# now the program gets autoconfigured in Makefile via build_host.h
build_host.h:
	@echo -n "Generating configuration: "
	@echo "#define BUILD_HOST \"`hostname -f`\"" > build_host.h;
	@echo ${BRIGHTNESSFILE} > .brightness_file.tmp
	@echo "#define BRIGHTNESSFILE \"`cat .brightness_file.tmp`\"" >> build_host.h
	@echo "#define SRCHSTR \"${SRCHSTR}\"" >> build_host.h
	@echo "const unsigned short levels[] = {${LEVELS}};" >> build_host.h
	@rm -f .brightness_file.tmp
	@echo "DONE"

install: $(TARGET)
	$(INSTALL) $(INSTALL_ARGS) $(TARGET) $(INSTALL_DIR)
	@echo "DONE"

uninstall:
	-rm -f $(INSTALL_DIR)$(TARGET)
	@echo "DONE"

clean:
	-rm -f .depend
	-rm -f $(OBJECTS)
	-rm -f *~ core *.core
	-rm -f version.h
	-rm -f $(TARGET)
	-rm -f build_host.h

depend:
.depend: Makefile $(SOURCES) $(HEADERS)
	@if [ ! -f .depend ]; then touch .depend; fi
	@makedepend -Y -f .depend  $(SOURCES) 2>/dev/null
