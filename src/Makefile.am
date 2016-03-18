#.PHONY: clean

bin_PROGRAMS = hdf5

CFLAGS = -Wall -Wextra $(shell pkg-config --cflags fuse) -std=gnu99
LDLIBS =-lhdf5 $(shell pkg-config --libs fuse)
TARGET = hdf5
hdf_SOURCES = driver.c

#hdf5.o : driver.c

hdf5: driver.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

#clean:
#	$(RM) $(TARGET)
