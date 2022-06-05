# Goal
Benchmark a variety of hash, tree, and trie implementations on large KV data sets. The basic approach:

1. Test sets are stored in a file
2. Using transparent huge memory support, load a test set into shared memory
3. Insert all data from (2) by scanning the data from beginning to end in order performing inserts, updates, deletes
4. Report timings in ns/op together with stats collected from Intel's PMU functionality including CPU cache hit/misses

The benchmark code does not sort or organize the file loaded. So, for example, if one wishes to benchmark data in
sorted order, provide a file with data presorted. For random order, provide a file with data in random order.

The data structures will point to data in memory loaded in (2) unless the data structure can't support that. For
example, hashmaps often can point back to (2) both for keys, values whereas trie structures must make a copy of the
key because key ordering is structurally dependent on it.

# Features
* This respository is self contained and self building. There are no dependencies and no GIT sub-modules. This comes at
the cost of copying third-party code into this repo rarely with small edits to make the code build. Build hell is real!
I've tried to clearly advertise where the original code came from so its authors get credit for their know-how.

* Simple build, simple setup

* Supports **xxhash, t1ha, cityhash** algorithms which may be intermixed with a hash map algorithm

* Hashing algorithms supported: **cuckoo** hash map, **Facebook's F14** hash map

* Trie algos supported: **HOT** Height Optimized Trie. Per HOT paper it beats ART (Adaptive Radix Tree)

* Tree algos supported: **None**: binary search trees, btrees regularly underpeform tries

* Learned Indexes: **None** at present. Too many papers work with integral key types e.g. uint64. But if keys are real
world, one needs a map from the string to an integral type. How best to do this is an open question

* Supports two memory allocators: default STL allocator, and Microsoft's MIM allocator each with one or two variations

* Intel PMU metrics (programmable!) provided with each benchmark

* SIMD support: some algos have SIMD equivalents or enablements

* Generator to make KV pairs, and to convert or help convert data you might have laying around ready for benchmarking

* Test Data is preloaded and organized into memory before the bechmark runs. So even if you have Gbs of KV pairs
disk I/O, TLB misses, and random memory I/O getting to the data before the code-under-test runs is minimized

* Decently documented

# PMU Background
The PMU code was developed by me. I copied it into this repository, again, to keep build dependencies down. Find the
[original source code with extensive documentation here](https://github.com/rodgarrison/rdpmc).

# Environments Supported
Code verified to run on:

* Ubuntu 20.04 LTS
* Equinix c3.small.x86 ($0.50/hr) (1) Intel Xeon E-2278G CPU 3.4Ghz 32Gb 4-channel RAM
* GNU 9.4.0 or better
* Last tested May 2022

No windows support. Your CPU must have Intel PMU and basic SIMD support.

# Building (Est 10mins)
There are three approaches available to you. If you already have an Intel x86 Linux system with a GNU C/C++ tool chain
and required dependencies then clone `kvbench` and build. The only atypical dependency is `libhuge*`:

* Run command: `git clone https://github.com/rodgarrison/kvbench`
* Run command: `cd kvbench`
* Run command: `mkdir build`
* Run command: `cd build`
* Run command: `cmake ..`
* Run command: `make -j 8`
* Proceed to setup below

If you don't have the tool chain and/or libraries and don't wanna waste your time figuring out what's needed get a throw
away Ubuntu 20.04 LTS bare metal machine:

* Provision a machine (e.g. AWS EC2 or Equinix c3.small.x86 recommended) preferrably bare metal. It must be `apt` based
* [Copy the install script contents](https://github.com/rodgarrison/kvbench/blob/main/benchmark/scripts/install) onto
your machine in your user's home directory called, say, `install`. Git provides a copy-contents button for this purpose
* `chmod 755 ./install`
* `sudo ./install` to download dependencies with apt (which requirs root) **and** built kvbench
* Proceed to setup

Finally, there's a hybrid approach: [lift the `apt` command line](https://github.com/rodgarrison/kvbench/blob/main/benchmark/scripts/install)
to install whatever you might be missing, then fall back to self-build in approach 1.

`kvbench` itself installs nothing. All tasks are found in  `~/Dev/kvbench/build`.

# Setup (Est 5 mins)
* Build code; see previous section
* Enable TLB/huge-pages. It's usually simplest and best to allocate a number of 1Gb huge pages equal to the
maximum size of your test set. The memory required for any one test file is equal to the size in bytes of that
file as reported by `ls -la`. Round the largest size found to next highest 1Gb size. Run `scripts/huge_1gb_pages <N>`
where `N` is the number of pages you need e.g. 1.5Gb file needs 2 1Gb pages so `N=2`
* Enable PMU in userspace by running `scripts/linux_pmu on`. Benchmarks will segfault otherwise. `rdpmc` has details
on this
* Optionally disable CPU hyper-threading by running `scripts/intel_ht off`
* Optionally disable NMI by running `scripts/linux_nmi off`; this has not been thoroughly vetted by me

All steps are one-time, must be run as root/sudo, and should be done before running the benchmark task. Benchmarking
code always deallocates memory on exit so `ipcs -m` will not show stranded allocations by this code. You can always
force remove allocations if required with `sudo ipcrm -m <id>` where id is from `sudo ipcs -m`. The most likely case
for `ipcrm` is on benchmark crash. And a crash almost always means input file is not valid.

If you don't have test files handy see the next section for options.

# Getting Data
This benchmark suite works with three kinds of data sets:

* Files containing whitespace separated text words transformed into a `-F bin-text` binary file with a one-liner; see
below. Since it does not contain KV pairs only words, these files are used to benchmark key performance where the value
is a constant `bool` set false. In these tests, no memory (well, no serious memory) is spent holding values. Each word
`W` in the file is considered to be a key. The KV pair `(W, false)` is inserted into the test structure. Keys or words
here are modeled in code as `Benchmark::Slice<char>`.

* Binary files holding explicit text KV pairs aka `-F bin-text-kv` also modeled as `Benchmark::Slice<char>`

* Binary files holding explicit blob KV pairs aka `-F bin-slice-kv` modeled as `Benchmark::Slice<unsigned char>`.

If you cannot transform a KV file already into your possession into the supported format, you can run the generator
task in this repository to make one. (NOTE: this task is not yet written but is coming). This will also give you
control over the size and distribution of key/values.

[In the alternative this web site](https://people.eng.unimelb.edu.au/sgog/data/) has a variety of text files you can
obtain with `curl` or `wget`. [The Guttenberg Org](https://www.gutenberg.org/) also has good files:

* [Shakespeare's Collected Works](https://www.gutenberg.org/cache/epub/100/pg100.txt)
* [English Dictionary](https://www.gutenberg.org/cache/epub/29765/pg29765.txt)

Vanilla text files are the simplest way to get a decent look at a test configuration. Text files are easy to make and
are readily available. The `generator.tsk` program in this repository has a conversion tool to transform them into
`bin-text` suitable for benchmarking. See the next section for a worked example.

# Worked Example
Obtain a test file:

* `curl -o dict.txt https://www.gutenberg.org/cache/epub/29765/pg29765.txt`

Convert `dict.txt` into `dict.bin`:

* `./generator.tsk -m convert-text -i dict.txt -o dict.bin`

You'll see output like this:

```
$ ./generator.tsk -m convert-text -i ./dict.txt -o ./dict.bin
reading './dict.txt' ...
000000000 bytes left
writing './dict.bin' ...
wrote 004545921 words
```

As of this writing we'll need a zero terminator on the end of strings for the HOT trie code. Add `-t`:

```
$ ./generator.tsk -m convert-text -i ./dict.txt -o ./dict.bin.trie -t`
```

The conversion finds all 4545921 words in `dict.txt` writing into `dict.bin`. Append `-v` to command line to see each
word found on stdout. A word is just the ASCII text sitting between whitespaces. There's only ~500,000 unique words in
this file with average key length of `28956348/4545921` about 6 chars or less. `28956348` is the filesize of `dict.txt`
including whitespaces.

We'll now benachmark this file using three algorithms:

* Cuckoo hash map
* Facebook's F14 hash map
* HOT Trie

The first benchmark will be explained at length. The other two will be summarized.

## Cuckoo Hashmap on dict.bin

```
# Run code pinned to CPU core 5
$ taskset -c 5 ./benchmark.tsk -f ./dict.bin -F bin-text -d cuckoo -h xxhash:XX3_64bits
loading './dict.bin'
000000000 bytes left
config: {
  filename     : "./dict.bin"
  fileSizeBytes: 41307637,
  format       : "bin-text"
  dataStructure: "cuckoo"
  hashAlgorithm: "xxhash:XX3_64bits"
  allocator    : "vanilla malloc or std::allocator"
  needsHashAlgo: true,
  customAlloc  : false,
  runs         : 10,
  recordRuns   : 1,
  verbosity    : 0
}
"stats": {
  "legend" = {
    "F0": "retired instructions",
    "F1": "no-halt cpu cycles",
    "F2": "reference no-halt cpu cycles",
    "P0": "LLC references",
    "P1": "LLC misses",
    "P2": "retired branch instructions",
    "P3": "retired branch instructions not taken",
    "IPC": "F0/F2",
    "branchRatio": "P2/F0",
    "branchWasteRatio": "P3/F0",
  },
  "result" = [
    {
      "description": "benchmark-overhead run 9",
      "total": {
        "iterations": 4545921,
        "elapsedTimeNs": 1723206.000000,
        "F0": 31821564,
        "F1": 8376825,
        "F2": 5825834,
        "P0": 0,
        "P1": 0,
        "P2": 4545934,
        "P3": 3,
      },
      scaledPerIteration: {
        "elapsedTimeNs": 0.379066,
        "opsPerSecond" : 2638060104.247548,
        "F0": 7.000026,
        "F1": 1.842712,
        "F2": 1.281552,
        "P0": 0.000000,
        "P1": 0.000000,
        "P2": 1.000003,
        "P3": 0.000001,
        "IPC": 5.462147,
        "branchRatio": 0.142857
        "branchWasteRatio": 0.000000
      },
    }
  ]
}
config: {
  filename     : "./dict.bin"
  fileSizeBytes: 41307637,
  format       : "bin-text"
  dataStructure: "cuckoo"
  hashAlgorithm: "xxhash:XX3_64bits"
  allocator    : "vanilla malloc or std::allocator"
  needsHashAlgo: true,
  customAlloc  : false,
  runs         : 10,
  recordRuns   : 1,
  verbosity    : 0
}
"stats": {
  "legend" = {
    "F0": "retired instructions",
    "F1": "no-halt cpu cycles",
    "F2": "reference no-halt cpu cycles",
    "P0": "LLC references",
    "P1": "LLC misses",
    "P2": "retired branch instructions",
    "P3": "retired branch instructions not taken",
    "IPC": "F0/F2",
    "branchRatio": "P2/F0",
    "branchWasteRatio": "P3/F0",
  },
  "result" = [
    {
      "description": "insert run 9",
      "total": {
        "iterations": 4545921,
        "elapsedTimeNs": 342085712.000000,
        "F0": 1485166525,
        "F1": 1701077861,
        "F2": 1162221010,
        "P0": 111786864,
        "P1": 21959335,
        "P2": 239749053,
        "P3": 96207229,
      },
      scaledPerIteration: {
        "elapsedTimeNs": 75.251134,
        "opsPerSecond" : 13288836.220087,
        "F0": 326.703109,
        "F1": 374.198729,
        "F2": 255.662386,
        "P0": 24.590587,
        "P1": 4.830558,
        "P2": 52.739380,
        "P3": 21.163419,
        "IPC": 1.277869,
        "branchRatio": 0.161429
        "branchWasteRatio": 0.064779
      },
    }
    {
      "description": "find run 9",
      "total": {
        "iterations": 4545921,
        "elapsedTimeNs": 274040580.000000,
        "F0": 1043742704,
        "F1": 1364296170,
        "F2": 932093538,
        "P0": 79255216,
        "P1": 16788133,
        "P2": 170012816,
        "P3": 78892110,
      },
      scaledPerIteration: {
        "elapsedTimeNs": 60.282741,
        "opsPerSecond" : 16588495.762197,
        "F0": 229.599833,
        "F1": 300.114360,
        "F2": 205.039537,
        "P0": 17.434358,
        "P1": 3.693010,
        "P2": 37.398982,
        "P3": 17.354483,
        "IPC": 1.119783,
        "branchRatio": 0.162888
        "branchWasteRatio": 0.075586
      },
    }
  ]
}
```

The top most of the output shows the config:

```
config: {                                                                                                               
  filename     : "./dict.bin"                                                                                           
  fileSizeBytes: 41307637,                                                                                              
  format       : "bin-text"                                                                                             
  dataStructure: "cuckoo"                                                                                               
  hashAlgorithm: "xxhash:XX3_64bits"                                                                                    
  allocator    : "vanilla malloc or std::allocator"                                                                     
  needsHashAlgo: true,                                                                                                  
  customAlloc  : false,                                                                                                 
  runs         : 10,                                                                                                    
  recordRuns   : 1,                                                                                                     
  verbosity    : 0                                                                                                      
}
```

The converted file has 41307637 bytes (versus 28956348 in the original raw file) with 4545921 words as we know from the
generator command earlier. The default config is to run each benchmark 10 times only recording the last `recordRuns=1`
runs. You can adjust these numbers on the benchmark command line. The default config also runs with the builtin,
standard memory allocator.

A legend section is then given where each PMU counter is given a mnemonic name e.g. `P0` with a description. The code
provides a seven counters (F0-F2, and P0-P3) with a couple of derived values:

```
  "legend" = {                                                                                                          
    "F0": "retired instructions",                                                                                       
    "F1": "no-halt cpu cycles",                                                                                         
    "F2": "reference no-halt cpu cycles",                                                                               
    "P0": "LLC references",                                                                                             
    "P1": "LLC misses",                                                                                                 
    "P2": "retired branch instructions",                                                                                
    "P3": "retired branch instructions not taken",                                                                      
    "IPC": "F0/F2",                                                                                                     
    "branchRatio": "P2/F0",                                                                                             
    "branchWasteRatio": "P3/F0",                                                                                        
  },
```

Three results sets are reported two ways. The first pair is `benchmark-overhead`. The same benchmark code that's run
on the cuckoo hashmap is first run over an empty-loop scanning the words in `dict.bin` loaded into huge-page memory.
It includes setup and tear-down. Since `dict.bin` contains 4545921 words the benchmark records 4545921 iterations. This
empty loop is run 10 times and only the last run is recorded. The first block shows the total number of operations,
total elapased time, and PMU counter values at test end. These numbers are snap-shotted once before and after the test:

```
  "result" = [                                                                                                          
    {                                                                                                                   
      "description": "benchmark-overhead run 9",                                                                        
      "total": {                                                                                                        
        "iterations": 4545921,                                                                                          
        "elapsedTimeNs": 1723206.000000,                                                                                
        "F0": 31821564,                                                                                                 
        "F1": 8376825,                                                                                                  
        "F2": 5825834,                                                                                                  
        "P0": 0,                                                                                                        
        "P1": 0,                                                                                                        
        "P2": 4545934,                                                                                                  
        "P3": 3,                                                                                                        
      }, 
```

The second block records the same data except that it's scaled down by `4545921` since that was the number of words
scanned. This gives you a slightly more useful resource consumption rate:

```
      scaledPerIteration: {                                                                                             
        "elapsedTimeNs": 0.379066,                                                                                      
        "opsPerSecond" : 2638060104.247548,                                                                             
        "F0": 7.000026,                                                                                                 
        "F1": 1.842712,                                                                                                 
        "F2": 1.281552,                                                                                                 
        "P0": 0.000000,                                                                                                 
        "P1": 0.000000,                                                                                                 
        "P2": 1.000003,                                                                                                 
        "P3": 0.000001,                                                                                                 
        "IPC": 5.462147,                                                                                                
        "branchRatio": 0.142857                                                                                         
        "branchWasteRatio": 0.000000                                                                                    
      }, 
```

The point of this data is to argue the benchmark setup time and memory scanner overhead is not a significant
contributor to the real benchmarks which appear next. This loop is quick on average ~0.5ns/word. It incurs no LLC cache
misses. That is P0, P1 record 0.0. LLC cache miss means the CPU had to wait for memory to be fetched from RAM into the
last level CPU cache (typically level 3 or L3) stalling it. The further removed required data is from a CPU register
falling back to L1, then L2, then L3, or worst case RAM [you incur an additional latency factor](https://pmem.io/blog/2019/12/300-nanoseconds-1-of-2/)
(approximately) of 10. That's why Intel considers LLC hit/misses to be architecturally signifcant.

IPC (instructions retired per retired cycle) is well above 1.0. Modern CPUs are super scalar, and ideally, can complete
multiple instructions per cycle if it can keep its pipeline busy. This loop does that. 

This broadly makes sense. The word scanner works over memory in a non-random increasing only order so the CPU can
prefetch data before it needs it. The scanner does not walk each byte looking for word boudaries. It's pre-computed
in `dict.bin`. The current word variable holding the next assignment stays CPU local-hot.

The next two sections record the time it took the Cuckoo hashmap to insert all 4545921 words giving total, elapsed
values and scaled values:

```
    {                                                                                                                   
      "description": "insert run 9",                                                                                    
      "total": {                                                                                                        
        "iterations": 4545921,                                                                                          
        "elapsedTimeNs": 342085712.000000,                                                                              
        "F0": 1485166525,                                                                                               
        "F1": 1701077861,                                                                                               
        "F2": 1162221010,                                                                                               
        "P0": 111786864,                                                                                                
        "P1": 21959335,                                                                                                 
        "P2": 239749053,                                                                                                
        "P3": 96207229,                                                                                                 
      },                                                                                                                
      scaledPerIteration: {                                                                                             
        "elapsedTimeNs": 75.251134,                                                                                     
        "opsPerSecond" : 13288836.220087,                                                                               
        "F0": 326.703109,                                                                                               
        "F1": 374.198729,                                                                                               
        "F2": 255.662386,                                                                                               
        "P0": 24.590587,                                                                                                
        "P1": 4.830558,                                                                                                 
        "P2": 52.739380,                                                                                                
        "P3": 21.163419,                                                                                                
        "IPC": 1.277869,                                                                                                
        "branchRatio": 0.161429                                                                                         
        "branchWasteRatio": 0.064779                                                                                    
      },                                                                                                                
    }
```

An insert requires ~75ns/op with 25 LLC cache hits and ~5 LLC cache misses per insert. This tranlates to about 13,288,836
inserts per second. Broadly speaking there are two factors. First, hashing fundamentally means random I/O since keys are
pseudo randomly assigned different buckets. The CPU has no way to predict which bucket so that it cannot prefectch memory
to avoid stalls. Second, the hashing algorithm will interplay with a hashmap's overflow logic adding or reorganizing
buckets which leads to more random memory accesses.

IPC is only 1.2. About 16% of all instructions are branch related, of which ~6.5% of the total ~327 instructions per
insert (e.g. about 21 instructions) were evaluated and never used.

The find operations are pretty much the same clocking in somewhat faster at 60ns/find. In many KV data structures,
the first 70% of an insert is find. The data reflects this.

## Facebook's F14 Hashmap

```
$ taskset -c 5 ./benchmark.tsk -f ./dict.bin -F bin-text -d f14 -h xxhash:XX3_64bits
```

This algorithm is 2x better. Inserts are 27ns/op and finds are 26ns/op. LLC cache hits are 9 with 2 misses per operation
insert. Find has slightly better numbers here.

## HOT Trie

A trie is fundamentally different from a hashmap. It makes copies of the keys then orders them. Consequently it does lot
more work. You must run this on a bin file with terminators. Refer to the previous section running generator with `-t`
option:

```
$ taskset -c 5 ./benchmark.tsk -f ./dict.bin.trie -F bin-text -d hot
```

Inserts run at 237ns/op with finds 83ns/op. Interestingly, HOT has almost no LLC misses. 

# Typical and Notable Benchmark Commandlines
Start here for typical benchmark invocations:
(TBD)

# Benchmark Highlights
(In progress)
In this section, I summarize notable findings giving their benchmark invocation line placing stats into context.
These results represent the best performance of all algorithm permutations to date.

# Guidance on Running and Testing
* Prefer building on bare metal. How/if virtual CPUs weigh into Intel PMU data and how virtual environments effect
performance is not addressed here. Here again I've found [Equinix](https://console.equinix.com) very cheap, and darn
easy to use. It's at least twice cheaper than bare metal `c5n` AWS EC2 instances and, like AWS, bills by the hour.
* Always `taskset` the benchmark task. PMU data is useless if the task bounces around cores.
* Throw away the first couple of test runs, and run each iteration on the same core unless you want to see how the
code runs code
* Strongly consider disabling CPU hyper-threading. There are three general reasons. More kernel work can run on
your test core otherwise. Second, as [rdpmu](https://github.com/rodgarrison/rdpmc/blob/main/doc/pmu.md#overview)
explains, Intel PMU data is easier to interpret if it's not polluted by other CPU threads running on the same core
the kernel may schedule there. Finally, fewer CPU threads in the same core may mean less CPU cache contention. The
`scripts` directory contains `intel_ht` for this purpose. Run with `on` to enable or `off` to disable
* Intel PMU metrics report, where needed, reference cycle counts which run at a fixed frequency. This eliminates a
whole rats nest of issues configuring the system under test to put it into performance mode, disable power savings
and so on. Variable CPU frequency issues are avoided. Usually we just need instructions-per-cycle ratios to compare 
across algorithms. Retired cycles at an unchanging reference frequency does this.
* Double check the usage line (`benchmark.tsk` with no args) to see if there's an alternate memory allocator for your
preferred algorithm. Allocations may have a large effect on performance
* Double check the usage line (`benchmark.tsk` with no args) to see if there's a variation of your preferred algorithm
with SIMD
* Hashmaps cannot and do not sort so generally run faster than alternatives
* Check memory utilization: don't trade fast performance for terrible memory unless you know this ahead of time.
* Although not documented here, it's possible to isolate kernel work on a core set. [DPDK](https://dpdk.org) is one
source. [This Intel page is another](https://www.intel.com/content/www/us/en/developer/articles/guide/dpdk-performance-optimization-guidelines-white-paper.html). That means you can task set your benchmark code on other cores. From a pareto standpoint SIMD,
a better allocator, or a better algorithm will give you much better results.
* Intel PMU events are smartly organized as (CPU) architectual or not. From a CPU core perspective it's architectually
important if the CPU has lots of LLC misses because those are much more expensive than core-to-core transfers, or L1/2/3
CPU cache lookups, or just running routine jmp, add, bit-logic instructions. Wasting a lot cycles in branch instructions
that are never taken is also expensive. That's why I've included those PMU stats. LLC misses don't have simple
relationship to IPC. If the CPU is waiting for cache line fills from RAM, it's not executing instructions even as
cycles go by. So consider LLC misses and average ns/iteration together.
* Intel PMU has a rich set of events. Feel free to change the default set. [rdpmu](https://github.com/rodgarrison/rdpmc)
will give you those details. Recall, as explained above, the Intel PMU code in this repository is a straight copy from
[rdpmc](https://github.com/rodgarrison/rdpmc). Therefore what applies there applies here unchanged.
