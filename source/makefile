all: vis

#CXX=g++-10.2.0
# -Og -g3 -ggdb 
# -Wno-error=inaccessible-base
# -Wshadow -Wdouble-promotion -fno-common -Wconversion 
CXXFLAGS=-Og -ggdb3 -pipe \
-std=gnu++2a \
-march=native -mtune=native \
-Wall -Wextra -Werror -Wfatal-errors \
-ffunction-sections -fdata-sections \
-Wno-error=unused-variable \
-Wno-error=unused-parameter \
-Wno-error=double-promotion \
-Wno-error=catch-value= \
-Wno-error=sign-compare

LDFLAGS= \
-L/usr/local/lib \
-Wl,--gc-sections \
-Wl,--print-gc-sections

CFILES:=$(shell find *.cpp| grep .cpp$)
OBJ:=$(CFILES:%.cpp=%.o)

depend: .depend

.depend: $(CFILES)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^ > ./.depend

include .depend

all: vis

%.o: %.cpp globals.h
	$(CXX) -c -o $@ $< $(CXXFLAGS)

vis: $(OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS)

clean:
	rm vis *.o .depend
