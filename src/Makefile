# backend config:

# host backends (separated with spaces). must have at least one!
HBACKEND=sdl2
# video backends (separated with spaces). must have at least one!
VBACKEND=sdl2
# input backends (separated with spaces). must have at least one!
IBACKEND=sdl2
# audio backends (separated with spaces)
ABACKEND=sdl2

# main makefile

CXX ?= g++
LD := $(CXX)
RM=rm -f

# release flags:
CXXFLAGS=-Ofast -DNDEBUG
# debug flags:
#CXXFLAGS=-g3 -O0 -Wall -Wextra -Werror -Wno-unused-parameter

# the rest
CXXFLAGS := -std=c++17 $(CXXFLAGS) -MMD -MP
LDFLAGS=
LDLIBS=-lm
OBJS=
SUBDIRS=base render main menu game

TARGET := ../hiemalia
IROOT := ../includes

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
