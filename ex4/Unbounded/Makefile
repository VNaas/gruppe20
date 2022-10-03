# program executable name
TARGET = UnboundedPriorityInversion
# compiler flags
CFLAGS = -s -O3 -lpthread -std=gnu11 -g
# linker flags
LDFLAGS = -g -fsanitize=address
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