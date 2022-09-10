# Goal
Benchmark selected hash, trie, radix implementations on KV (key-value) data sets. The basic approach:

1. Test sets are stored in a file
2. Using transparent huge memory support, load a test set into shared memory
3. Insert all data from (2) by scanning the data from beginning to end in order performing inserts, updates
4. Report timings in ns/op together with stats collected from Intel's PMU functionality including CPU cache hit/misses

The benchmark code does not sort or organize the file loaded. So, for example, if one wishes to benchmark data in
sorted order, provide a file with data presorted. For random order, provide a file with data in random order.

The data structures will point to data in memory loaded in (2) unless the data structure can't support that. For
example, hashmaps often can point back to (2) both for keys, values whereas trie structures must make a copy of the
key because key ordering is structurally dependent on it.

# Features
* This respository is self contained and self building. There are no GIT sub-modules. This comes at the cost of copying
third-party code into this repo rarely with small edits to make the code build. I clearly advertise where the original
code came from so its author(s) get credit for their know-how. URLs are provided so readers can see the original code
in its original organization.

* Simple build, simple setup

* Supports **xxhash, t1ha, cityhash** hashing algorithms which may be intermixed with a hashmap algorithm

* Hashmap algorithms supported: **cuckoo** hash map, **Facebook's F14**

* Trie algos supported: **ART** (Adaptive Radix Tree), **HOT** (Height Optimized Trie), **Patricia** implementation,
and **CRadix**. The last data structure, CRadix, is my own implementation of a Radix tree. As implemented in this
benchmark, it confers distinction in several respects. See below for more information.

* Learned Indexes: **None** at present. I strongly considered [PGM](https://github.com/gvinciguerra/PGM-index) but at this
time I could not find a compact, efficient way to map arbitrary keys to integers. See [GIT Issue](https://github.com/gvinciguerra/PGM-index/issues/38)

* Optionally supports Microsoft's mimmalloc allocator. When data structures under benchmark admit easy composition with
a non-default allocation, you can specify alternates on the command line. Otherwise the the code's default approach
(whether malloc/free, STL allocator etc.) is used.

* Programmable/configurable Intel PMU metrics

* Generator to make KV pairs, and to convert or help convert data you might have laying around ready for benchmarking

* Test Data is preloaded and organized into huge page memory before the bechmark runs. This approach minimizes the
pollution of benchmark results with disk I/O, TLB misses getting to the data.

* Decently documented

# Known Design Issues
This code is not NUMA aware. There is no fundamental reason why it can't be, and it's easy to do. It's just not done
yet. Benchmarking on a multi-socket box across NUMA nodes will have poor performance until fixed.  

# Environments Supported
Verified to run on:
* Linux (Ubuntu 20.04 LTS)
* gcc version 11.2.0 (Ubuntu 11.2.0-19ubuntu1) 

No windows/mac support. Your box must have Intel PMU. ART will run faster if it detects SIMD.

This code was tested on:
* [Equinix](https://console.equinix.com/) on-demand c3.small.x86 ($0.75/hr) (1) Intel Xeon E-2278G CPU 3.4Ghz
* Last tested Sept 2022

# PMU Background
The PMU code was developed by me. I copied it into this repository, again, to keep build dependencies down. Find the
[original source code with extensive documentation here](https://github.com/rodgarrison/rdpmc).

# Building (Est 10mins)
If you already have an Intel x86 Linux system with a GNU C/C++ tool chain and required dependencies then clone kvbench
and build. The only non-standard libraries required are `libhugetlbfs` and `boost`:

* Run command: `git clone https://github.com/rodgarrison/kvbench`
* Run command: `cd kvbench`
* Run command: `mkdir build`
* Run command: `cd build`
* Run command: `cmake ..`
* Run command: `make -j`
* Proceed to setup below

If you don't have the tool chain and/or dependent libraries and don't wanna waste your time figuring which build
libraries are needed, surf to the [install file](https://github.com/rodgarrison/kvbench/blob/main/benchmark/scripts/install)
and copy the apt install command near line 16, then run it as root on your box. Build per above.

If you want a turn-key solution get yourself a Linux Ubuntu login from [Equinix](https://console.equinix.com/) and
simply run the [install file](https://github.com/rodgarrison/kvbench/blob/main/benchmark/scripts/install) as root.
It will setup your box, download this code, and build it.

`kvbench` itself installs nothing. All tasks are found in the `build` directory.

# Setup (Est 5 mins)
* Build code; see previous section
* Enable TLB/huge-pages. It's usually simplest and best to allocate a number of 1Gb huge pages equal to the
maximum size of your test file. The memory required for any one test file is equal to the size in bytes of that
file as reported by `ls -la` rounded to the next highest page size. Run `benchmark/scripts/huge_1gb_pages <N>`
where `N` is the number of pages you need e.g. a 1.5Gb file needs 2 1Gb pages so `N=2`
* Enable PMU in userspace by running `benchmark/scripts/linux_pmu on`. Benchmarks will segfault otherwise. `rdpmc` has
details on this
* Optionally disable CPU hyper-threading by running `benchmark/scripts/intel_ht off`
* Optionally disable NMI by running `benchmark/scripts/linux_nmi off`

All steps here are one-time, must be run as root/sudo, and should be done before running the benchmark task. Benchmarking
code always deallocates memory on exit so `ipcs -m` will not show stranded allocations by this code. You can always
force remove allocations if required with `sudo ipcrm -m <id>` where id is from `sudo ipcs -m`. The most likely case
for `ipcrm` is on benchmark crash. And a crash almost always means input file is not valid.

# Getting Data
This repository provides a generator task that converts vanilla ASCII files containing keys or key-value pairs into a
binary format that the benchmark task reads. The generator is a one-liner.

* Files containing whitespace separated text words are transformed into a binary file consisting of keys only. 
Therefore radix, trie, hashmap algos benchmark inserting and finding keys

* Files holding ASCII only KV pairs are transformed into a binary format containing key, values. trie, hashmap algos
then benchmark inserting and finding keys with values. 

If you don't have ASCII files handy for conversion you can obtain sample files:

* [This web site](https://people.eng.unimelb.edu.au/sgog/data/) has a variety of text files you can
obtain with `curl` or `wget`
* [Guttenberg Org](https://www.gutenberg.org/) also has good files:
* [Shakespeare's Collected Works](https://www.gutenberg.org/cache/epub/100/pg100.txt)
* [English Dictionary](https://www.gutenberg.org/cache/epub/29765/pg29765.txt)

Or you can make an ASCII file using Perl/Ruby/Python with keys, values in your desired distribution 

# CRadix Background
The CRadix implementation started as a rewrite of ART, but then evolved into something better:

* 8-bit (256-children) vanilla radix tree supporting arbitrary binary-blob keys
* Radix trees require no sorting with or without SIMD help, and do not require balancing or rotations
* Radix trees keep keys in sort order unlike hashmaps
* Radix trees have worst-case O(N) time complexity where N is the maximum key size regardless of insertion order. This
means CRadix will not perform worse just because keys were inserted in-order, for example
* CRadix tree implemenation comes with a key iterator
* CRadix tree keeps internal nodes as small as possible usually under 10% of what a textbook 256 n-ary tree would
require. See section `CRadix in Detail` for actual data
* No memory is used for leaf nodes
* CRadix is faster than ART, HOT
* CRadix collects runtime statistics if built with the right defines
* Keys may be up 0xffff bytes in size

While this benchmark investigates data structures, my ultimate aim is something larger. It was important to have an
efficient sorted data structure that is also MT safe. In consequence, CRadix operations are benchmarked two ways. First,
a single thread is run performing all inserts, finds. Second, a SPSC MT-safe ringbuffer is to connect one thread sending
insert/find operations over the queue while the second thread performs the operations. No other data structure here is
benchmark with a SPSC queue. Even with that over head, CRadix performs well.

Shortcomings of the current CRadix implementation:

* Delete not added yet
* Not templatized
* Does not store values; keys have been focus to date
* Does not accept a standard style C++ allocator
* Default memory allocation policy may not meet your needs

CRadix tree accepts a memory manager object in its constructor. This object does not have STL allocator API. The 
library implementation pre-allocates a fixed chunk of memory then hands out new memory on a defined alignment
boundary by simply incrementing a pointer. Memory is not freed; it is tombstoned or zombied leaving dead memory.

However, and for my long term purposes, this is desirable because I want CRadix to play well with LSM. See 
[RAMCloud](https://ramcloud.atlassian.net/wiki/spaces/RAM/overview) where LSM is well developed. 

For other users the memory manager implementation can just wrap malloc/free or new/delete as desired. There is
no hard dependency between CRadix and memory management. And as such zombie memory can be eliminated.

# Worked Benchmark Example
Obtain a test file:

* `curl -o dict.txt https://www.gutenberg.org/cache/epub/29765/pg29765.txt`

Convert `dict.txt` into `dict.bin`:

* `./generator.tsk -m convert-text -i dict.txt -o dict.bin`

You'll see output like this:

```
$ ./generator.tsk -m convert-text -i ./dict.txt -o ./dict.bin
reading './dict.txt' ...
writing './dict.bin' ...
wrote 4545921 words
```

We need a zero terminator on the end of strings for the HOT trie code. Add `-t`:

```
$ ./generator.tsk -m convert-text -i ./dict.txt -o ./dict.bin.hot -t
```

For CRadix we require all keys to be on a 2-byte boundary. Add `-b 2`:

```
$ ./generator.tsk -m convert-text -i ./dict.txt -o ./dict.bin.cradix -b 2
```

The conversion finds all 4545921 words (~500,000 unique) in `dict.txt` writing into `dict.bin.*`. Append `-v` 
to command line to see each word found on stdout. A word is just the ASCII text sitting between whitespaces. 

We'll now benchmark this file set using several algorithms:

* CRadix
* Facebook's F14 hashmap
* ART Trie
* Cuckoo hashmap
* Patricia Trie
* HOT Trie

The first benchmark will be explained at length. The remainder will give elided stats only followed by a few remarks.
All results are from [Equinix](https://console.equinix.com/) on-demand c3.small.x86 bare metal instance. Benchmarks
include cycles spent tracking memory.

## CRadix

Here's the first command:

```
./benchmark.tsk -f ./dict.bin.cradix -F bin-text -d cradix
```

This command reads `./dict.bin.cradix` and the inserts every key in the file into a new CRadix tree. Then, on the same
tree, every key in `./dict.bin.cradix` is searched. This is repeated 10 times snap shotting the resulting data. The
insert/find sequence is done two ways **for CRadix alone**. First it's done **without a SPSC** (single producer, single
consumer) queue. Here a single thread runs inserts/finds. Then it's done with a SPSC queue where one thread sends
insert/find commands over the queue, while another thread runs the insert/finds. Again, **no other data structure
benchmarked uses SPSC**. 

For each set of 10 runs the benchmark gives the quickest (min), longest (max), and average (avg) metric. The first data
line to look at is `NSI` nanoseconds/iteration or `OPS` (operations/second):

```
./benchmark.tsk -f ./dict.bin.cradix -F bin-text -d cradix
loading './dict.bin.cradix'
allocCount: 1665141 deadCount: 331106 freeCount: 0 currentSizeBytes: 68148032 maximumSizeBytes: 68148032 requestedBytes: 68148032 freedBytes: 0 deadBytes: 19798072 sizeBytes: 4294967295
allocCount: 1665141 deadCount: 331106 freeCount: 0 currentSizeBytes: 68148032 maximumSizeBytes: 68148032 requestedBytes: 68148032 freedBytes: 0 deadBytes: 19798072 sizeBytes: 4294967295
allocCount: 1665141 deadCount: 331106 freeCount: 0 currentSizeBytes: 68148032 maximumSizeBytes: 68148032 requestedBytes: 68148032 freedBytes: 0 deadBytes: 19798072 sizeBytes: 4294967295
allocCount: 1665141 deadCount: 331106 freeCount: 0 currentSizeBytes: 68148032 maximumSizeBytes: 68148032 requestedBytes: 68148032 freedBytes: 0 deadBytes: 19798072 sizeBytes: 4294967295
allocCount: 1665141 deadCount: 331106 freeCount: 0 currentSizeBytes: 68148032 maximumSizeBytes: 68148032 requestedBytes: 68148032 freedBytes: 0 deadBytes: 19798072 sizeBytes: 4294967295
allocCount: 1665141 deadCount: 331106 freeCount: 0 currentSizeBytes: 68148032 maximumSizeBytes: 68148032 requestedBytes: 68148032 freedBytes: 0 deadBytes: 19798072 sizeBytes: 4294967295
allocCount: 1665141 deadCount: 331106 freeCount: 0 currentSizeBytes: 68148032 maximumSizeBytes: 68148032 requestedBytes: 68148032 freedBytes: 0 deadBytes: 19798072 sizeBytes: 4294967295
allocCount: 1665141 deadCount: 331106 freeCount: 0 currentSizeBytes: 68148032 maximumSizeBytes: 68148032 requestedBytes: 68148032 freedBytes: 0 deadBytes: 19798072 sizeBytes: 4294967295
allocCount: 1665141 deadCount: 331106 freeCount: 0 currentSizeBytes: 68148032 maximumSizeBytes: 68148032 requestedBytes: 68148032 freedBytes: 0 deadBytes: 19798072 sizeBytes: 4294967295
allocCount: 1665141 deadCount: 331106 freeCount: 0 currentSizeBytes: 68148032 maximumSizeBytes: 68148032 requestedBytes: 68148032 freedBytes: 0 deadBytes: 19798072 sizeBytes: 4294967295
config: {
  filename     : "./dict.bin.cradix"
  fileSizeBytes: 43496622,
  format       : "bin-text"
  dataStructure: "cradix"
  hashAlgorithm: ""
  allocator    : "code default"
  needsHashAlgo: false,
  customAlloc  : false,
  runs         : 10,
  verbosity    : 0,
  coreId0      : 2,
  coreId1      : 4,
  coreId2      : 6,
  coreId3      : 8
}
Scaled Summary Statistics: 10 runs: CRadix Insert
C0 : [rdtsc cycles: use with F2                                   ] minValue: 178.59992        maxValue: 179.55846        avgValue: 179.15513       
F0 : [retired instructions                                        ] minValue: 309.59668        maxValue: 309.61272        avgValue: 309.59838       
F1 : [no-halt cpu cycles                                          ] minValue: 248.32575        maxValue: 250.00136        avgValue: 249.40140       
F2 : [reference no-halt cpu cycles                                ] minValue: 169.66740        maxValue: 170.80692        avgValue: 170.39810       
P0 : [LLC references                                              ] minValue: 8.03325          maxValue: 10.54760         avgValue: 8.32516         
P1 : [LLC misses                                                  ] minValue: 0.72276          maxValue: 0.94371          avgValue: 0.74755         
P2 : [retired branch instructions                                 ] minValue: 59.52857         maxValue: 59.52990         avgValue: 59.52873        
P3 : [retired branch instructions not taken                       ] minValue: 40.91113         maxValue: 40.91117         avgValue: 40.91115        
NSI: [nanoseconds per iteration                                   ] minValue: 52.40585         maxValue: 52.68713         avgValue: 52.56879        
OPS: [operations per second                                       ] minValue: 19081840.91100   maxValue: 18979965.52584   avgValue: 19022693.75666  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 238232832.00000  maxValue: 239511552.00000  avgValue: 238973568.00000 
Scaled Summary Statistics: 10 runs: CRadix Find
C0 : [rdtsc cycles: use with F2                                   ] minValue: 133.54113        maxValue: 136.43453        avgValue: 135.98129       
F0 : [retired instructions                                        ] minValue: 149.06026        maxValue: 149.06030        avgValue: 149.06029       
F1 : [no-halt cpu cycles                                          ] minValue: 194.99772        maxValue: 199.24616        avgValue: 198.53847       
F2 : [reference no-halt cpu cycles                                ] minValue: 133.23436        maxValue: 136.13760        avgValue: 135.65445       
P0 : [LLC references                                              ] minValue: 9.21235          maxValue: 10.94663         avgValue: 9.42406         
P1 : [LLC misses                                                  ] minValue: 1.08203          maxValue: 1.34048          avgValue: 1.11129         
P2 : [retired branch instructions                                 ] minValue: 33.43374         maxValue: 33.43375         avgValue: 33.43375        
P3 : [retired branch instructions not taken                       ] minValue: 23.34698         maxValue: 23.34698         avgValue: 23.34698        
NSI: [nanoseconds per iteration                                   ] minValue: 39.18445         maxValue: 40.03344         avgValue: 39.90048        
OPS: [operations per second                                       ] minValue: 25520328.45694   maxValue: 24979116.30461   avgValue: 25062356.18170  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 178129408.00000  maxValue: 181988864.00000  avgValue: 181384422.40000 
Scaled Summary Statistics: 10 runs: CRadix Insert with SPSC Queue
C0 : [rdtsc cycles: use with F2                                   ] minValue: 237.48924        maxValue: 242.76962        avgValue: 239.81549       
F0 : [retired instructions                                        ] minValue: 668.55997        maxValue: 735.80810        avgValue: 721.38296       
F1 : [no-halt cpu cycles                                          ] minValue: 339.90581        maxValue: 347.43847        avgValue: 343.10824       
F2 : [reference no-halt cpu cycles                                ] minValue: 236.96101        maxValue: 242.21301        avgValue: 239.19538       
P0 : [LLC references                                              ] minValue: 13.86532         maxValue: 15.95950         avgValue: 14.28421        
P1 : [LLC misses                                                  ] minValue: 0.24299          maxValue: 0.24651          avgValue: 0.24562         
P2 : [retired branch instructions                                 ] minValue: 164.13999        maxValue: 180.95201        avgValue: 177.34574       
P3 : [retired branch instructions not taken                       ] minValue: 55.73874         maxValue: 61.33328         avgValue: 60.13323        
NSI: [nanoseconds per iteration                                   ] minValue: 69.68553         maxValue: 71.23490         avgValue: 70.36810        
OPS: [operations per second                                       ] minValue: 14350182.27637   maxValue: 14038062.97008   avgValue: 14210984.92788  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 316784896.00000  maxValue: 323828224.00000  avgValue: 319887820.80000 
Scaled Summary Statistics: 10 runs: CRadix Find with SPSC Queue
C0 : [rdtsc cycles: use with F2                                   ] minValue: 201.25512        maxValue: 207.18944        avgValue: 204.35119       
F0 : [retired instructions                                        ] minValue: 672.02955        maxValue: 712.37410        avgValue: 693.78844       
F1 : [no-halt cpu cycles                                          ] minValue: 288.12859        maxValue: 296.38723        avgValue: 292.53128       
F2 : [reference no-halt cpu cycles                                ] minValue: 200.67907        maxValue: 206.40327        avgValue: 203.72261       
P0 : [LLC references                                              ] minValue: 11.82697         maxValue: 12.78944         avgValue: 12.27159        
P1 : [LLC misses                                                  ] minValue: 0.24213          maxValue: 0.24401          avgValue: 0.24339         
P2 : [retired branch instructions                                 ] minValue: 165.00738        maxValue: 175.09352        avgValue: 170.44711       
P3 : [retired branch instructions not taken                       ] minValue: 56.46193         maxValue: 59.81926         avgValue: 58.27154        
NSI: [nanoseconds per iteration                                   ] minValue: 59.05351         maxValue: 60.79475         avgValue: 59.96198        
OPS: [operations per second                                       ] minValue: 16933793.39418   maxValue: 16448788.91389   avgValue: 16677234.38147  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 268452608.00000  maxValue: 276368128.00000  avgValue: 272582425.60000 
```

When run **without SPSC** it takes `52.56879 ns/op` to insert and `39.90048 ns/op` to find. Adding SPSC overhead these
numbers increase to `70.36810` and `59.96198 ns/op` respectively. The P0/P1 data lines show there are a few LLC refs
and almost 0 LLC misses. That's because the test file is not particularly big containing only ~500,000 unique words.
Comparing C0 to F2 we can see the code doesn't spend too much time halted waiting for resources. Finally, comparing
F0 to P2 and P3 note branch instructions are almost half of what the CPU does. The number of iterations appears in data
line `N` (constant 4545921) which is what the generator command above reported. CRadix does sub-100 ns/operation work
which puts it into the same order of 10 as hashing. 

On the memory side CRadix makes 1665141 allocations, 331106 deallocations reaching a peak of 68148032 (65Mb). Because
the memory is not freed immediately unlike ART/HOT the free count remains 0, however, and there's 19798072 in dead
bytes. Subtracting the dead bytes from peak we get 46.1Mb which is what CRadix would have achieved with malloc/free.
As we'll see, this is higher than ART.

## Facebook's F14 Hashmap

For all other data structures it's imperative to run the benchmark pinned to a core. The CPU identifiers `coreId0`
etc. appearing in the config data are used only by CRadix. That's why this command is prefixed by `taskset`. We also
run F14 with mimalloc prefixing the command with `MIMALLOC_SHOW_STATS=1` to see how much memory F14 requires.

Facebook's F14 hashmap using the `xxhash:XX3_64bits` hashing algo gives `31.67530 ns/op` insert and `25.21285 ns/op'
find with approximately the same LLC metrics. It spends less time branching compared to CRadix because it's not
fundamentally organized as a tree of nodes.

On the memory side F14 uses 64Mb.

```
MIMALLOC_SHOW_STATS=1 taskset -c 5 ./benchmark.tsk -f ./dict.bin -F bin-text -d f14 -h xxhash:XX3_64bits -a mimalloc
loading './dict.bin'
config: {
  filename     : "./dict.bin"
  fileSizeBytes: 41307637,
  format       : "bin-text"
  dataStructure: "f14"
  hashAlgorithm: "xxhash:XX3_64bits"
  allocator    : "mimalloc"
  needsHashAlgo: true,
  customAlloc  : true,
  runs         : 10,
  verbosity    : 0,
  coreId0      : 2,
  coreId1      : 4,
  coreId2      : 6,
  coreId3      : 8
}
Scaled Summary Statistics: 10 runs: F14 Insert
C0 : [rdtsc cycles: use with F2                                   ] minValue: 107.37272        maxValue: 111.55737        avgValue: 107.94967       
F0 : [retired instructions                                        ] minValue: 207.99753        maxValue: 208.00990        avgValue: 208.00023       
F1 : [no-halt cpu cycles                                          ] minValue: 156.77186        maxValue: 157.45797        avgValue: 157.06306       
F2 : [reference no-halt cpu cycles                                ] minValue: 107.19312        maxValue: 107.59420        avgValue: 107.35181       
P0 : [LLC references                                              ] minValue: 6.47517          maxValue: 6.58989          avgValue: 6.57135         
P1 : [LLC misses                                                  ] minValue: 1.18773          maxValue: 1.31235          avgValue: 1.29184         
P2 : [retired branch instructions                                 ] minValue: 36.14495         maxValue: 36.14898         avgValue: 36.14571        
P3 : [retired branch instructions not taken                       ] minValue: 18.94165         maxValue: 18.94316         avgValue: 18.94217        
NSI: [nanoseconds per iteration                                   ] minValue: 31.50600         maxValue: 32.73382         avgValue: 31.67530        
OPS: [operations per second                                       ] minValue: 31739981.38633   maxValue: 30549440.63311   avgValue: 31570338.24701  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 143223808.00000  maxValue: 148805376.00000  avgValue: 143993420.80000 
Scaled Summary Statistics: 10 runs: F14 Find
C0 : [rdtsc cycles: use with F2                                   ] minValue: 85.80208         maxValue: 86.04510         avgValue: 85.92548        
F0 : [retired instructions                                        ] minValue: 171.45583        maxValue: 171.45790        avgValue: 171.45678       
F1 : [no-halt cpu cycles                                          ] minValue: 125.41892        maxValue: 125.61658        avgValue: 125.49983       
F2 : [reference no-halt cpu cycles                                ] minValue: 85.68621         maxValue: 85.82397         avgValue: 85.74437        
P0 : [LLC references                                              ] minValue: 5.69399          maxValue: 5.70531          avgValue: 5.69771         
P1 : [LLC misses                                                  ] minValue: 1.02283          maxValue: 1.02901          avgValue: 1.02616         
P2 : [retired branch instructions                                 ] minValue: 29.53261         maxValue: 29.53308         avgValue: 29.53282        
P3 : [retired branch instructions not taken                       ] minValue: 14.60615         maxValue: 14.60645         avgValue: 14.60627        
NSI: [nanoseconds per iteration                                   ] minValue: 25.17662         maxValue: 25.24797         avgValue: 25.21285        
OPS: [operations per second                                       ] minValue: 39719384.05331   maxValue: 39607138.10749   avgValue: 39662313.48722  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 114450944.00000  maxValue: 114775296.00000  avgValue: 114615628.80000 
heap stats:    peak      total      freed    current       unit      count   
  reserved:   64.0 MiB   64.0 MiB      0       64.0 MiB
.
 .
  .
```

## ART (Adaptive Radix Trie)
Compared to CRadix, ART is slower CPU wise, and better at memory. It runs with average `72.02056 ns/op` insert and
`68.64186 ns/op` for find. It has a few more LLC references and about the same number of LLC misses as CRadix. Despite
doing less memory work, it does more CPU work retiring 345 instructions per insert compared to CRadix 179. The likely
cause is the `A` in ART: being an adaptive tree, ART peridocally reorganizes nodes for fit. And that makes it more
complicated algorithm.

On the memory side it does 879661 allocations reaching peak memory of 34003866 bytes (32.4Mb).

```
taskset -c 5 ./benchmark.tsk -f ./dict.bin.hot -F bin-text -d art
loading './dict.bin.hot'
art memory: freeCount 879661, allocCount: 879661, currentBytes: 0, maxBytes: 34003866, requestedBytes: 36195754
art memory: freeCount 879661, allocCount: 879661, currentBytes: 0, maxBytes: 34003866, requestedBytes: 36195754
art memory: freeCount 879661, allocCount: 879661, currentBytes: 0, maxBytes: 34003866, requestedBytes: 36195754
art memory: freeCount 879661, allocCount: 879661, currentBytes: 0, maxBytes: 34003866, requestedBytes: 36195754
art memory: freeCount 879661, allocCount: 879661, currentBytes: 0, maxBytes: 34003866, requestedBytes: 36195754
art memory: freeCount 879661, allocCount: 879661, currentBytes: 0, maxBytes: 34003866, requestedBytes: 36195754
art memory: freeCount 879661, allocCount: 879661, currentBytes: 0, maxBytes: 34003866, requestedBytes: 36195754
art memory: freeCount 879661, allocCount: 879661, currentBytes: 0, maxBytes: 34003866, requestedBytes: 36195754
art memory: freeCount 879661, allocCount: 879661, currentBytes: 0, maxBytes: 34003866, requestedBytes: 36195754
art memory: freeCount 879661, allocCount: 879661, currentBytes: 0, maxBytes: 34003866, requestedBytes: 36195754
config: {
  filename     : "./dict.bin.hot"
  fileSizeBytes: 45853558,
  format       : "bin-text"
  dataStructure: "art"
  hashAlgorithm: ""
  allocator    : "code default"
  needsHashAlgo: false,
  customAlloc  : false,
  runs         : 10,
  verbosity    : 0,
  coreId0      : 2,
  coreId1      : 4,
  coreId2      : 6,
  coreId3      : 8
}
Scaled Summary Statistics: 10 runs: ART Insert
C0 : [rdtsc cycles: use with F2                                   ] minValue: 243.68451        maxValue: 253.22887        avgValue: 245.44728       
F0 : [retired instructions                                        ] minValue: 345.35154        maxValue: 345.81221        avgValue: 345.51978       
F1 : [no-halt cpu cycles                                          ] minValue: 355.94812        maxValue: 360.98015        avgValue: 357.52406       
F2 : [reference no-halt cpu cycles                                ] minValue: 243.18875        maxValue: 246.64307        avgValue: 244.27252       
P0 : [LLC references                                              ] minValue: 10.03844         maxValue: 13.36795         avgValue: 12.91674        
P1 : [LLC misses                                                  ] minValue: 0.56324          maxValue: 0.96312          avgValue: 0.92041         
P2 : [retired branch instructions                                 ] minValue: 85.15652         maxValue: 85.25395         avgValue: 85.19328        
P3 : [retired branch instructions not taken                       ] minValue: 44.54928         maxValue: 44.60369         avgValue: 44.56835        
NSI: [nanoseconds per iteration                                   ] minValue: 71.50329         maxValue: 74.30386         avgValue: 72.02056        
OPS: [operations per second                                       ] minValue: 13985369.92900   maxValue: 13458251.88374   avgValue: 13884924.79173  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 325048320.00000  maxValue: 337779456.00000  avgValue: 327399756.80000 
Scaled Summary Statistics: 10 runs: ART Find
C0 : [rdtsc cycles: use with F2                                   ] minValue: 233.23340        maxValue: 234.57883        avgValue: 233.93271       
F0 : [retired instructions                                        ] minValue: 225.12540        maxValue: 225.59568        avgValue: 225.29879       
F1 : [no-halt cpu cycles                                          ] minValue: 340.70605        maxValue: 342.42088        avgValue: 341.62537       
F2 : [reference no-halt cpu cycles                                ] minValue: 232.77534        maxValue: 233.95875        avgValue: 233.40772       
P0 : [LLC references                                              ] minValue: 14.24628         maxValue: 15.57193         avgValue: 15.28599        
P1 : [LLC misses                                                  ] minValue: 1.18624          maxValue: 1.25014          avgValue: 1.23641         
P2 : [retired branch instructions                                 ] minValue: 59.14462         maxValue: 59.25034         avgValue: 59.18836        
P3 : [retired branch instructions not taken                       ] minValue: 36.58782         maxValue: 36.64224         avgValue: 36.60665        
NSI: [nanoseconds per iteration                                   ] minValue: 68.43665         maxValue: 68.83146         avgValue: 68.64186        
OPS: [operations per second                                       ] minValue: 14612054.58752   maxValue: 14528239.47659   avgValue: 14568369.76163  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 311107584.00000  maxValue: 312902400.00000  avgValue: 312040473.60000 
```
## HOT (Height Optimized Trie)
I initially had high hopes for HOT. I thought it'd beat ART handily. But insertion performance of `202.96023 ns/op`
is an order of 10 higher. Finds clock in at `81.09925 ns/op`. I did not benchmark memory because it's performance is
too slow. HOT retires 2152.62048 instructions per iteration with more LLC references but, like ART and CRadix, almost
no LLC misses.

```
taskset -c 5 ./benchmark.tsk -f ./dict.bin.hot -F bin-text -d hot
loading './dict.bin.hot'
config: {
  filename     : "./dict.bin.hot"
  fileSizeBytes: 45853558,
  format       : "bin-text"
  dataStructure: "hot"
  hashAlgorithm: ""
  allocator    : "code default"
  needsHashAlgo: false,
  customAlloc  : false,
  runs         : 10,
  verbosity    : 0,
  coreId0      : 2,
  coreId1      : 4,
  coreId2      : 6,
  coreId3      : 8
}
Scaled Summary Statistics: 10 runs: HOT Insert
C0 : [rdtsc cycles: use with F2                                   ] minValue: 690.94548        maxValue: 692.93342        avgValue: 691.69116       
F0 : [retired instructions                                        ] minValue: 2152.36850       maxValue: 2152.78835       avgValue: 2152.62048      
F1 : [no-halt cpu cycles                                          ] minValue: 1006.62524       maxValue: 1011.81385       avgValue: 1009.99367      
F2 : [reference no-halt cpu cycles                                ] minValue: 687.75706        maxValue: 691.29891        avgValue: 690.05238       
P0 : [LLC references                                              ] minValue: 18.91982         maxValue: 19.19026         avgValue: 19.02994        
P1 : [LLC misses                                                  ] minValue: 0.58097          maxValue: 0.68751          avgValue: 0.64916         
P2 : [retired branch instructions                                 ] minValue: 530.26357        maxValue: 530.34170        avgValue: 530.30878       
P3 : [retired branch instructions not taken                       ] minValue: 265.75163        maxValue: 265.80489        avgValue: 265.78353       
NSI: [nanoseconds per iteration                                   ] minValue: 202.74139        maxValue: 203.32475        avgValue: 202.96023       
OPS: [operations per second                                       ] minValue: 4932391.98425    maxValue: 4918240.45884    avgValue: 4927073.74925   
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 921646336.00000  maxValue: 924298240.00000  avgValue: 922641152.00000 
Scaled Summary Statistics: 10 runs: HOT Find
C0 : [rdtsc cycles: use with F2                                   ] minValue: 275.63572        maxValue: 277.28205        avgValue: 276.38705       
F0 : [retired instructions                                        ] minValue: 430.01599        maxValue: 430.01599        avgValue: 430.01599       
F1 : [no-halt cpu cycles                                          ] minValue: 402.79076        maxValue: 405.03214        avgValue: 403.71289       
F2 : [reference no-halt cpu cycles                                ] minValue: 275.19732        maxValue: 276.72276        avgValue: 275.82717       
P0 : [LLC references                                              ] minValue: 14.36829         maxValue: 14.63487         avgValue: 14.55280        
P1 : [LLC misses                                                  ] minValue: 0.54326          maxValue: 0.56571          avgValue: 0.55585         
P2 : [retired branch instructions                                 ] minValue: 66.02836         maxValue: 66.02836         avgValue: 66.02836        
P3 : [retired branch instructions not taken                       ] minValue: 33.04357         maxValue: 33.04357         avgValue: 33.04357        
NSI: [nanoseconds per iteration                                   ] minValue: 80.87882         maxValue: 81.36183         avgValue: 81.09925        
OPS: [operations per second                                       ] minValue: 12364176.10444   maxValue: 12290775.78173   avgValue: 12330570.67476  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 367668736.00000  maxValue: 369864448.00000  avgValue: 368670771.20000 
```

