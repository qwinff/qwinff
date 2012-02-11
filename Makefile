
# Build Parameters
# You can change these parameters to customize the build.

# Installation Prefix
PREFIX=/usr
# Version ID String (displayed after the version string in the about dialog)
VIDSTR=

###############################
# Variables here are not meant to be changed.

# Paths
PROJECT_ROOT=$(PWD)
SRC_DIR=$(PROJECT_ROOT)/src
BUILD_DIR=$(PROJECT_ROOT)/build
BIN_DIR=$(PROJECT_ROOT)/bin
DATA_PATH=$(PREFIX)/share/qwinff
TRANSLATION_PATH=$(DATA_PATH)/translations

# Tools
QMAKE=qmake
LRELEASE=lrelease

# Settings
QMAKE_DEFS= DESTDIR=$(BIN_DIR) \
            OBJECTS_DIR=$(BUILD_DIR)/obj \
            MOC_DIR=$(BUILD_DIR)/moc \
            RCC_DIR=$(BUILD_DIR)/rcc \
            UI_DIR=$(BUILD_DIR)/ui

DEFS= DATA_PATH=\\\"$(DATA_PATH)\\\" \
      VERSION_ID_STRING=\\\"$(VIDSTR)\\\"

USE_LIBNOTIFY=0

ifneq ($(USE_LIBNOTIFY),0)
	QMAKE_DEFS += CONFIG+=libnotify
endif

release:
	+cd $(SRC_DIR) && $(QMAKE) $(QMAKE_DEFS) qwinff.pro && $(DEFS) make
	-cd src && $(LRELEASE) qwinff.pro

clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(BUILD_DIR)/*
	-cd $(SRC_DIR) && make clean && rm Makefile

install:
	-install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 bin/qwinff $(DESTDIR)$(PREFIX)/bin/
	-install -d $(DESTDIR)$(DATA_PATH)
	install -m 644 src/presets.xml $(DESTDIR)$(DATA_PATH)
	-install -d $(DESTDIR)$(TRANSLATION_PATH)
	-install -m 644 src/translations/*.qm $(DESTDIR)$(TRANSLATION_PATH)
	-install -d $(DESTDIR)$(PREFIX)/share/man/man1
	install -m 644 man/qwinff.1 $(DESTDIR)$(PREFIX)/share/man/man1/
	gzip -9 -f $(DESTDIR)$(PREFIX)/share/man/man1/qwinff.1
	
uninstall:
	-rm -f $(DESTDIR)$(PREFIX)/bin/qwinff
	-rm -f $(DESTDIR)$(DATA_PATH)/presets.xml
	-rm -f $(DESTDIR)$(TRANSLATION_PATH)/*.qm
	-rmdir $(DESTDIR)$(TRANSLATION_PATH)
	-rmdir $(DESTDIR)$(DATA_PATH)
	-rm -f $(DESTDIR)$(PREFIX)/share/man/man1/qwinff.1.gz
	-rm -f $(DESTDIR)$(PREFIX)/share/applications/qwinff.desktop
