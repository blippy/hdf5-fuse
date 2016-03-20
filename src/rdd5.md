rdd5(1) - read an HDF5 file as an array of doubles, and print it in human-readable form
====

## SYNOPSIS

`rdd5`

## DESCRIPTION

rdd5 reads from stdin in blocks of 8 bytes, interprets each block as a double (signed 8-byte float), and prints
each block in ASCII form, followed by a newline. A NaN is written as `nan'.

## EXAMPLE

Suppose you have mounted an HDF5 file in the ~/tmp directory using mkfs5, which contains a dataset DSET1 
of doubles. To dump the dataset in human-readable form, issue the command:

    rdd5 < ~/tmp/DSET1

## AUTHOR

Mark Carter >alt.mcarter@gmail.com>

