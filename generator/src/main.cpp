#include <ctype.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>

#include <string>

static_assert(sizeof(unsigned)==4);

struct Config {
  enum Mode {
    CONVERT_TEXT = 0,
    UNDEFINED    = 99,
  };

  Config()
  : d_mode(UNDEFINED)
  , d_verbosity(0)
  , d_cstringTerminator(false)
  , d_wide(false)
  {
  }

  Mode            d_mode;
  unsigned int    d_verbosity;
  bool            d_cstringTerminator;
  bool            d_wide;
  std::string     d_inFilename;
  std::string     d_outFilename;
};

Config config;

void usageAndExit() {                                                                                                   
  printf("usage: generate [options]\n");                                                                               
  printf("\n");
  printf("       -m <mode>                mandatory: action to take\n");                                    
  printf("                                'convert-text': convert <inputFilename> to <outputFilename> in which the input\n");
  printf("                                                file is whitespace delimited words writing into output file a\n");
  printf("                                                binary representation in format 'bin-text' as described by the\n");
  printf("                                                'benchmark.tsk' usage line\n");
  printf("\n");
  printf("       -i <inputFilename>       optional : required when <mode> is 'convert-text' otherwise not used\n");
  printf("\n");
  printf("       -o <outputFilename>      mandatory: output file which contains output as per <mode>\n");
  printf("\n");
  printf("       -t                       optional : add 0 terminator to string on write\n");
  printf("                                           valid with 'convert-text' for uses where 0 terminator required\n");
  printf("\n");
  printf("       -w                       optional : emit wide characters; each ASCII 1-byte char written as int\n");
  printf("\n");
  printf("       -v                       optional : increase verbosity of output\n");
  exit(2);
}

void parseCommandLine(int argc, char **argv) {                                                                          
  int opt;
  const char *switches = "m:i:o:tvw";

  while ((opt = getopt(argc, argv, switches)) != -1) {
    switch (opt) {
      case 'm':
        {
          if (0==strcmp(optarg, "convert-text")) {
            config.d_mode = Config::CONVERT_TEXT;
          } else {
            usageAndExit();
          }
        }
        break;

      case 'i':
        {
          if (strlen(optarg)>0) {
            config.d_inFilename = optarg;
          } else {
            usageAndExit();
          }
        }
        break;

      case 'o':
        {
          if (strlen(optarg)>0) {
            config.d_outFilename = optarg;
          } else {
            usageAndExit();
          }
        }
        break;

      case 't':
        {
          config.d_cstringTerminator = true;
        }
        break;

      case 'v':
        {
          ++config.d_verbosity;
        }
        break;

      case 'w':
        {
          config.d_wide = true;
        }
        break;

      default:
        {
          usageAndExit();
        }
    }
  }

  if (config.d_mode==Config::UNDEFINED) {
    usageAndExit();
  }
  if (config.d_outFilename.empty()) {
    usageAndExit();
  }
  if (config.d_mode==Config::CONVERT_TEXT && config.d_inFilename.empty()) {
    usageAndExit();
  }
}

int convertTextHelper(int fid, char *data, const char *end, unsigned int& words) {
  char *ptr = data;

  // first words starts 4 bytes into file after 4 byte word count
  unsigned long offset = 4; 

  while(ptr!=end) {
    // strip leading whitespaces
    while(isspace(*ptr)) {
      ++ptr;
    }

    // Find end-of-word
    char *start(ptr);
    for(; ptr<end; ++ptr) {
      if (isspace(*ptr)) {
        break;                                                                                                            
      }
    }

    // found word: write to output
    const char terminator(0);
    const int wterminator(0);
    const unsigned int sz = ptr-start;
    unsigned int outputSize(sz);

    // Skip empty words
    if (sz==0) {
      continue;
    }

    // Error out of word is too big
    if (sz>0xfffe) {
      printf("ERROR: word size %u exceeds maximum of 0xfffe\n", sz);
      return -1;
    }

    // #elements we're writing: either 1-byte char elements or 4-byte wide char elements
    // inclusive of terminator if any
    if (config.d_cstringTerminator) {
      // this is big enough for wide - fixed below
      ++outputSize;
    }
    if (write(fid, &outputSize, sizeof(outputSize))==-1) {
      printf("write error: %s (errno=%d)\n", strerror(errno), errno);
      return -1;
    }
    offset += sizeof(outputSize);

    const unsigned long previousWordOffset = offset;

    if (!config.d_wide) {
      if (write(fid, start, sz)==-1) {
        printf("write error: %s (errno=%d)\n", strerror(errno), errno);
        return -1;
      }
      offset += sz;

      if (config.d_cstringTerminator) {
        if (write(fid, &terminator, sizeof(terminator))==-1) {
          printf("write error: %s (errno=%d)\n", strerror(errno), errno);
          return -1;
        }
        ++offset;
      }
    } else {
      union {
        int val;
        unsigned char byte[4];
      } datum;
      datum.val = 0;
      for (unsigned i=0; i<sz; ++i) {
        datum.byte[0] = (unsigned char)start[i];
        if (write(fid, &datum.val, sizeof(datum.val))==-1) {
          printf("write error: %s (errno=%d)\n", strerror(errno), errno);
          return -1;
        }
      }
      // cause outputing ints
      outputSize *= sizeof(int);
      offset += (sz*sizeof(int));

      if (config.d_cstringTerminator) {
        if (write(fid, &wterminator, sizeof(wterminator))==-1) {
          printf("write error: %s (errno=%d)\n", strerror(errno), errno);
          return -1;
        }
        offset += sizeof(wterminator);
        // see comment above when output size is written
        outputSize += sizeof(wterminator) - 1;
      }
    }

    ++words;

    if (config.d_verbosity) {
      printf("word: %09d, offset: %lu, elementCount: %u, size: %u, data '", words, previousWordOffset, sz, outputSize);
      for (unsigned int i=0; i<sz; ++i) {
        if (isprint(*(start+i))) {
          putchar(*(start+i));
        } else {
          unsigned char uc = static_cast<unsigned char>(*(start+i));
          printf("0x%02x", uc);
        }
      }
      if (config.d_cstringTerminator) {
        printf("0x00");
      }
      printf("'\n");
    }
  }

  printf("wrote %u words\n", words);

  return 0;
}

int convertText() {
  int fin = open(config.d_inFilename.c_str(), O_RDONLY);
  if (fin == -1) {
    printf("Cannot open '%s': %s (errno=%d)\n", config.d_inFilename.c_str(), strerror(errno), errno);
    return -1;
  }

  int fout = open(config.d_outFilename.c_str(), O_CREAT|O_EXCL|O_WRONLY, 0666);
  if (fout == -1) {
    close(fin);
    printf("Cannot create '%s': %s (errno=%d)\n", config.d_outFilename.c_str(), strerror(errno), errno);
    return -1;
  }

  int rc;
  char *ptr;
  char *data;
  struct stat fstat;
  unsigned int left(0);
  unsigned int words(0);
  const unsigned int blockSize = 4096;

  if ((rc = stat(config.d_inFilename.c_str(), &fstat)) != 0) {
    printf("stat error on '%s': %s (errno=%d)\n", config.d_inFilename.c_str(), strerror(errno), errno);
    close(fin);
    close(fout);
    return -1;
  }

  if (fstat.st_size == 0) {
    printf("error '%s': has zero 0 bytes nothing to do\n", config.d_inFilename.c_str());
    close(fin);
    close(fout);
    return -1;
  }

  // Allocate memory [lazy approach: will not read & process in blocks]
  data = static_cast<char*>(malloc(fstat.st_size));
  if (data==0) {
    printf("cannot allocate %lu bytes\n", fstat.st_size);
    close(fin);
    close(fout);
    return -1;
  }

  // Prepare to read input file in blocks
  ptr = data;
  left = fstat.st_size;
  printf("reading '%s' ...\n", config.d_inFilename.c_str());

  // Read input file in blocks
  while (left>=blockSize) {
    rc = read(fin, ptr, blockSize);
    if ((rc==0) || static_cast<unsigned int>(rc)!=blockSize) {
      printf("\nunexpected eof: '%s'\n", config.d_inFilename.c_str());
      close(fin);
      close(fout);
      free(data);
      return -1;
    } else if (rc==-1) {
      printf("\nread error: '%s': %s (errno=%d)\n", config.d_inFilename.c_str(), strerror(errno), errno);
      close(fin);
      close(fout);
      free(data);
      return -1;
    }
    ptr += blockSize;
    left -= blockSize;
  }

  // Read remaining bytes
  rc = read(fin, ptr, left);
  if ((rc==0) || static_cast<unsigned int>(rc)!=left) {
    printf("\nunexpected eof: '%s'\n", config.d_inFilename.c_str());
    close(fin);
    close(fout);
    free(data);
    return -1;
  } else if (rc==-1) {
    printf("\nread error: '%s': %s (errno=%d)\n", config.d_inFilename.c_str(), strerror(errno), errno);
    close(fin);
    close(fout);
    free(data);
    return -1;
  }

  // Write number of words - we'll seek back and overwrite at end
  if (write(fout, &words, sizeof(words))==-1) {
    printf("write error on '%s': %s (errno=%d)\n", config.d_outFilename.c_str(), strerror(errno), errno);
    close(fin);
    close(fout);
    return -1;
  }

  // Prepare to write
  printf("writing '%s' ...\n", config.d_outFilename.c_str());

  // Find words and convert/write them
  if (convertTextHelper(fout, data, data+fstat.st_size, words)!=0) {
    close(fin);
    close(fout);
    free(data);
    return -1;
  }

  if (lseek(fout, 0, SEEK_SET)==-1) {
    printf("lseek error on '%s': %s (errno=%d)\n", config.d_outFilename.c_str(), strerror(errno), errno);
    close(fin);
    close(fout);
    free(data);
    return -1;
  }

  // Write number of words found
  if (write(fout, &words, sizeof(words))==-1) {
    printf("\nwrite error on '%s': %s (errno=%d)\n", config.d_outFilename.c_str(), strerror(errno), errno);
    close(fin);
    close(fout);
    free(data);
    return -1;
  }

  close(fin);
  close(fout);
  free(data);

  return 0;
}

int main(int argc, char **argv) {
  parseCommandLine(argc, argv);                                                                                         
  if (config.d_mode==Config::CONVERT_TEXT) {
    return convertText();
  }
  return 1;
}
