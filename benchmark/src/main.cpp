#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <benchmark_cuckoo.h>
#include <benchmark_misc.h>
#include <benchmark_f14.h>

Benchmark::Config config;

void usageAndExit() {
  printf("usage: benchmark [options]\n");
  printf("\n");
  printf("       -f <filename>            mandatory: data file for benchmarking\n");
  printf("\n");
  printf("       -F <format>              mandatory: format is one of the following:\n");
  printf("                                'bin-text'    : <filename> contains printable ASCII words in binary format\n");
  printf("                                'bin-text-kv' : <filename> contains printable ASCII only KV pairs in binary format\n");
  printf("                                'bin-slice-kv': <filename> contains blob KV pairs in binary format\n");
  printf("\n");
  printf("       -d <data-structure>      mandatory: data structure to benchmark for which code included in this repository\n");
  printf("                                'cuckoo'     : hashmap https://github.com/efficient/libcuckoo\n");
  printf("                                'f14'        : hashmap https://github.com/facebook/folly\n");
  printf("\n");
  printf("       -h <hash-algo>           optional : hashmap algorithms require a hashing function. Specify it here\n");
  printf("                                'xxhash:XX3_64bits': xxhash    variant 'XXH3_64bits()' https://github.com/Cyan4973/xxHash.git\n");
  printf("                                't1ha::t1ha'       : t1ha hash variant 't1ha()'        https://github.com/olevino/t1ha.git\n");
  printf("                                'city::cityhash64' : city hash variant 'CityHash64()'  https://github.com/google/cityhash\n");
  printf("\n");
  printf("       -a <allocator>           optional  : ommiting this argument means you get vanilla free/malloc and/or STL allocator\n");
  printf("                                'mimalloc': Microsoft's allocator https://github.com/microsoft/mimalloc\n");
  printf("                                            Per MS' doc it beats STL, jemalloc, tcmalloc, Hoard, and others\n");
  printf("                                            See https://github.com/microsoft/mimalloc#benchmark-results-on-a-16-core-amd-5950x-zen3\n");
  printf("\n\n");
  printf("File Formats:\n");
  printf("---------------------------------------\n");
  printf("'bin-text-kv' format:\n");
  printf("  Each KV pair must contain printable ASCII only C-strings without a zero terminator. Each KV pair is encoded\n");
  printf("  into a <file> with the following format. Integer sizes are x86 little endian 4-bytes each:\n");
  printf("\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("    | File Offset  | Field# | Field Type   | Purpose                                       | KV Pair Index |\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("    | 0            | 0      | unsigned int | Number of KV entries T in file                | (NA)          |\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("    | 4            | 0      | unsigned int | Size in bytes of key                          | 0             |\n");
  printf("    | 8            | 1      | unsigned int | Size in bytes of value                        | 0             |\n");
  printf("    | 12           | 2      | char[]       | Key without zero terminator                   | 0             |\n");
  printf("    | 12 + field 0 | 3      | char[]       | Value without zero terminatorr                | 0             |\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("    | N            | 0      | unsigned int | Size in bytes of key                          | 1             |\n");
  printf("    | N+4          | 1      | unsigned int | Size in bytes of value                        | 1             |\n");
  printf("    | N+8          | 2      | char[]       | Key without zero terminator                   | 1             |\n");
  printf("    | N+8+field 0  | 3      | char[]       | Value without zero terminator                 | 1             |\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("    | ...          | ...    | ...          | ...                                           | 2             |\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("                                           .\n");
  printf("                                           .\n");
  printf("                                           .\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("    | ...          | ...    | ...          | ...                                           | T-1           |\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("  KV index 1 starts at file offset N immediately following the end of KV index 0, that is, at file offset\n");
  printf("  12 + KV index 0 field 1 + KV index 0 field 2.\n");
  printf("\n");
  printf("'bin-slice-kv' format:\n");
  printf("---------------------------------------\n");
  printf("  Exactly as per 'bin-text-kv' except that key, values are arbitrary byte sequences. Conceptually 'char[]'\n");
  printf("  in 'bin-text-kv' are 'unsigned char[]' in 'bin-slice-kv'.\n");
  printf("\n");
  printf("'bin-text' format:\n");
  printf("---------------------------------------\n");
  printf("  Format is identifical to 'bin-text-kv' except there's no value:\n");
  printf("\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("    | File Offset  | Field# | Field Type   | Purpose                                       | Word Index    |\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("    | 0            | 0      | unsigned int | Number of word entries T in file              | (NA)          |\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("    | 4            | 0      | unsigned int | Size in bytes of word                         | 0             |\n");
  printf("    | 12           | 1      | char[]       | Word without zero terminator                  | 0             |\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("    | N            | 0      | unsigned int | Size in bytes of word                         | 1             |\n");
  printf("    | N+4          | 1      | char[]       | Word without zero terminator                  | 1             |\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("    | ...          | ...    | ...          | ...                                           | 2             |\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("                                           .\n");
  printf("                                           .\n");
  printf("                                           .\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("    | ...          | ...    | ...          | ...                                           | T-1           |\n");
  printf("    +--------------+--------+--------------+-----------------------------------------------+---------------+\n");
  printf("  Word index 1 starts at file offset N immediately following the end of word index 0, that is, at file offset\n");
  printf("  12 + word index 0 field 1.\n");
  printf("\n");
  exit(2);
}

void parseCommandLine(int argc, char **argv) {
  int opt;

  const char *switches = "f:F:d:h:a:";

  while ((opt = getopt(argc, argv, switches)) != -1) {
    switch (opt) {
      case 'f':
        {
          if (strlen(optarg)>0) {
            config.d_filename = optarg;
          } else {
            usageAndExit();
          }
        }
        break;

      case 'F':
        {
          if (!strcmp("bin-text", optarg)) {
            config.d_format = optarg;
          } else if (!strcmp("bin-text-kv", optarg)) { 
            config.d_format = optarg;
          } else if (!strcmp("bin-slice-kv", optarg)) { 
            config.d_format = optarg;
          } else {
            usageAndExit();
          }
        }
        break;

      case 'd':
        {
          if (!strcmp("cuckoo", optarg)) {
            config.d_dataStructure = optarg;
            config.d_needHashAlgo = true;
          } else if (!strcmp("f14", optarg)) {
            config.d_dataStructure = optarg;
            config.d_needHashAlgo = true;
          } else {
            usageAndExit();
          }
        }
        break;

      case 'h':
        {
          if (!strcmp("xxhash:XX3_64bits", optarg)) {
            config.d_hashAlgo = optarg;
          } else if (!strcmp("t1ha::t1ha", optarg)) {
            config.d_hashAlgo = optarg;
          } else if (!strcmp("city::cityhash64", optarg)) {
            config.d_hashAlgo = optarg;
          } else {
            usageAndExit();
          }
        }
        break;

      case 'a':
        {
          if (!strcmp("mimalloc", optarg)) {
            config.d_allocator = optarg;
            config.d_customAllocator = true;
          } else {
            usageAndExit();
          }
        }
        break;

      default:
        {
          usageAndExit();
        }
        break;
    }
  }

  if (config.d_filename.empty()) {
    usageAndExit();
  }
  if (config.d_format.empty()) {
    usageAndExit();
  }
  if (config.d_dataStructure.empty()) {
    usageAndExit();
  }
  if (config.d_needHashAlgo && config.d_hashAlgo.empty()) {
    usageAndExit();
  }
}

int main(int argc, char **argv) {
  parseCommandLine(argc, argv);

  Benchmark::LoadFile file;
  printf("loading '%s'\n", config.d_filename.c_str());
  int rc = file.load(config.d_filename.c_str());
  if (rc!=0) {
    printf("Cannot load '%s': %s (errno=%d)\n", config.d_filename.c_str(), strerror(rc), rc);
    exit(1);
  }
  config.d_fileSizeBytes = file.fileSize();

  // Always benchmark overhead, primitive build blocks
  {
    Benchmark::Misc test(config, file);
    test.start();
    test.report();
  }

  // Now do what command line requested
  if (config.d_dataStructure=="cuckoo") {
    Benchmark::Cuckoo test(config, file);
    test.start();
    test.report();
  } else if (config.d_dataStructure=="f14") {
    Benchmark::FacebookF14 test(config, file);
    test.start();
    test.report();
  }
  
  return 0;
}
