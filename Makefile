
# Build Parameters
# Edit these parameters to comply with your system policy.
PREFIX=/usr/local

###############################

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
            UI_DIR=$(BUILD_DIR)/ui \
            DATA_PATH=$(DATA_PATH)

DEFS= DATA_PATH=\\\"$(DATA_PATH)\\\"

bin/qwinff:
	+cd $(SRC_DIR) && $(QMAKE) $(QMAKE_DEFS) qwinff.pro && $(DEFS) make
	-cd src && $(LRELEASE) qwinff.pro

clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(BUILD_DIR)/*
	cd $(SRC_DIR) && make clean && rm Makefile

install: bin/qwinff
	-install -d $(PREFIX)/bin/
	install -m 755 bin/qwinff $(PREFIX)/bin/
	-install -d $(DATA_PATH)
	install -m 644 src/presets.xml $(DATA_PATH)
	-install -d $(TRANSLATION_PATH)
	-install -m 644 src/translations/*.qm $(TRANSLATION_PATH)
	
uninstall:
	-rm -f $(PREFIX)/bin/qwinff
	-rm -f $(DATA_PATH)/presets.xml
	-rm -f $(TRANSLATION_PATH)/*.qm
	-rmdir $(TRANSLATION_PATH)
	-rmdir $(DATA_PATH)
