
# Build Parameters
# Edit these parameters to comply with your system policy.
PREFIX=/usr/share
DATA_PATH=$(PREFIX)/qwinff

###############################

# Paths
PROJECT_ROOT=$(PWD)
SRC_DIR=$(PROJECT_ROOT)/src
BUILD_DIR=$(PROJECT_ROOT)/build
BIN_DIR=$(PROJECT_ROOT)/bin

# Tools
QMAKE=qmake

# Settings
QMAKE_DEFS= DESTDIR=$(BIN_DIR) \
            OBJECTS_DIR=$(BUILD_DIR)/obj \
            MOC_DIR=$(BUILD_DIR)/moc \
            RCC_DIR=$(BUILD_DIR)/rcc \
            UI_DIR=$(BUILD_DIR)/ui \
            DATA_PATH=$(DATA_PATH)

DEFS= DATA_PATH=\\\"$(DATA_PATH)\\\"

release:
	cd $(SRC_DIR) && $(QMAKE) $(QMAKE_DEFS) *.pro && $(DEFS) make

clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(BUILD_DIR)/*
	cd $(SRC_DIR) && make clean && rm Makefile

