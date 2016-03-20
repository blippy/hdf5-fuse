#.PHONY: clean

bin_PROGRAMS = hdf5 create5 mkfs5 rdd5

CFLAGS = -Wall -Wextra $(shell pkg-config --cflags fuse) -std=gnu99 -ggdb
#LDLIBS =-lhdf5 $(shell pkg-config --libs fuse)
#TARGET = hdf5
LDFLAGS = -lhdf5
mkfs5_LDFLAGS = -lhdf5
hdf_SOURCES = driver.c
create5_SOURCES = create5.c
mkfs5_SOURCES = mkfs5.c

rdd5_SOURCES = rdd5.c
rdd5_LDFLAGS = -lm


#hdf5.o : driver.c

hdf5: driver.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

#clean:
#	$(RM) $(TARGET)
