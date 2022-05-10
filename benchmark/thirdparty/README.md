Everything in this directory is third-party code not written by me. Identifying URLs are provided by running
`benchmark` (no arguments), or is self-described in code here, and in per diectory README.md files. 

I've included the code directly without recourse to submodules or other dependencies for end-user ease-of-use:

- C/C++ has crappy build dependency management
- some original code doesn't built needing remedial fixes
- some original code has its own rats nest of dependency non-sense
- some original code didn't use cmake. We don't want 8 build systems here
- to keep focus on benchmarking not build-hell rabbit holes
- support building third-party code multiple ways if there's a chance of better performance

As such I may have made minor changes to the author's original code (but never the algorithm) so that it folds
into this eco system
