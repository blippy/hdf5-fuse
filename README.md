# hdf5-fuse


HDF5-FUSE provides a FUSE driver and suite of programs for maniuplating HDF5 files. The programs mimic standard Linux filesystem
utilities, and are likely to be useful in their own right, even if you decide not to use the FUSE driver.

The utility programs are:

* ```hdf5``` - mount an HDF5 file as a FUSE system
* ```mkfs5``` - create an HDF5 file
* ```rdd5``` - read as doubles
* ```rdp5''' - read strings



## Quickstart

Create a mount point (e.g. ~/tmp) for your HDF5 file:

    mkdir ~/tmp

Mount your HDF5 file (e.g. my.h5) to the mount point:

    hdf5 ~/tmp my.h5

Explore the structure of the HDF5 file:

    cd ~/tmp
    ls
    cd mygroup1 # etc


Suppose DSET1 is a dataset containing information you are interested in. 
You can view a raw dump of its contents by issuing the standard Linux command:

    xxd DSET1

That may not be particularly helpful, though. If you know that DSET1 is an array of doubles, 
then you can view it in text form, one line per value:

    rdd5 < DSET1

If DSET1 is a dataset of strings, then they can be dumped out, one line per string:

    rdp5 DSET

Note that you cannot use redirection from stdin using rds5, because meta-information is required

## Attributes

There is currently fledgling support for reading file attributes. User-defined attributes cannot (yet) be
inspected. No attributes can be written.

Attributes are obtained by running the standard UNIX command getfattr. Atrributes recognised are:
* ```@T``` - the type of the dataset; either ```S``` for string, ```D``` for double, or ```?``` otherwise
* ```@N``` - the number of elements/points in a dataset
* ```@W``` - width; the max num of chars in a string, assumed 8 for a double, or ```?``` otherwise

Example usage:

    getfattr -n @T file

Notes that the ```@``` is reserved for properties of the dataset itself. You can never set them, because
the dataset properties are fixed upon creation. The prefix ```~``` will probably be used for
user-definable properties.

## logging

hdf5-fuse has some logging. It uses the syslog. If you are running systemd, you can view the logging "on the fly" by executing this command:

    journalctl -f | grep hdf5-fuse


