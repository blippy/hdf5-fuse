# hdf5-fuse


HDF5-FUSE provides a FUSE driver and suite of programs for maniuplating HDF5 files. The programs mimic standard Linux filesystem
utilities, and are likely to be useful in their own right, even if you decide not to use the FUSE driver.

The utility programs are:

* ```mkfs5``` - create an HDF5 file



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


