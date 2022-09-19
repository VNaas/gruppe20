# program executable name
TARGET = Scheduling
# compiler flags
CFLAGS = -g -Wall -pthread -std=gnu11 
# linker flags
LDFLAGS = -g -pthread
# list of sources
SOURCES = $(shell find -name "*.c")
# default rule, to compile everything
all: $(TARGET)
# define object files
OBJECTS = $(SOURCES:.c=.o)
# link programs
$(TARGET): $(OBJECTS)
	gcc $(LDFLAGS) -o $@ $^
# compile
%.o : %.c
	gcc $(CFLAGS) -c -o $@ $<
# cleaning
clean:
	rm -f $(TARGET) $(OBJECTS)