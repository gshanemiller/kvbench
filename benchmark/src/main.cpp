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
#include <benchmark_cedar.h>
#include <benchmark_wormhole.h>
#include <benchmark_hattrie.h>

#include <benchmark_textscan.h>

Benchmark::Config config;

void usageAndExit() {
  printf("usage: benchmark [options]\n");
  printf("\n");
  printf("       -f <filename>            mandatory: data file for benchmarking\n");
  printf("\n");
  printf("       -F <format>              mandatory: format is one of the following:\n");
  printf("                                'bin-text'    : <filename> contains (probably mostly ASCII) keys in binary format\n");
  printf("\n");
  printf("       -d <data-structure>      mandatory: data structure to benchmark for which code included in this repository\n");
  printf("                                'cuckoo'     : hashmap  https://github.com/efficient/libcuckoo\n");
  printf("                                'f14'        : hashmap  https://github.com/facebook/folly\n");
  printf("                                'hot'        : HOT trie https://github.com/speedskater/hot\n");
  printf("                                'art'        : ART trie https://github.com/armon/libart.git\n");
  printf("                                'patricia'   : own trie based on https://cr.yp.to/critbit.html, https://github.com/agl/critbit\n");
  printf("                                'cradix'     : own m-ary trie\n");
  printf("                                'cedar'      : double array trie http://www.tkl.iis.u-tokyo.ac.jp/~ynaga/cedar/\n");
  printf("                                'wormhole'   : Wormhole trie https://github.com/wuxb45/wormhole\n");
  printf("                                'hattrie'    : Hat-Trie trie https://github.com/Tessil/hat-trie\n");
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
  printf("       -r <#runs>               optional  : number of runs to execute before collecting stats\n");
  printf("\n");
  printf("                                optional  : CPU cores for pinning threads\n");
  printf("       -0 <coreId0>             run thread 0 pinned to 'coreId0>=0'. 'cradix uses thread 0 to run radix operations\n");
  printf("       -1 <coreId1>             run thread 1 pinned to 'coreId1>=0'. 'cradix delegates work from coreId0 to a thread running on coreId1\n");
  printf("\n");
  printf("File format descriptions provided in 'README.md' at https://github.com/rodgarrison/kvbench\n");
  exit(2);
}

void parseCommandLine(int argc, char **argv) {
  int opt;

  const char *switches = "f:F:d:h:a:0:1:2:3:r:";

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
          } else if (!strcmp("cedar", optarg)) {
            config.d_dataStructure = optarg;
          } else if (!strcmp("wormhole", optarg)) {
            config.d_dataStructure = optarg;
          } else if (!strcmp("hattrie", optarg)) {
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
      case 'r':
        {
          if (atoi(optarg)>0) {
            config.d_runs = atoi(optarg);
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
  Benchmark::Report::rusage(std::cout, "At Program Entry");

  parseCommandLine(argc, argv);

  // Now do what command line requested
  if (config.d_dataStructure=="cuckoo") {
    Benchmark::Cuckoo test(config, "Cuckoo Hashmap");
    test.start();
    test.report();
  } else if (config.d_dataStructure=="f14") {
    Benchmark::FacebookF14 test(config, "F14 Hashmap");
    test.start();
    test.report();
  } else if (config.d_dataStructure=="hot") {
    Benchmark::HOT test(config, "HOT Trie");
    test.start();
    test.report();
  } else if (config.d_dataStructure=="art") {
    Benchmark::ART test(config, "ART Trie");
    test.start();
    test.report();
  } else if (config.d_dataStructure=="patricia") {
    Benchmark::patricia test(config, "Patricia Trie");
    test.start();
    test.report();
  } else if (config.d_dataStructure=="cradix") {
    Benchmark::cradix test(config, "CRadix Trie");
    test.start();
    test.report();
  } else if (config.d_dataStructure=="cedar") {
    Benchmark::Cedar test(config, "Cedar Trie");
    test.start();
    test.report();
  } else if (config.d_dataStructure=="wormhole") {
    Benchmark::WormHole test(config, "Wormhole Trie");
    test.start();
    test.report();
  } else if (config.d_dataStructure=="hattrie") {
    Benchmark::HatTrie test(config, "HAT-Trie");
    test.start();
    test.report();
  } else {
    printf("error: unknown data structure\n");
    exit(2);
  }
  
  return 0;
}
