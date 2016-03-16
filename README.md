# hdf5-fuse

Accessing HDF5 files using FUSE


Unpacking a dataset of doubles using python:

    with open("doubles", "rb") as fp: bytes = fp.read()
    import struct
    vals = [ x[0] for x in struct.iter_unpack("d", bytes)]

Alternatively using UNIX command:

    od -t f8 <DATASET>

Possibly this has trouble with NAN's?
