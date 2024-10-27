CC := g++
#CODEDIRS=.
#INCDIRS=.
CODEDIRS := src
INCDIRS := include

LDFLAGS := -g -lX11 
CFLAGS := -Wall -Wextra -g -std=c++20 -pthread $(foreach D,$(INCDIRS),-I$(D)) -MMD

#CFILES = $(foreach D,$(CODEDIRS),$(wildcard $(D)/*.cpp))
CFILES := $(wildcard $(CODEDIRS)/*.cpp)

OBJECTS = $(patsubst %.cpp,%.o,$(CFILES))
DEPENDS = $(OBJECTS:.o=.d)

EXECUTABLE := ../MOJO_BINARY

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

#%.o: %.cpp
$(CODEDIRS)/%.o: $(CODEDIRS)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

# Include dependency files
-include $(DEPENDS)

clean: 
	rm -rf $(EXECUTABLE) $(OBJECTS) $(DEPENDS)
