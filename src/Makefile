# backend config: 

# host backends (separated with spaces). must have at least one!
HBACKEND=sdl2
# video backends (separated with spaces). must have at least one!
VBACKEND=sdl2
# input backends (separated with spaces). must have at least one!
IBACKEND=sdl2
# audio backends (separated with spaces)
ABACKEND=
# sdlmixer

# main makefile

CXX=g++
LD=g++
RM=rm -f
CXXFLAGS=-std=c++17 -g3 -O0 -Wall -Werror -MMD -MP
LDFLAGS=
LDLIBS=
OBJS=
SUBDIRS=base main

ROOT := $(realpath $(dir $(realpath $(firstword $(MAKEFILE_LIST)))))
TARGET := $(ROOT)/../hiemalia
IROOT := $(ROOT)/../includes

CXXFLAGS := -I$(IROOT) $(CXXFLAGS)
BASECXXFLAGS := $(CXXFLAGS) $(addprefix -DHBACKEND_, $(HBACKEND)) \
					    $(addprefix -DVBACKEND_, $(VBACKEND)) \
					    $(addprefix -DIBACKEND_, $(IBACKEND)) \
					    $(addprefix -DABACKEND_, $(ABACKEND))

include $(addsuffix /Makefile.inc, $(SUBDIRS))

DEPS := $(OBJS:.o=.d)

default: all

.PHONY: all clean
all: $(TARGET)
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)

base/%.o: CXXFLAGS := $(BASECXXFLAGS)
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

-include $(DEPS)
