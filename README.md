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
* Run command: `make -j 8`
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
All results are from [Equinix](https://console.equinix.com/) on-demand c3.small.x86 bare metal instance. 

## CRadix

Here's the first command:

```
./example/benchmark.tsk -f ./dict.bin.cradix -F bin-text -d cradix
```

This command reads `./dict.bin.cradix` and the inserts every key in the file into a new CRadix tree. Then, on the same
tree, every key in `./dict.bin.cradix` is searched. This is repeated 10 times snap shotting the resulting data. The
insert/find sequence is done two ways **for CRadix alone**. First it's done **without a SPSC** (single producer, single
consumer) queue. Here a single thread runs inserts/finds. Then it's done with a SPSC queue where one thread sends
insert/find commands over the queue, while another thread runs the insert/finds. Again, **no other data structure
benchmarked uses SPSC**. 

For each set of 10 runs the benchmark gives the quickest (min), longest (max), and average metric. The first data line
to look at is `NSI` nanoseconds/iteration or `OPS` (operations/second) which quickly summarize performance:

```
./example/benchmark.tsk -f ./dict.bin.cradix -F bin-text -d cradix
loading './dict.bin.cradix'
config: {
  filename     : "./dict.bin.cradix"
  fileSizeBytes: 43496622,
  format       : "bin-text"
  dataStructure: "cradix"
  hashAlgorithm: ""
  allocator    : "vanilla malloc or std::allocator"
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
C0 : [rdtsc cycles: use with F2                                   ] minValue: 177.82932        maxValue: 179.77454        avgValue: 178.80151       
F0 : [retired instructions                                        ] minValue: 310.59668        maxValue: 310.61272        avgValue: 310.59838       
F1 : [no-halt cpu cycles                                          ] minValue: 247.57177        maxValue: 249.47584        avgValue: 248.85532       
F2 : [reference no-halt cpu cycles                                ] minValue: 169.15065        maxValue: 170.45734        avgValue: 170.08109       
P0 : [LLC references                                              ] minValue: 8.06368          maxValue: 10.61815         avgValue: 8.59964         
P1 : [LLC misses                                                  ] minValue: 0.72530          maxValue: 0.94934          avgValue: 0.77121         
P2 : [retired branch instructions                                 ] minValue: 59.52857         maxValue: 59.52990         avgValue: 59.52873        
P3 : [retired branch instructions not taken                       ] minValue: 40.91113         maxValue: 40.91117         avgValue: 40.91115        
NSI: [nanoseconds per iteration                                   ] minValue: 52.17974         maxValue: 52.75054         avgValue: 52.46499        
OPS: [operations per second                                       ] minValue: 19164525.00291   maxValue: 18957150.29096   avgValue: 19060328.84574  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 237204992.00000  maxValue: 239799808.00000  avgValue: 238501708.80000 
Scaled Summary Statistics: 10 runs: CRadix Find
C0 : [rdtsc cycles: use with F2                                   ] minValue: 133.54341        maxValue: 136.40191        avgValue: 135.69191       
F0 : [retired instructions                                        ] minValue: 149.06026        maxValue: 149.06030        avgValue: 149.06029       
F1 : [no-halt cpu cycles                                          ] minValue: 195.00121        maxValue: 199.19319        avgValue: 198.16128       
F2 : [reference no-halt cpu cycles                                ] minValue: 133.22924        maxValue: 136.10152        avgValue: 135.39444       
P0 : [LLC references                                              ] minValue: 9.25547          maxValue: 10.98943         avgValue: 9.62411         
P1 : [LLC misses                                                  ] minValue: 1.08097          maxValue: 1.33715          avgValue: 1.13369         
P2 : [retired branch instructions                                 ] minValue: 33.43374         maxValue: 33.43375         avgValue: 33.43375        
P3 : [retired branch instructions not taken                       ] minValue: 23.34698         maxValue: 23.34698         avgValue: 23.34698        
NSI: [nanoseconds per iteration                                   ] minValue: 39.18512         maxValue: 40.02387         avgValue: 39.81553        
OPS: [operations per second                                       ] minValue: 25519888.34378   maxValue: 24985091.12715   avgValue: 25115825.41517  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 178132480.00000  maxValue: 181945344.00000  avgValue: 180998272.00000 
Scaled Summary Statistics: 10 runs: CRadix Insert with SPSC Queue
C0 : [rdtsc cycles: use with F2                                   ] minValue: 197.08603        maxValue: 204.22293        avgValue: 200.19575       
F0 : [retired instructions                                        ] minValue: 298.82205        maxValue: 331.44770        avgValue: 318.58599       
F1 : [no-halt cpu cycles                                          ] minValue: 282.05897        maxValue: 292.08773        avgValue: 286.42194       
F2 : [reference no-halt cpu cycles                                ] minValue: 196.63362        maxValue: 203.62676        avgValue: 199.67627       
P0 : [LLC references                                              ] minValue: 7.92447          maxValue: 13.52094         avgValue: 9.46566         
P1 : [LLC misses                                                  ] minValue: 0.24514          maxValue: 0.24788          avgValue: 0.24643         
P2 : [retired branch instructions                                 ] minValue: 71.70551         maxValue: 79.86192         avgValue: 76.64649        
P3 : [retired branch instructions not taken                       ] minValue: 24.91892         maxValue: 27.63444         avgValue: 26.56389        
NSI: [nanoseconds per iteration                                   ] minValue: 57.83014         maxValue: 59.92430         avgValue: 58.74263        
OPS: [operations per second                                       ] minValue: 17292020.01935   maxValue: 16687720.87203   avgValue: 17023413.46159  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 262891264.00000  maxValue: 272411136.00000  avgValue: 267039334.40000 
Scaled Summary Statistics: 10 runs: CRadix Find with SPSC Queue
C0 : [rdtsc cycles: use with F2                                   ] minValue: 165.45301        maxValue: 169.33818        avgValue: 167.14123       
F0 : [retired instructions                                        ] minValue: 295.59081        maxValue: 305.64934        avgValue: 299.91896       
F1 : [no-halt cpu cycles                                          ] minValue: 236.81538        maxValue: 242.52406        avgValue: 239.33592       
F2 : [reference no-halt cpu cycles                                ] minValue: 164.89685        maxValue: 168.85168        avgValue: 166.64309       
P0 : [LLC references                                              ] minValue: 6.70423          maxValue: 7.07149          avgValue: 6.84820         
P1 : [LLC misses                                                  ] minValue: 0.24289          maxValue: 0.24548          avgValue: 0.24437         
P2 : [retired branch instructions                                 ] minValue: 70.89770         maxValue: 73.41233         avgValue: 71.97973        
P3 : [retired branch instructions not taken                       ] minValue: 25.19150         maxValue: 26.02587         avgValue: 25.55095        
NSI: [nanoseconds per iteration                                   ] minValue: 48.54821         maxValue: 49.68823         avgValue: 49.04359        
OPS: [operations per second                                       ] minValue: 20598082.46916   maxValue: 20125489.07648   avgValue: 20390025.42595  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 220696320.00000  maxValue: 225878784.00000  avgValue: 222948275.20000 
```

When run **without SPSC** it takes `52.46499 ns/op` to insert and `39.81553 ns/op` to find. Adding SPSC overhead these
numbers increase to `58.74263` and `49.04359 ns/op` respectively. The P0/P1 data lines show there are a few LLC refs
and almost 0 LLC misses. That's because the test file is not particularly big containing only ~500,000 unique words.
Comparing C0 to F2 we can see the code doesn't spend too much time halted waiting for resources. Finally, comparing
F0 to P2 and P3 note branch instructions are almost half of what the CPU does. The number of iterations appears in data
line `N` (constant 4545921) which is what the generator command above reported. After repeated testing CRadix does
sub-100 ns/operation work which puts it into the same order of 10 as hashing. 

## Facebook's F14 Hashmap

For all other data structures it's imperative to run the benchmark pinned to a core. The CPU identifiers `coreId0`
etc. appearing in the config data are used only by CRadix. That's why this command is prefixed by `taskset`. Facebook's
F14 hashmap using the `xxhash:XX3_64bits` hashing algo gives `33.55490 ns/op` insert and `25.91206 ns/op' find with
approximately the same LLC metrics. It spends less time branching compared to CRadix because it's not fundamentally
organized as a tree of nodes.

```
taskset -c 5 ./example/benchmark.tsk -f ./dict.bin -F bin-text -d f14 -h xxhash:XX3_64bits 
loading './dict.bin'
config: {
  filename     : "./dict.bin"
  fileSizeBytes: 41307637,
  format       : "bin-text"
  dataStructure: "f14"
  hashAlgorithm: "xxhash:XX3_64bits"
  allocator    : "vanilla malloc or std::allocator"
  needsHashAlgo: true,
  customAlloc  : false,
  runs         : 10,
  verbosity    : 0,
  coreId0      : 2,
  coreId1      : 4,
  coreId2      : 6,
  coreId3      : 8
}
Scaled Summary Statistics: 10 runs: F14 Insert
C0 : [rdtsc cycles: use with F2                                   ] minValue: 113.51912        maxValue: 118.13129        avgValue: 114.35575       
F0 : [retired instructions                                        ] minValue: 207.74573        maxValue: 207.74834        avgValue: 207.74705       
F1 : [no-halt cpu cycles                                          ] minValue: 158.76499        maxValue: 167.61762        avgValue: 161.99272       
F2 : [reference no-halt cpu cycles                                ] minValue: 110.09121        maxValue: 114.52474        avgValue: 110.90500       
P0 : [LLC references                                              ] minValue: 6.53855          maxValue: 7.26735          avgValue: 6.73450         
P1 : [LLC misses                                                  ] minValue: 1.17671          maxValue: 1.26706          avgValue: 1.20137         
P2 : [retired branch instructions                                 ] minValue: 36.14632         maxValue: 36.14694         avgValue: 36.14663        
P3 : [retired branch instructions not taken                       ] minValue: 18.94157         maxValue: 18.94244         avgValue: 18.94189        
NSI: [nanoseconds per iteration                                   ] minValue: 33.30958         maxValue: 34.66287         avgValue: 33.55490        
OPS: [operations per second                                       ] minValue: 30021393.09081   maxValue: 28849315.71737   avgValue: 29801910.11825  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 151422720.00000  maxValue: 157574656.00000  avgValue: 152537907.20000 
Scaled Summary Statistics: 10 runs: F14 Find
C0 : [rdtsc cycles: use with F2                                   ] minValue: 86.14668         maxValue: 99.70892         avgValue: 88.30825        
F0 : [retired instructions                                        ] minValue: 170.45465        maxValue: 170.45721        avgValue: 170.45636       
F1 : [no-halt cpu cycles                                          ] minValue: 125.80771        maxValue: 145.66379        avgValue: 128.93414       
F2 : [reference no-halt cpu cycles                                ] minValue: 85.95366         maxValue: 99.51830         avgValue: 88.09124        
P0 : [LLC references                                              ] minValue: 5.71993          maxValue: 7.67854          avgValue: 6.05827         
P1 : [LLC misses                                                  ] minValue: 1.03748          maxValue: 1.21057          avgValue: 1.07355         
P2 : [retired branch instructions                                 ] minValue: 29.53234         maxValue: 29.53293         avgValue: 29.53272        
P3 : [retired branch instructions not taken                       ] minValue: 14.60601         maxValue: 14.60636         avgValue: 14.60622        
NSI: [nanoseconds per iteration                                   ] minValue: 25.27776         maxValue: 29.25732         avgValue: 25.91206        
OPS: [operations per second                                       ] minValue: 39560460.50360   maxValue: 34179478.85569   avgValue: 38592069.27174  
N  : [iterations                                                  ] minValue: 4545921.00000    maxValue: 4545921.00000    avgValue: 4545921.00000   
NS : [nanoseconds elapsed                                         ] minValue: 114910720.00000  maxValue: 133001472.00000  avgValue: 117794176.00000 
```

## ART (Adaptive Radix Trie)

