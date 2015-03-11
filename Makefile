#
# for explanation of '$<', '$^' and '$@' symbols look at 
# https://www.gnu.org/software/make/manual/make.html#Automatic-Variables
#

ROOT_FLAGS := `root-config --cflags --libs`
CFLAGS := -O3 -I/usr/include/root/ -lstdc++ -std=c++11 -g -Wall -Ilibs/include/
LIBS := -Llibs -lFadcEvent -lMyServiceFunctions -lBoostServiceFunctions -lPulseAnalizer -lboost_program_options -lboost_system -lboost_filesystem -fopenmp
CC := g++

CXX_FILES := $(wildcard src/*.cxx)
exec := $(notdir $(CXX_FILES:.cxx=.exe))
includes := $(wildcard include/*.h)

exec_with_incl := $(notdir $(includes:.h=.exe))
exec_without_incl := $(filter-out $(exec_with_incl),$(exec)) # subtract one list from another

all: $(exec_with_incl) $(exec_without_incl)

$(exec_with_incl): %.exe: src/%.cxx include/%.h
	$(CC) $(CFLAGS) $(ROOT_FLAGS) $(LIBS) -Iinclude/ $< -o $@

$(exec_without_incl): %.exe: src/%.cxx
	$(CC) $(CFLAGS) $(ROOT_FLAGS) $(LIBS) $< -o $@

.PHONY: clean

clean:
	rm -f *~ 
