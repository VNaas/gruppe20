# program executable name
TARGET = raceCondition
# compiler flags
CFLAGS = -s -O3 -lpthread -std=gnu11 -g -lrt
# linker flags
LDFLAGS = -g -fsanitize=thread
# list of sources
SOURCES = $(shell find -name "*.c")
# default rule, to compile everything
all: $(TARGET)
# define object files
OBJECTS = $(SOURCES:.c=.o)
# link programs
$(TARGET): $(OBJECTS)
	clang $(LDFLAGS) -o $@ $^
# compile
%.o : %.c
	clang $(CFLAGS) -c -o $@ $<
# cleaning
clean:
	rm -f $(TARGET) $(OBJECTS)