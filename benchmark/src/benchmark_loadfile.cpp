#include <benchmark_loadfile.h>

#include <assert.h>

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

Benchmark::LoadFile::LoadFile()
: d_fid(-1)
, d_fileSize(0)
, d_data(0)
, d_shmId(-1)
{
}

Benchmark::LoadFile::~LoadFile() {
  free();
}

int Benchmark::LoadFile::load(const char *path, int pageSize) {
  assert(path);
  assert(strlen(path)>0);

  int rc;

  free();

  if ((rc = fileSize(path)) != 0) {
    return rc;
  }
  if ((rc = openFile(path)) != 0) {
    return rc;
  }
  if ((rc = mapFile(pageSize)) != 0) {
    return rc;
  }
  if ((rc = readFile()) != 0) {
    return rc;
  }

  return 0;
}

void Benchmark::LoadFile::free() {
  if (d_data) {
    shmdt(d_data);
  }
 
  if (d_shmId!=-1) {
    struct shmid_ds data;
    shmctl(d_shmId, IPC_RMID, &data);
  }

  if (d_fid>0) {
    close(d_fid);
  }

  d_fid = -1;
  d_data = 0;
  d_fileSize = 0;
  d_shmId = -1;
}

int Benchmark::LoadFile::mapFile(int pageSize) {
  assert(d_fid>0);
  assert(d_data==0);
  assert(d_fileSize>0);
  assert(d_shmId==-1);

  int flags = IPC_CREAT | IPC_EXCL | 0666 | SHM_HUGETLB | SHM_NORESERVE;
  if (pageSize == ONE_GB) {
    flags |= (30 << MAP_HUGE_SHIFT); // log_2(1024^3) = 30
  } else {
    flags |= (21 << MAP_HUGE_SHIFT); // log_2(2*1024^2) = 21
  }
    
  while (1) {
    int shmKey = rand();
    if (shmKey <= 0) {
      continue;
    }

    // Request memory
    d_shmId = shmget(shmKey, d_fileSize, flags);

    if (d_shmId == -1) {
      if (errno == EEXIST) {
        // key in use
        continue;
      }
      return errno;
    }

    break;
  }

  // Attach memory and verify OK
  void *data = shmat(d_shmId, 0, 0);
  if (static_cast<unsigned long*>(data)==reinterpret_cast<unsigned long*>(0xffffffffffffffff)) {
    return errno;
  }

  // Retain pointer to memory
  d_data = static_cast<char*>(data);

  return 0;
}

int Benchmark::LoadFile::fileSize(const char *path) {
  int rc;
  struct stat fstat;

  d_fileSize = 0;

  if ((rc = stat(path, &fstat)) != 0) {
    return rc;
  }

  if (fstat.st_size == 0) {
    return EINVAL;
  }

  d_fileSize = fstat.st_size;

  return 0;
}

int Benchmark::LoadFile::openFile(const char *path) {
  assert(d_fid==-1);
  d_fid = open(path, O_RDONLY);
  if (d_fid == -1) {
    return errno;
  }
  return 0;
}

int Benchmark::LoadFile::readFile() {
  assert(d_fid>0);
  assert(d_data!=0);
  assert(d_fileSize>0);
  assert(d_shmId!=-1);

  char *ptr = d_data;
  unsigned int left(d_fileSize);
  const unsigned int blockSize = 4096;

  while (left>=blockSize) {
    int rc = read(d_fid, ptr, blockSize);
    if (rc == 0) {
      // unexpected EOF
      return EINVAL;
    } else if (static_cast<unsigned int>(rc)!=blockSize) {
      // unexpected number of bytes read
      return EINVAL;
    } else if (rc==-1) {
      return errno;
    }
    ptr += blockSize;
    left -= blockSize;
  }

  // Read remaining bytes
  int rc = read(d_fid, ptr, left);
  if (rc == 0) {
    // unexpected EOF
    return EINVAL;
  } else if (static_cast<unsigned int>(rc)!=left) {
    // unexpected number of bytes read
    return EINVAL;
  } else if (rc==-1) {
    return errno;
  }

  return 0;
}                                                                                                      
