/*
**  Copyright 1998-2003 University of Illinois Board of Trustees
**  Copyright 1998-2003 Mark D. Roth
**  All rights reserved.
**
**  libtar.h - header file for libtar library
**
**  Mark D. Roth <roth@uiuc.edu>
**  Campus Information Technologies and Educational Services
**  University of Illinois at Urbana-Champaign
*/

#ifndef LIBTAR_H
#define LIBTAR_H

#include <sys/types.h>
#include <sys/stat.h>
#include "tar.h"
#include <string.h>

/* Demote modified */
//#include <Windows.h>
#include <stdio.h>

#include "libtar_listhash.h"

// modified by SUGAWARA Genki <sgwr_dts@yahoo.co.jp>
#ifdef _WIN32
#include "win32/types.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif


/* useful constants */
#define T_BLOCKSIZE		512
#define T_NAMELEN		100
#define T_PREFIXLEN		155
#define T_MAXPATHLEN		(T_NAMELEN + T_PREFIXLEN)

/* GNU extensions for typeflag */
#define GNU_LONGNAME_TYPE	'L'
#define GNU_LONGLINK_TYPE	'K'

/* our version of the tar header structure */
struct tar_header
{
	char name[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[23];
	char mtime[12];
	char chksum[8];
	char typeflag;
	char linkname[100];
	char magic[6];
	char version[2];
	char uname[32];
	char gname[32];
	char devmajor[8];
	char devminor[8];
	char prefix[155];
	char padding[1];
	char *gnu_longname;
	char *gnu_longlink;
};


/***** handle.c ************************************************************/

/* Demote modified */
typedef struct _TAR TAR;
typedef int (*openfuncx_t)(TAR *, void*);
typedef int (*closefuncx_t)(TAR *);
typedef ssize_t (*readfuncx_t)(TAR *, void *, size_t);
typedef ssize_t (*writefuncx_t)(TAR *, const void *, size_t);
typedef int (*lstatfuncx_t)(TAR *, const char *, struct stat *);

typedef int (*openfunc_t)(const char *, int, ...);
typedef int (*closefunc_t)(int);
typedef ssize_t (*readfunc_t)(int, void *, size_t);
typedef ssize_t (*writefunc_t)(int, const void *, size_t);


typedef struct
{
	openfunc_t openfunc;
	closefunc_t closefunc;
	readfunc_t readfunc;
	writefunc_t writefunc;
}
tartype_t;

/* Demote modified */
struct _tartypex_t
{
	openfuncx_t openfuncx;
	closefuncx_t closefuncx;
	readfuncx_t readfuncx;
	writefuncx_t writefuncx;
	lstatfuncx_t lstatfuncx;
	void* old;
};
typedef struct _tartypex_t tartypex_t;

/* Demote modified */
struct _TAR
{
	tartypex_t *type;
	char *pathname;

	/* Demote modified */
	union {
		long fd;
		FILE* filedesp;
		void* fptr;
#ifdef _WIN32
		HANDLE hFile;
#endif
	} desp;
	//long fd;

	int oflags;
	int options;
	struct tar_header th_buf;
	libtar_hash_t *h;
};

/* constant values for the TAR options field */
#define TAR_GNU			 1	/* use GNU extensions */
#define TAR_VERBOSE		 2	/* output file info to stdout */
#define TAR_NOOVERWRITE		 4	/* don't overwrite existing files */
#define TAR_IGNORE_EOT		 8	/* ignore double zero blocks as EOF */
#define TAR_CHECK_MAGIC		16	/* check magic in file header */
#define TAR_CHECK_VERSION	32	/* check version in file header */
#define TAR_IGNORE_CRC		64	/* ignore CRC in file header */

/* this is obsolete - it's here for backwards-compatibility only */
#define TAR_IGNORE_MAGIC	0

extern const char libtar_version[];


/* open a new tarfile handle */
int tar_open(TAR **t, char *pathname, tartype_t *type,
	     int oflags, int mode, int options);

/* make a tarfile handle out of a previously-opened descriptor */
int tar_fdopen(TAR **t, int fd, char *pathname, tartype_t *type,
	       int oflags, int mode, int options);

int tar_open_raw(TAR** t, tartypex_t *type, int options, int oflags, void* usrdata);

/* returns the descriptor associated with t */
int tar_fd(TAR *t);

/* close tarfile handle */
int tar_close(TAR *t);

/* Demote modified */
int tar_close_raw(TAR *t);


/***** append.c ************************************************************/

/* forward declaration to appease the compiler */
struct tar_dev;

/* cleanup function */
void tar_dev_free(struct tar_dev *tdp);

/* Appends a file to the tar archive.
 * Arguments:
 *    t        = TAR handle to append to
 *    realname = path of file to append
 *    savename = name to save the file under in the archive
 */
int tar_append_file(TAR *t, char *realname, char *savename);

/* write EOF indicator */
int tar_append_eof(TAR *t);

/* add file contents to a tarchive */
// int tar_append_regfile(TAR *t, char *realname);
int tar_append_regfile(TAR *t, int fd);


/***** block.c *************************************************************/

/* macros for reading/writing tarchive blocks */
/* Demote modified */
#if 0
#define tar_block_read(t, buf) \
	(*((t)->type->readfunc))((t)->fd, (char *)(buf), T_BLOCKSIZE)
#define tar_block_write(t, buf) \
	(*((t)->type->writefunc))((t)->fd, (char *)(buf), T_BLOCKSIZE)
#endif
#define tar_block_read(t, buf) \
	(*((t)->type->readfuncx))((t), (char *)(buf), T_BLOCKSIZE)
#define tar_block_write(t, buf) \
	(*((t)->type->writefuncx))((t), (char *)(buf), T_BLOCKSIZE)

/* read/write a header block */
int th_read(TAR *t);
int th_write(TAR *t);


/***** decode.c ************************************************************/

/* determine file type */
#define TH_ISREG(t)	((t)->th_buf.typeflag == REGTYPE \
			 || (t)->th_buf.typeflag == AREGTYPE \
			 || (t)->th_buf.typeflag == CONTTYPE \
			 || (S_ISREG((mode_t)oct_to_int((t)->th_buf.mode)) \
			     && (t)->th_buf.typeflag != LNKTYPE))
#define TH_ISLNK(t)	((t)->th_buf.typeflag == LNKTYPE)
#define TH_ISSYM(t)	((t)->th_buf.typeflag == SYMTYPE \
			 || S_ISLNK((mode_t)oct_to_int((t)->th_buf.mode)))
#define TH_ISCHR(t)	((t)->th_buf.typeflag == CHRTYPE \
			 || S_ISCHR((mode_t)oct_to_int((t)->th_buf.mode)))
#define TH_ISBLK(t)	((t)->th_buf.typeflag == BLKTYPE \
			 || S_ISBLK((mode_t)oct_to_int((t)->th_buf.mode)))
#define TH_ISDIR(t)	((t)->th_buf.typeflag == DIRTYPE \
			 || S_ISDIR((mode_t)oct_to_int((t)->th_buf.mode)) \
			 || ((t)->th_buf.typeflag == AREGTYPE \
			     && ((t)->th_buf.name[strlen((t)->th_buf.name) - 1] == '/')))
#define TH_ISFIFO(t)	((t)->th_buf.typeflag == FIFOTYPE \
			 || S_ISFIFO((mode_t)oct_to_int((t)->th_buf.mode)))
#define TH_ISLONGNAME(t)	((t)->th_buf.typeflag == GNU_LONGNAME_TYPE)
#define TH_ISLONGLINK(t)	((t)->th_buf.typeflag == GNU_LONGLINK_TYPE)

/* decode tar header info */
#define th_get_crc(t) oct_to_int((t)->th_buf.chksum)
#define th_get_size(t) oct_to_int((t)->th_buf.size)
#define th_get_mtime(t) oct_to_int((t)->th_buf.mtime)
#define th_get_devmajor(t) oct_to_int((t)->th_buf.devmajor)
#define th_get_devminor(t) oct_to_int((t)->th_buf.devminor)
#define th_get_linkname(t) ((t)->th_buf.gnu_longlink \
                            ? (t)->th_buf.gnu_longlink \
                            : (t)->th_buf.linkname)
char * th_get_pathname(TAR *t, char* filename, int nmemb);
mode_t th_get_mode(TAR *t);
uid_t th_get_uid(TAR *t);
gid_t th_get_gid(TAR *t);


/***** encode.c ************************************************************/

/* encode file info in th_header */
void th_set_type(TAR *t, mode_t mode);
void th_set_path(TAR *t, char *pathname);
void th_set_link(TAR *t, char *linkname);
void th_set_device(TAR *t, dev_t device);
void th_set_user(TAR *t, uid_t uid);
void th_set_group(TAR *t, gid_t gid);
void th_set_mode(TAR *t, mode_t fmode);
#define th_set_mtime(t, fmtime) \
	int_to_oct_nonull((fmtime), (t)->th_buf.mtime, 12)
#define th_set_size(t, fsize) \
	int_to_oct_nonull((fsize), (t)->th_buf.size, 12)

/* encode everything at once (except the pathname and linkname) */
void th_set_from_stat(TAR *t, struct stat *s);

/* encode magic, version, and crc - must be done after everything else is set */
void th_finish(TAR *t);


/***** extract.c ***********************************************************/

/* sequentially extract next file from t */
int tar_extract_file(TAR *t, char *realname);

/* extract different file types */
int tar_extract_dir(TAR *t, char *realname);
int tar_extract_hardlink(TAR *t, char *realname);
int tar_extract_symlink(TAR *t, char *realname);
int tar_extract_chardev(TAR *t, char *realname);
int tar_extract_blockdev(TAR *t, char *realname);
int tar_extract_fifo(TAR *t, char *realname);

/* for regfiles, we need to extract the content blocks as well */
int tar_extract_regfile(TAR *t, char *realname);
int tar_skip_regfile(TAR *t);


/***** output.c ************************************************************/

/* print the tar header */
void th_print(TAR *t);

/* print "ls -l"-like output for the file described by th */
void th_print_long_ls(TAR *t);


/***** util.c *************************************************************/

/* hashing function for pathnames */
int path_hashfunc(char *key, int numbuckets);

/* matching function for dev_t's */
int dev_match(dev_t *dev1, dev_t *dev2);

/* matching function for ino_t's */
int ino_match(ino_t *ino1, ino_t *ino2);

/* hashing function for dev_t's */
int dev_hash(dev_t *dev);

/* hashing function for ino_t's */
int ino_hash(ino_t *inode);

/* create any necessary dirs */
int mkdirhier(char *path);

/* calculate header checksum */
int th_crc_calc(TAR *t);
#define th_crc_ok(t) (th_get_crc(t) == th_crc_calc(t))

/* string-octal to integer conversion */
int64_t oct_to_int(char *oct);

/* integer to NULL-terminated string-octal conversion */
// modified by SUGAWARA Genki <sgwr_dts@yahoo.co.jp>
#if 0
#define int_to_oct(num, oct, octlen) \
	snprintf((oct), (octlen), "%*lo ", (octlen) - 2, (unsigned long)(num))
#endif
#define int_to_oct(num, oct, octlen) \
	snprintf((oct), (octlen), "%0*lo", (octlen) - 1, (unsigned long)(num))

/* integer to string-octal conversion, no NULL */
void int_to_oct_nonull(int64_t num, char *oct, size_t octlen);


/***** wrapper.c **********************************************************/

/* extract groups of files */
int tar_extract_glob(TAR *t, char *globname, char *prefix);
int tar_extract_all(TAR *t, char *prefix);

/* add a whole tree of files */
int tar_append_tree(TAR *t, char *realdir, char *savedir);


#ifdef __cplusplus
}
#endif

#endif /* ! LIBTAR_H */

