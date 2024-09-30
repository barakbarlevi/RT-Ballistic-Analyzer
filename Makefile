# CC := g++
# CODEDIRS=.
# INCDIRS=.

# LDFLAGS := -g -lX11 
# CFLAGS := -Wall -Wextra -g -std=c++20 -pthread $(foreach D,$(INCDIRS),-I$(D)) -MMD

# CFILES = $(foreach D,$(CODEDIRS),$(wildcard $(D)/*.cpp))
# OBJECTS = $(patsubst %.cpp,%.o,$(CFILES))
# DEPENDS = $(OBJECTS:.o=.d)

# EXECUTABLE := MOJO

# all: $(EXECUTABLE)

# $(EXECUTABLE): $(OBJECTS)
# 	$(CC) -o $@ $^ $(LDFLAGS)

# %.o: %.cpp
# 	$(CC) -c -o $@ $< $(CFLAGS)

# # Include dependency files
# -include $(DEPENDS)

# clean: 
# 	rm -rf $(EXECUTABLE) $(OBJECTS) $(DEPENDS)

# Top-level Makefile

.PHONY: all clean rt_sendDetection MOJO SIX_DOF

# Define targets
all: rt_sendDetection MOJO SIX_DOF

rt_sendDetection:
	@echo "Building Part 1: rt_sendDetection"
	cd rt_sendDetection && make

MOJO:
	@echo "Building Part 2: MOJO"
	cd MOJO && make

SIX_DOF:
	@echo "Building Part 3: SIX_DOF"
	cd SIX_DOF && make

clean:
	@echo "Cleaning up..."
	cd rt_sendDetection && make clean
	cd MOJO && make clean
	cd SIX_DOF && make clean
