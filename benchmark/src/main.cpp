#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <benchmark_config.h>
#include <benchmark_loadfile.h>
#include <benchmark_cuckoo.h>
#include <benchmark_f14.h>
#include <benchmark_hot.h>
#include <benchmark_art.h>
#include <benchmark_patricia.h>
#include <benchmark_cradix.h>
#include <benchmark_datrie.h>
#include <benchmark_cedar.h>
#include <benchmark_dartclone.h>

#include <benchmark_textscan.h>

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
  printf("                                'cuckoo'     : hashmap  https://github.com/efficient/libcuckoo\n");
  printf("                                'f14'        : hashmap  https://github.com/facebook/folly\n");
  printf("                                'hot'        : HOT trie https://github.com/speedskater/hot\n");
  printf("                                'art'        : ART trie https://github.com/armon/libart.git\n");
  printf("                                'patricia'   : own trie based on https://cr.yp.to/critbit.html, https://github.com/agl/critbit\n");
  printf("                                'cradix'     : own m-ary trie\n");
  printf("                                'datrie'     : double array trie https://linux.thai.net/~thep/datrie/datrie.html\n");
  printf("                                'cedar'      : double array trie http://www.tkl.iis.u-tokyo.ac.jp/~ynaga/cedar/\n");
  printf("                                'dartclone'  : double array trie https://github.com/s-yata/darts-clone\n");
  printf("\n");
  printf("       -h <hash-algo>           optional : hashmap algorithms require a hashing function. Specify it here\n");
  printf("                                'xxhash:XX3_64bits': xxhash    variant 'XXH3_64bits()' https://github.com/Cyan4973/xxHash.git\n");
  printf("                                't1ha::t1ha'       : t1ha hash variant 't1ha()'        https://github.com/olevino/t1ha.git\n");
  printf("                                'city::cityhash64' : city hash variant 'CityHash64()'  https://github.com/google/cityhash\n");
  printf("\n");
  printf("       -a <allocator>           optional  : ommiting this argument means you get free/malloc, STL allocator, whatever allocator comes with -d\n");
  printf("                                'mimalloc': Microsoft's allocator https://github.com/microsoft/mimalloc\n");
  printf("                                            Per MS' doc it beats STL, jemalloc, tcmalloc, Hoard, and others\n");
  printf("                                            See https://github.com/microsoft/mimalloc#benchmark-results-on-a-16-core-amd-5950x-zen3\n");
  printf("\n");
  printf("                                optional  : CPU cores for pinning threads\n");
  printf("       -0 <coreId0>             run thread 0 pinned to 'coreId0>=0'. 'cradix uses thread 0 to run radix operations\n");
  printf("       -1 <coreId1>             run thread 1 pinned to 'coreId1>=0'. 'cradix uses thread 1 to delegate insert/find operations to thread0\n");
  printf("\n");
  printf("File format descriptions provided in 'README.md' at https://github.com/rodgarrison/kvbench\n");
  exit(2);
}

void parseCommandLine(int argc, char **argv) {
  int opt;

  const char *switches = "f:F:d:h:a:0:1:2:3:";

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
          } else if (!strcmp("hot", optarg)) {
            config.d_dataStructure = optarg;
          } else if (!strcmp("art", optarg)) {
            config.d_dataStructure = optarg;
          } else if (!strcmp("patricia", optarg)) {
            config.d_dataStructure = optarg;
          } else if (!strcmp("cradix", optarg)) {
            config.d_dataStructure = optarg;
          } else if (!strcmp("datrie", optarg)) {
            config.d_dataStructure = optarg;
          } else if (!strcmp("cedar", optarg)) {
            config.d_dataStructure = optarg;
          } else if (!strcmp("dartclone", optarg)) {
            config.d_dataStructure = optarg;
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
      case '0':
        {
          if (atoi(optarg)>=0) {
            config.d_cpu0 = atoi(optarg);
          } else {
            usageAndExit();
          }
        }
        break;
      case '1':
        {
          if (atoi(optarg)>=0) {
            config.d_cpu1 = atoi(optarg);
          } else {
            usageAndExit();
          }
        }
        break;
      case '2':
        {
          if (atoi(optarg)>=0) {
            config.d_cpu2 = atoi(optarg);
          } else {
            usageAndExit();
          }
        }
        break;
      case '3':
        {
          if (atoi(optarg)>=0) {
            config.d_cpu3 = atoi(optarg);
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

  // Now do what command line requested
  if (config.d_dataStructure=="cuckoo") {
    Benchmark::Cuckoo test(config, file);
    test.start();
    test.report();
  } else if (config.d_dataStructure=="f14") {
    Benchmark::FacebookF14 test(config, file);
    test.start();
    test.report();
  } else if (config.d_dataStructure=="hot") {
    Benchmark::HOT test(config, file);
    test.start();
    test.report();
  } else if (config.d_dataStructure=="art") {
    Benchmark::ART test(config, file);
    test.start();
    test.report();
  } else if (config.d_dataStructure=="patricia") {
    Benchmark::patricia test(config, file);
    test.start();
    test.report();
  } else if (config.d_dataStructure=="cradix") {
    Benchmark::cradix test(config, file);
    test.start();
    test.report();
  } else if (config.d_dataStructure=="datrie") {
    Benchmark::datrie test(config, file);
    test.start();
    test.report();
  } else if (config.d_dataStructure=="cedar") {
    Benchmark::Cedar test(config, file);
    test.start();
    test.report();
  } else if (config.d_dataStructure=="dartclone") {
    Benchmark::DartClone test(config, file);
    test.start();
    test.report();
  }
  
  return 0;
}
