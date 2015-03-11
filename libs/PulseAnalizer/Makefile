CXX_FLAGS := -O2 -fPIC
LIB_INC = include
LINKDEF_FILE = LinkDef.h

INC_FILES := $(wildcard include/*.h)
CXX_FILES := $(wildcard src/*.cxx)
OBJ_FILES := $(addprefix obj/,$(notdir $(CXX_FILES:.cxx=.o)))

LIB_NAME = PulseAnalizer

all: lib$(LIB_NAME).so

clean:
	rm lib$(LIB_NAME).so obj/*o MyDict.cxx MyDict.h

MyDict.cxx: $(INC_FILES) $(LINKDEF_FILE)
	rootcint -f $@ -c $(CXX_FLAGS) -p $^

obj/MyDict.o: MyDict.cxx
	g++ $(CXX_FLAGS) `root-config --cflags` -c -o $@ $<

obj/%.o: src/%.cxx
	g++ $(CXX_FLAGS) `root-config --cflags` -c -o $@ $<

lib$(LIB_NAME).so: $(OBJ_FILES) obj/MyDict.o
	g++ -shared -o $@ `root-config --cflags` -I$(LIB_INC) $^
