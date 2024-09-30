CC := g++
CODEDIRS=.
INCDIRS=.

LDFLAGS := -g -lX11 
CFLAGS := -Wall -Wextra -g -std=c++20 -pthread $(foreach D,$(INCDIRS),-I$(D)) -MMD

CFILES = $(foreach D,$(CODEDIRS),$(wildcard $(D)/*.cpp))
OBJECTS = $(patsubst %.cpp,%.o,$(CFILES))
DEPENDS = $(OBJECTS:.o=.d)

EXECUTABLE := MOJO

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

# Include dependency files
-include $(DEPENDS)

clean: 
	rm -rf $(EXECUTABLE) $(OBJECTS) $(DEPENDS)
