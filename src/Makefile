CC=gcc
CCFLAGS=-Wall -g
LDFLAGS= -lm -lhdf5 -L../../hdf5-1.12.0/src/.libs/
SOURCES=$(wildcard *.c)
OBJECTS=obj/$(SOURCES:.c=.o)
TARGET=hdf5_lookup

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CCFLAGS) -c -o obj/dimalloc3.o hdf5_helper.src/dimalloc3.c
	$(CC) $(CCFLAGS) -c -o obj/hdf5_helper.o hdf5_helper.src/hdf5_helper.c
	$(CC) $(CCFLAGS) -o ../$@ $^ $(LDFLAGS)

obj/%.o: %.c %.h
	$(CC) $(CCFLAGS) -c $< -o $@

obj/%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)
	rm -f obj/*.o
