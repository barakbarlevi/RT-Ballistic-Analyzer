CC := g++
#CODEDIRS=.
#INCDIRS=.
CODEDIRS := src
INCDIRS := include

#CFLAGS := $(foreach D,$(INCDIRS),-I$(D)) // ORIG
LDFLAGS := -g

CFLAGS :=-Wall -Wextra -g $(foreach D,$(INCDIRS),-I$(D))


#CFILES = $(foreach D,$(CODEDIRS),$(wildcard $(D)/*.cpp))
CFILES := $(wildcard $(CODEDIRS)/*.cpp)
OBJECTS = $(patsubst %.cpp,%.o,$(CFILES))

EXECUTABLE := ../CADAC_6DOF_BINARY

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

#%.o: %.cpp
$(CODEDIRS)/%.o: $(CODEDIRS)/%.cpp
		echo $@
		echo $<
		$(CC) -c -o $@ $< $(CFLAGS)

clean: 
		rm -rf $(EXECUTABLE) $(OBJECTS)
