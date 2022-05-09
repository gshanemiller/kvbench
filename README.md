# kvbench
task: benchmarks various hash, trie data structures using `rdpmc` found in this repository

# Goal

Choose a sorted KV data structure with performance within 20% of fast-hashing. Main candidate is tries. Intel PMU is used to help quantify differences.

# Background

Bucketed-hashmaps with per-bucket locking is common in high speed KV systems like RAMCloud. Now, as [Vinciguerra explains](http://www.vldb.org/pvldb/vol13/p1162-ferragina.pdf) explains:

```
Existing indexing data structures can be grouped into: (i) hash-based, which range from traditional hash tables to recent techniques, like Cuckoo
hashing [30]; (ii) tree-based, such as the B-tree and its variants [3, 6, 33, 35, 37]; (iii) bitmap-based [10, 38], which allow efficient set operations; and (iv) trie-based, which are commonly used for variable-length keys. Unfortunately, hash based indexes do not support predecessor or range searches;
bitmap-based indexes can be expensive to store, maintain and decompress [36]; trie-based indexes are mostly pointer based and, apart from recent results [15], keys are stored uncompressed thus taking space proportional to the dictionary size. As a result, the B-tree and its variants remain the
predominant data structures in commercial database systems for these kinds of queries [31].
```

[Vinciguerra's PGM](https://github.com/gvinciguerra/PGM-index) is a new devlopment in that solution space, however, you need a order-perserving hash unless your keys happen to be a built in type e.g float, uint64. See [PGM iussue 17](https://github.com/gvinciguerra/PGM-index/issues/17). PGM does not keep the key or values in the index. Therefore, storage is completely orthogonal to PGM. In a distributed KV store that might have practical benefits. I may circle back to this.

# Discarded

Unlike PGM and Google code which are well organized and expertly engineered repos that actually build, most code sets are a rats-nest of build errors and bad dependencies.

* https://gitlab.com/habatakitai/ctriepp supposedly quite good, but does not build. replete with errors
* https://github.com/malbrain/HatTrie.git does not build
