#XENO_CONFIG := /usr/xenomai/bin/xeno-config

CC := g++
CODEDIRS=.
INCDIRS=.

#CFLAGS := $(shell $(XENO_CONFIG)  --posix --alchemy --cflags) $(foreach D,$(INCDIRS),-I$(D))
#LDFLAGS := $(shell $(XENO_CONFIG)  --posix --alchemy --ldflags --auto-init-solib)

#CFLAGS := $(foreach D,$(INCDIRS),-I$(D)) // ORIG
LDFLAGS := -g

CFLAGS :=-Wall -Wextra -g $(foreach D,$(INCDIRS),-I$(D))


CFILES = $(foreach D,$(CODEDIRS),$(wildcard $(D)/*.cpp))
OBJECTS = $(patsubst %.cpp,%.o,$(CFILES))

EXECUTABLE := SIX_DOF

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
		echo $@
		echo $<
		$(CC) -c -o $@ $< $(CFLAGS)

clean: 
		rm -rf $(EXECUTABLE) $(OBJECTS)
