COMPILER_OPTIONS?=g++ -std=c++11 -O2 -Wall -Wpedantic -Wextra

ADDITIONAL_INCLUDE=-I..
COMPILER_ALL_OPTIONS=$(COMPILER_OPTIONS) -lpthread

BIN_DIR=./bin
DEPS_DIR=./deps
RAW_OBJECTS=$(patsubst ./cpp/%.cpp,%.o,$(shell ls ./cpp/*.cpp))
OBJECTS=$(patsubst %.o,$(BIN_DIR)/%.o,$(RAW_OBJECTS))
DEPS=$(patsubst %.o, $(DEPS_DIR)/%.d,$(RAW_OBJECTS))

.PHONY: build

build: $(OBJECTS)

$(DEPS): $(DEPS_DIR)/%.d: ./cpp/%.cpp | $(DEPS_DIR)
	$(COMPILER_ALL_OPTIONS) $(ADDITIONAL_INCLUDE) -MM $< | sed -e 1's,^,$(BIN_DIR)/,' > $@

$(DEPS_DIR):
	mkdir -p $@

$(OBJECTS): $(BIN_DIR)/%.o: ./cpp/%.cpp | $(BIN_DIR)
	$(COMPILER_ALL_OPTIONS) $(ADDITIONAL_INCLUDE) -o $@ -c $<

$(BIN_DIR):
	mkdir -p $@

include $(DEPS)

.PHONY: clean
clean:
	-rm $(OBJECTS) $(DEPS)
	-rmdir $(DEPS_DIR) $(BIN_DIR)
