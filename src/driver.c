/*
 * hdf5-fuse: fuse wrapper around the hdf5 file format
 */

#define FUSE_USE_VERSION 26

#include <errno.h>
#include <fuse.h>
#include <hdf5.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <stdarg.h>

#define STREQ(x, y) (strcmp(x, y) == 0)

hid_t root_group = -1;

size_t hdf5_fuse_filesize(const char* path)
{
  hid_t dataset = H5Dopen(root_group, path, H5P_DEFAULT);
  if(dataset < 0)
    return 0;

  hid_t datatype = H5Dget_type(dataset);
  hid_t dataspace = H5Dget_space(dataset);
  size_t type_size = H5Tget_size(datatype);
  size_t num_elems = H5Sget_simple_extent_npoints(dataspace);
  H5Sclose(dataspace);
  H5Dclose(dataset);
  return num_elems * type_size;
}

static int hdf5_fuse_getattr(const char* path, struct stat *stbuf)
{
	syslog(LOG_DEBUG, "getattr path %s", path);
	memset(stbuf, 0, sizeof(struct stat));

	H5O_info_t obj_info;
	if(H5Oget_info_by_name(root_group, path, &obj_info, H5P_DEFAULT) < 0)
		return -ENOENT;

	if(obj_info.type == H5O_TYPE_GROUP) {
		stbuf->st_mode = S_IFDIR | 0777; // mcarter 0555; need to factor in if system is read-only
		H5G_info_t group_info;
		H5Gget_info_by_name(root_group, path, &group_info, H5P_DEFAULT);
		stbuf->st_nlink = 2 + group_info.nlinks;
		stbuf->st_size = group_info.nlinks;
	} else if (obj_info.type == H5O_TYPE_DATASET) {
		stbuf->st_mode = S_IFREG | 0666; // mcarter changed from  0444; but will prolly need to change it to something sensible
		stbuf->st_size = hdf5_fuse_filesize(path);
	} else {
		stbuf->st_mode = S_IFCHR | 0000;
		stbuf->st_size = 0;
	}

	return 0;
}

// implemented by mcarter
static int hdf5_fuse_mkdir(const char *path, mode_t mode  __attribute__ ((unused)))
{
	hid_t gid = H5Gcreate2(root_group, path, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	herr_t status = H5Gclose(gid);
	syslog(LOG_DEBUG, "mkdir %s %d", path, status);
	return status; // 0 success, -1 failure, exactly as mkdir

}
static int hdf5_fuse_readdir(const char* path, void* buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi)
{
  (void) offset;
  (void) fi;

  H5G_info_t group_info;
  if(H5Gget_info_by_name(root_group, path, &group_info, H5P_DEFAULT) < 0)
    return -ENOENT;

  filler(buf, ".", NULL, 0);
  filler(buf, "..", NULL, 0);

  for(hsize_t i = 0; i < group_info.nlinks; ++i) {
    char name[128];
    H5Lget_name_by_idx(root_group, path,
        H5_INDEX_NAME, H5_ITER_INC, i, name, 128, H5P_DEFAULT);
    filler(buf, name, NULL, 0);
  }

  return 0;
}

static int hdf5_fuse_open(const char *path, struct fuse_file_info *fi)
{
	syslog(LOG_DEBUG, "open");
	(void)fi;
	//if((fi->flags & 3) != O_RDONLY) return -EACCES; // mcarter decided to relax read-only restrictions

  H5O_info_t obj_info;
  if(H5Oget_info_by_name(root_group, path, &obj_info, H5P_DEFAULT) < 0)
    return -ENOENT;

  return 0;
}

static int hdf5_fuse_read(const char *path, char *buf, size_t size, off_t offset,
    struct fuse_file_info *fi)
{
	(void) fi;
	syslog(LOG_DEBUG, "read: path %s, size %zu, offset %zu", path, size, (size_t)offset);

	hid_t dataset = H5Dopen(root_group, path, H5P_DEFAULT);
	hid_t datatype = H5Dget_type(dataset);
	size_t buf_size = hdf5_fuse_filesize(path);
	char *hdf5_buf = malloc(buf_size);
	H5Dread(dataset, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, hdf5_buf);
	size_t copy_size = buf_size - offset < size ? buf_size - offset : size;
	memcpy(buf, hdf5_buf+offset, copy_size);
	free(hdf5_buf);
	H5Dclose(dataset);
	return copy_size;
}

static int hdf5_fuse_write(const char *path, const char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	(void)path;
	(void)buf;
	(void)size;
	(void)offset;
	(void)fi;

	// this algorithm is likely to be very inefficient. Consider using slabs
	syslog(LOG_DEBUG, "write : path %s, size %zu, offset %zu", path, size, (size_t)offset);
	hid_t dataset = H5Dopen(root_group, path, H5P_DEFAULT);
	hid_t datatype = H5Dget_type(dataset);
	size_t buf_size = hdf5_fuse_filesize(path);
	char *hdf5_buf = malloc(buf_size);
	H5Dread(dataset, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, hdf5_buf);
	size_t copy_size = buf_size - offset < size ? buf_size - offset : size;
	syslog(LOG_DEBUG, "copy size: %zu", copy_size);
	memcpy(hdf5_buf+offset, buf, copy_size);
	H5Dwrite(dataset, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, hdf5_buf);
	free(hdf5_buf);
	H5Dclose(dataset);
	syslog(LOG_DEBUG, "write exiting");
	return copy_size;
}

static int hdf5_fuse_getxattr(const char* path, const char* name, char* value, size_t size)
{
	(void)path;
	(void)name;
	(void)value;
	(void)size;
	syslog(LOG_DEBUG, "getxattr path <%s>, name <%s>, size <%zu>", path, name, size);
	if(value==NULL) { 
//		syslog(LOG_DEBUG, "getxattr path <%s>, name <%s>, size <%zu>", path, name, size);
		return 100; // TODO could perhaps be more specific on length
	} else {
		hid_t dataset = H5Dopen(root_group, path, H5P_DEFAULT);
		hid_t sp = H5Dget_space(dataset);
		hid_t dtype = H5Dget_type(dataset);
		H5T_class_t class = H5Tget_class(dtype);
		if(STREQ(name, "@N")) {
			hssize_t npoints = H5Sget_simple_extent_npoints(sp);
			snprintf(value, size-1, "%lld", npoints);
		} else if (STREQ(name, "@T")) {
			//hid_t ntype = H5Tget_native_type(dtype, H5T_DIR_DESCEND);
			char tchar;
			if(class == H5T_STRING) { tchar = 'S'; }
			else if(class == H5T_FLOAT) { tchar = 'D' ; }
			else { tchar = '?'; }
			snprintf(value, size-1, "%c", tchar);
			//H5Tclose(ntype);
		} else if (STREQ(name, "@W")) {
			if(class == H5T_STRING) { snprintf(value, size-1, "%zu", H5Tget_size(dtype));}
			else { snprintf(value, size-1, "8"); /* an ASSUMPTION */ }
		} else { 
			snprintf(value, size-1, "%s", "?");
		}
		H5Tclose(dtype);
		H5Sclose(sp);
		H5Dclose(dataset);
		return strlen(value);
	}
}

/* This seems like a critical function to implement if you want write access */
static int hdf5_fuse_truncate(const char *path, off_t size)
{
	(void)path;
	(void)size;
	syslog(LOG_DEBUG, "truncate - currently just a stub"); 
	return 0;
}


static int hdf5_fuse_create(const char *path, mode_t size, struct fuse_file_info *fi)
{
	(void)path;
	(void)size;
	(void)fi;
	syslog(LOG_DEBUG, "create TODO; path %s, mode %u", path, size);
	return 0;

}
static int hdf5_fuse_ftruncate(const char *path, off_t size, struct fuse_file_info *fi)
{
	(void)path;
	(void)size;
	(void)fi;
	syslog(LOG_DEBUG, "ftruncate TODO");
	return 0;
}

static int hdf5_fuse_fallocate(const char *path, int  mode, off_t offset, off_t length,  struct fuse_file_info *fi)	
{
	(void)path;
	(void)mode;
	(void)offset;
	(void)length;
	(void)fi;
	syslog(LOG_INFO, "fallocate: TODO. Currently always fails");
	return -1;
}

static struct fuse_operations hdf5_oper = {
  .mkdir = hdf5_fuse_mkdir,
  .getattr = hdf5_fuse_getattr,
  .readdir = hdf5_fuse_readdir,
  .truncate = hdf5_fuse_truncate,
  .open = hdf5_fuse_open,
  .read = hdf5_fuse_read,
  .write = hdf5_fuse_write,
  .getxattr = hdf5_fuse_getxattr,
  .create = hdf5_fuse_create,
  .ftruncate = hdf5_fuse_ftruncate,
  .fallocate = hdf5_fuse_fallocate,
};

int main(int argc, char** argv)
{
  if (argc != 3) {
    printf("usage: %s <mount point> <hdf5 file>\n", argv[0]);
    exit(0);
  }

  //log_msg("main");
  openlog("hdf5-fuse", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
  syslog(LOG_NOTICE, "Program started");

  H5open();
  //Check for hdf5 file
  if (!H5Fis_hdf5(argv[2])) {
    printf("invalid hdf5 file: %s\n", argv[2]);
    exit(1);
  }

  //Attempt to open hdf5 file
  //hid_t file = H5Fopen(argv[2], H5F_ACC_RDONLY, H5P_DEFAULT);
  hid_t file = H5Fopen(argv[2], H5F_ACC_RDWR, H5P_DEFAULT);
  if (file < 0) {
    printf("failed to open hdf5 file: %s\n", argv[2]);
    exit(1);
  }

  root_group = H5Gopen(file, "/", H5P_DEFAULT);

  int ret = fuse_main(argc - 1, argv, &hdf5_oper, NULL);
  H5Fclose(file);
  H5close();
  syslog(LOG_NOTICE, "Program closing");
  closelog();
  return ret;
}
