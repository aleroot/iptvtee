CXX      := -c++
CXXFLAGS := -std=c++20 -pthread
LDFLAGS  := -L/usr/lib -lstdc++ -lvlc -lcurl
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/app
TARGET   := iptvtee
INCLUDE  := -Ilibs/ -I$(TARGET)/iptv/ -I$(TARGET)/utils/
SRC      := $(wildcard iptvtee/*.cpp) $(wildcard iptvtee/*/*.cpp) $(wildcard iptvtee/*/*/*.cpp)

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES \
         := $(OBJECTS:.o=.d)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)
	
ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

-include $(DEPENDENCIES)

.PHONY: all build clean debug release info install

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*

info:
	@echo "[*] Application dir: ${APP_DIR}     "
	@echo "[*] Object dir:      ${OBJ_DIR}     "
	@echo "[*] Sources:         ${SRC}         "
	@echo "[*] Objects:         ${OBJECTS}     "
	@echo "[*] Dependencies:    ${DEPENDENCIES}"

install:
	install -d $(PREFIX)/bin/
	install -m 755 $(BIN)/$(EXECUTABLE) $(PREFIX)/bin/
