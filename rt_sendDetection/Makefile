CC := g++
CODEDIRS=.
INCDIRS=.

LDFLAGS := -g -lX11 
CFLAGS := -Wall -Wextra -g -std=c++20 -pthread $(foreach D,$(INCDIRS),-I$(D)) -MMD

CFILES = $(foreach D,$(CODEDIRS),$(wildcard $(D)/*.c))
OBJECTS = $(patsubst %.c,%.o,$(CFILES))
DEPENDS = $(OBJECTS:.o=.d)

EXECUTABLE := rt_sendDetection

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

# Include dependency files
-include $(DEPENDS)

clean: 
	rm -rf $(EXECUTABLE) $(OBJECTS) $(DEPENDS)
