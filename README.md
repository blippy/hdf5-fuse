# hdf5-fuse



## dunno

Accessing HDF5 files using FUSE


Unpacking a dataset of doubles using python:

    with open("doubles", "rb") as fp: bytes = fp.read()
    import struct
    vals = [ x[0] for x in struct.iter_unpack("d", bytes)]

Alternatively using UNIX command:

    od -t f8 <DATASET>

Possibly this has trouble with NAN's?

## logging

hdf5-fuse has some logging. It uses the syslog. If you are running systemd, you can view the logging "on the fly" by executing this command:

    journalctl -f | grep hdf5-fuse


