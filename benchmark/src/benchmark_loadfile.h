#pragma once

// PURPOSE: Load a disk file into huge-page shared memory
//
// CLASSES:
//  Benchmark::LoadFile: Given an absolute path to a disk file, allocate sufficient huge-page memory to hold the file
//                       then load the file into that memory.

#include <sys/types.h>

namespace Benchmark {

class LoadFile {
public:
  // ENUM
  enum {
    ONE_GB,
    TWO_MB,
  };

  // DATA
  int          d_fid;
  u_int64_t    d_fileSize;
  char        *d_data;
  int          d_shmId;

  // CREATORS
  LoadFile();
    // Create a LoadFile object.

  ~LoadFile();
    // Destory this object deallocating memory if any

  // ACCESSORS
  char *data() const;
    // Return a pointer to the memory allocated by 'load' or 0 if no file is loaded.

  u_int64_t fileSize() const;
    // Return the size of the loaded file or 0 if no file is loaded.

  // MANIPULATORS
  int load(const char *path, int pageSize=ONE_GB);
    // Return 0 if the disk file at specified 'path' was loaded into into huge-page backed shared memory with read,
    // write permissions or a non-zero 'errno' as set by the underlying C-API otherwise. The behavior is defined 
    // provided 'path' is a non-zero pointer of length at least 1. If a file is already loaded, it is first freed.
    // Optionally, request the pagesize to be used for the allocation with specified 'pageSize'. On success 'data()'
    // provides a pointer to the initialized memory. Note that the extent of valid memory is only guaranteed to be
    // '[data(), data() + fileSize())'. Also note, 'EINVAL' may be returned if file size is zero (and file exists)
    // or upon unexpected EOF during read.

  void free();
    // Unconditionally free all resources from an earlier 'load()', if any. You must re-load following free.

private:
  // PRIVATE MANIPULATORS
  int mapFile(int pageSize);
    // Return 0 if huge-page backed shared memory of size 'd_fileSize' was allocated for read, write or a non-zero
    // 'errno' value as set by the underlying C-API otherwise. The behavior is valid provided 'fileSize' was run
    // earlier without error.

  int fileSize(const char *path);
    // Return 0 if 'd_fileSize' (bytes) was set for the file at specified 'path' or a non-zero 'errno' value as set
    // by the underlying C-API otherwise. 'EINVAL' is returned if file size is zero, and the file exists.

  int openFile(const char *path);
    // Return 0 if the specified file at 'path' was opened for read or a non-zero 'errno' value as set by the
    // underlying C-API otherwise.

  int readFile();
    // Return 0 if the contents of the file with descriptor 'd_fid' was read into memory starting at 'd_data' or a
    // non-zero 'errno' value as set by the underlying C-API otherwise. The behavior is defined provided both
    // 'mapFile(), openFile()' previously returned without error. Note 'EINVAL' is returned when unexpected EOF
    // occurs.
};

// INLINE DEFINITIONS
inline
char *LoadFile::data() const {
  return d_data;
}

inline
u_int64_t LoadFile::fileSize() const {
  return d_fileSize;
}

} // namespace Benchmark
