COMPILER?=g++
CPP_STANDARD?=-std=c++11
OPTIMIZATION?=-O2
WARNING_LEVEL?=-Wall -Wpedantic -Wextra # -Werror
PREDEFINED_MACRO?=
ADDITIONAL_OPTIONS?=
COMPILER_OPTIONS=$(COMPILER) $(CPP_STANDARD) $(OPTIMIZATION) $(WARNING_LEVEL) $(PREDEFINED_MACRO) $(ADDITIONAL_OPTIONS)

PROJECTS=tmsocket server client

.PHONY: build
build: ./build/bin/server.out ./build/bin/client.out

./build/bin/server.out : ./build/bin/libtmsocket.a $(PROJECTS)
	$(COMPILER_OPTIONS) -o $@ ./src/server/bin/*.o -L./build/bin -ltmsocket -lpthread

./build/bin/client.out : ./build/bin/libtmsocket.a $(PROJECTS)
	$(COMPILER_OPTIONS) -o $@ ./src/client/bin/*.o -L./build/bin -ltmsocket -lpthread

./build/bin/libtmsocket.a : $(PROJECTS)
	mkdir -p ./build
	mkdir -p ./build/bin
	ar -o $@ -cr ./src/tmsocket/bin/*.o

.PHONY: $(PROJECTS)
$(PROJECTS):
	make -C ./src/$@ "COMPILER_OPTIONS=$(COMPILER_OPTIONS)"

CLEAN_PROJECTS=$(patsubst %,clean_%,$(PROJECTS))

.PHONY: clean, CLEAN_PROJECTS
clean: $(CLEAN_PROJECTS)
	-rm -rf ./build

$(CLEAN_PROJECTS):
	make -C $(patsubst clean_%,./src/%,$@) clean
