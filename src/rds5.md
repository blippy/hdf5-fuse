rds5(1) - dump a string file from HDF5
====

## SYNOPSIS

'rds5' FILE

## DESCRIPTION

rds5 reads an hdf5-fuse mounted string file, and prints each string it finds to stdout



## NOTES

Unlike rdd5, rds5 can only be used on files that are mounted under hdf5-fuse. This
is because rds5 needs information about the width of a string, which it can only
obtain from hdf5-fuse.


## AUTHOR

Mark Carter <alt.mcarter@gmail.com>

## SEE ALSO

hdf5-fuse

