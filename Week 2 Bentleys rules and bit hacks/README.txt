# ANSWERS
## RECITATION
## RUNNING THE PROFILERS
follow instructions listed on:
https://gperftools.github.io/gperftools/cpuprofile.html
https://codearcana.com/posts/2013/02/26/introduction-to-using-profiling-tools.html

The main difference is using the -lprofiler flag during linking (DFLAGS), 
which works for both gcc and clang. This only seems to work when 'brew install gpperftools'. 
There is the bare pprof and also the go tool pprof. 
Added a dedicated section PROFILE in Makefile.

Building:
$ make clean
$ make sort PROFILE=1
$ CPUPROFILE=isort.prof ./isort 10000 10
$ PPROF_BINARY_PATH=/Users/SimonAertssen/Desktop/Zelfstudie/6.172\ Performance\ Engineering\ of\ Software\ Systems/MIT-6.172-Performance-Engineering-of-Software-Systems/Week\ 2\ Bentleys\ rules\ and\ bit\ hacks
$ pprof --text isort isort.prof

If a visual output is preferred, then use:
$ pprof --pdf isort isort.prof > results.pdf

Symbols not showing? 
See: https://users.rust-lang.org/t/pprof-wont-show-symbols/24241/2
Problem is partially solved with flags -Wl,-no_pie -fno-omit-frame-pointer -Wl,-export_dynamic

Despite everything, output is:
PROFILE: interrupts/evictions/bytes = 13/3/448
PPROF_BINARY_PATH=/Users/SimonAertssen/Desktop/Zelfstudie/6.172\ Performance\ Engineering\ of\ Software\ Systems/MIT-6.172-Performance-Engineering-of-Software-Systems/Week\ 2\ Bentleys\ rules\ and\ bit\ hacks
pprof --text isort isort.prof
Using local file isort.
Using local file isort.prof.
Total: 13 samples
      12  92.3%  92.3%       12  92.3% _isort
       1   7.7% 100.0%        1   7.7% 0x00007fff69bc631d
       0   0.0% 100.0%        1   7.7% 0x0000000100001007
       0   0.0% 100.0%       12  92.3% 0x0000000106c1df47
       0   0.0% 100.0%       13 100.0% 0x00007fff69bda3d4
       0   0.0% 100.0%        1   7.7% __mh_execute_header
       0   0.0% 100.0%       12  92.3% _main
These addresses are practically unreadable.

When using the DTU cluster we can run perf (built in):
$ make clean
$ make isort DEBUG=1
$ perf record ./isort 10000 10
$ perf report
## Overhead  Command  Shared Object     Symbol                
## ........  .......  ................  ......................
##
    99.60%  isort    isort             [.] isort
     0.25%  isort    [unknown]         [k] 0xffffffff9d18c4ef
     0.07%  isort    libc-2.17.so      [.] rand_r
     0.04%  isort    isort             [.] rand_r@plt
     0.02%  isort    isort             [.] main
     0.01%  isort    ld-2.17.so        [.] do_lookup_x
This seems to give similar results but at least we can do more.


## CHECKOFF ITEM 1:
A good instructional reading can be found on 
https://sandsoftwaresound.net/perf/perf-tutorial-hot-spots/

-- Elapsed time and event counting:
$ perf stat -e cpu-clock,faults ./isort 10000 10
## Performance counter stats for './isort 10000 10':
      929.74 msec cpu-clock:u          ##    0.999 CPUs utilized          
      178    faults:u                  ##    0.191 K/sec  
      0.938420000 seconds user
      0.001000000 seconds sys

-- Branch misses:
$ perf stat -e branch-misses ./isort 10000 10
## Performance counter stats for './isort 10000 10':
      103,968      branch-misses:u                                             
      0.862921126 seconds time elapsed
      0.862432000 seconds user
      0.000000000 seconds sys

-- CPU instruction cycles:
$ perf stat -e cpu-cycles,instructions ./isort 10000 10
## Performance counter stats for './isort 10000 10':
     2,270,497,213      cpu-cycles:u                                                
     3,264,379,290      instructions:u            ##1.44  insn per cycle         
      1.009240766 seconds time elapsed
      1.007062000 seconds user
      0.001000000 seconds sys

-- All at once:
$ perf stat -e branch-misses,cycles,instructions ./isort 10000 10
## Performance counter stats for './isort 10000 10':
           103,955      branch-misses:u                                             
     2,270,694,215      cycles:u                                                    
     3,264,379,838      instructions:u            ##1.44  insn per cycle         

       0.956292825 seconds time elapsed
       0.955825000 seconds user
       0.000000000 seconds sys

The biggest bottleneck might be the number of branch misses, 
as we seem to predict the wrong outcome many times.


## CHECKOFF ITEM 2:
$ lscpu
Architecture:          x86_64
CPU op-mode(s):        32-bit, 64-bit
Byte Order:            Little Endian
CPU(s):                20
On-line CPU(s) list:   0-19
Thread(s) per core:    1
Core(s) per socket:    10
Socket(s):             2
NUMA node(s):          2
Vendor ID:             GenuineIntel
CPU family:            6
Model:                 63
Model name:            Intel(R) Xeon(R) CPU E5-2660 v3 @ 2.60GHz
Stepping:              2
CPU MHz:               1199.865
CPU max MHz:           3300.0000
CPU min MHz:           1200.0000
BogoMIPS:              5188.20
Virtualization:        VT-x
L1d cache:             32K
L1i cache:             32K
L2 cache:              256K
L3 cache:              25600K
NUMA node0 CPU(s):     0-9
NUMA node1 CPU(s):     10-19
Flags:                 fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc aperfmperf eagerfpu pni pclmulqdq dtes64 ds_cpl vmx smx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid dca sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm epb invpcid_single intel_ppin ssbd ibrs ibpb stibp tpr_shadow vnmi flexpriority ept vpid fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms invpcid cqm xsaveopt cqm_llc cqm_occup_llc dtherm ida arat pln pts md_clear spec_ctrl intel_stibp flush_l1d

Here, the clock speed is very interesting: up to 3.3 GHz. Also, look at those flags!

(had to decrease array size because it just kept on running...)
$ valgrind --tool=cachegrind --branch-sim=yes ./sum > cachegrind_results.txt
==4682== Cachegrind, a cache and branch-prediction profiler
==4682== Copyright (C) 2002-2017, and GNU GPL'd, by Nicholas Nethercote et al.
==4682== Using Valgrind-3.14.0 and LibVEX; rerun with -h for copyright info
==4682== Command: ./sum
==4682== 
--4682-- warning: L3 cache found, using its data for the LL simulation.
--4682-- warning: specified LL cache: line_size 64  assoc 20  total_size 31,457,280
--4682-- warning: simulated LL cache: line_size 64  assoc 30  total_size 31,457,280
==4682== 
==4682== I   refs:      364,198,304
==4682== I1  misses:            954
==4682== LLi misses:            950
==4682== I1  miss rate:        0.00%
==4682== LLi miss rate:        0.00%
==4682== 
==4682== D   refs:       61,070,309  (40,051,975 rd   + 21,018,334 wr)
==4682== D1  misses:      9,984,649  ( 9,921,481 rd   +     63,168 wr)
==4682== LLd misses:         65,165  (     2,086 rd   +     63,079 wr)
==4682== D1  miss rate:        16.3% (      24.8%     +        0.3%  )
==4682== LLd miss rate:         0.1% (       0.0%     +        0.3%  )
==4682== 
==4682== LL refs:         9,985,603  ( 9,922,435 rd   +     63,168 wr)
==4682== LL misses:          66,115  (     3,036 rd   +     63,079 wr)
==4682== LL miss rate:          0.0% (       0.0%     +        0.3%  )
==4682== 
==4682== Branches:       21,039,823  (11,039,424 cond + 10,000,399 ind)
==4682== Mispredicts:         4,947  (     4,833 cond +        114 ind)
==4682== Mispred rate:          0.0% (       0.0%     +        0.0%   )

Looking at the D1 and LLd miss rates, the first is pretty high. 
It seems that for higher U and N, the cache miss rates increase exponentially.
However, we know that the L1d cache is about 32Kb. Setting U=1000 results in a 
miss rate of 0% for both caches. 


## HOMEWORK
## WRITEUP 1
For DEBUG=0 we get:
==6231== I   refs:      299,521,008
==6231== I1  misses:          1,392
==6231== LLi misses:          1,318
==6231== I1  miss rate:        0.00%
==6231== LLi miss rate:        0.00%
==6231== 
==6231== D   refs:      101,043,738  (64,728,902 rd   + 36,314,836 wr)
==6231== D1  misses:        300,773  (   163,057 rd   +    137,716 wr)
==6231== LLd misses:          5,227  (     2,562 rd   +      2,665 wr)
==6231== D1  miss rate:         0.3% (       0.3%     +        0.4%  )
==6231== LLd miss rate:         0.0% (       0.0%     +        0.0%  )
==6231== 
==6231== LL refs:           302,165  (   164,449 rd   +    137,716 wr)
==6231== LL misses:           6,545  (     3,880 rd   +      2,665 wr)
==6231== LL miss rate:          0.0% (       0.0%     +        0.0%  )
==6231== 
==6231== Branches:       53,372,694  (51,671,645 cond +  1,701,049 ind)
==6231== Mispredicts:     3,660,257  ( 3,660,012 cond +        245 ind)
==6231== Mispred rate:          6.9% (       7.1%     +        0.0%   )

For DEBUG=1 we get:
==6351== I   refs:      531,387,884
==6351== I1  misses:          1,382
==6351== LLi misses:          1,319
==6351== I1  miss rate:        0.00%
==6351== LLi miss rate:        0.00%
==6351== 
==6351== D   refs:      256,903,415  (206,216,746 rd   + 50,686,669 wr)
==6351== D1  misses:        308,609  (    171,178 rd   +    137,431 wr)
==6351== LLd misses:          5,220  (      2,558 rd   +      2,662 wr)
==6351== D1  miss rate:         0.1% (        0.1%     +        0.3%  )
==6351== LLd miss rate:         0.0% (        0.0%     +        0.0%  )
==6351== 
==6351== LL refs:           309,991  (    172,560 rd   +    137,431 wr)
==6351== LL misses:           6,539  (      3,877 rd   +      2,662 wr)
==6351== LL miss rate:          0.0% (        0.0%     +        0.0%  )
==6351== 
==6351== Branches:       59,153,714  ( 57,452,793 cond +  1,700,921 ind)
==6351== Mispredicts:     4,356,108  (  4,355,873 cond +        235 ind)
==6351== Mispred rate:          7.4% (        7.6%     +        0.0%   )

Result: in debug mode, we can see there are about twice as many references,
but the miss counts stay approximately the same. Therefore the miss rate 
increases for the optimised code. When only looking at instruction count, 
we can learn how optimised a code is (less instrcutions = better). However, 
we do not get any information on how efficient these instructions are executed
as there is no time information. Also, the miss rates look better for
unoptimised code, because there are many more instructions.

$ perf stat -e branch-misses,cycles,instructions ./sort 10000 10
Performance counter stats for './sort 10000 10':
         1,391,220      branch-misses:u                                             
       127,891,782      cycles:u                                                    
       299,490,874      instructions:u            ##    2.34  insn per cycle         

       0.085810121 seconds time elapsed
       0.083193000 seconds user
       0.001980000 seconds sys


## WRITEUP 2
Another version of merge_i and copy_i were copied and inlined.

$ perf stat -e branch-misses,cycles,instructions ./sort 10000 10
Performance counter stats for './sort 10000 10':
           700,360      branch-misses:u                                             
        64,315,740      cycles:u                                                    
       151,618,393      instructions:u            ##2.36  insn per cycle         

       0.044307892 seconds time elapsed
       0.041664000 seconds user
       0.001984000 seconds sys
Much faster and nearly half of the branch misses, cycles, and instructions!

==8893== I   refs:      151,650,605
==8893== I1  misses:          1,401
==8893== LLi misses:          1,313
==8893== I1  miss rate:        0.00%
==8893== LLi miss rate:        0.00%
==8893== 
==8893== D   refs:       50,968,066  (32,598,662 rd   + 18,369,404 wr)
==8893== D1  misses:        167,920  (    85,954 rd   +     81,966 wr)
==8893== LLd misses:          5,228  (     2,564 rd   +      2,664 wr)
==8893== D1  miss rate:         0.3% (       0.3%     +        0.4%  )
==8893== LLd miss rate:         0.0% (       0.0%     +        0.0%  )
==8893== 
==8893== LL refs:           169,321  (    87,355 rd   +     81,966 wr)
==8893== LL misses:           6,541  (     3,877 rd   +      2,664 wr)
==8893== LL miss rate:          0.0% (       0.0%     +        0.0%  )
==8893== 
==8893== Branches:       26,845,464  (25,944,519 cond +    900,945 ind)
==8893== Mispredicts:     1,793,393  ( 1,793,152 cond +        241 ind)
==8893== Mispred rate:          6.7% (       6.9%     +        0.0%   )
There are many less references and branches, resulting in a faster execution.
The miss rates are slightly higher.

If we add an inlined version of the utils.c functions, then we are even a little faster
but no difference is found in the cache results.


## WRITEUP 3
We can get the annotated assembly code with 
$ objdump -lSd sort > annotated_ASM.txt
Here it does not seem like the code is simply inlined.

Inlining functions increases compile time with increasing inline function size.
Also the size of the binary increases. The compiler can also just ignore the "inline"
keyword. However, the performance can increase up to 20% due to increased knowledge
of the context, as it is easier to optimise large functions.
Specifically for recursive functions: here the issue is that we cannot infinitely inline
the function body. At some point we have to stop, so we inline to a certain depth.
Set with ##pragma inline_recursion(on) and ##pragma inline_depth.

Inlining also imposes a cost on performance, due to the code expansion (due to duplication) hurting instruction cache performance. This is most significant if, prior to expansion, the working set of the program (or a hot section of code) fit in one level of the memory hierarchy (e.g., L1 cache), but after expansion it no longer fits, resulting in frequent cache misses at that level. Due to the significant difference in performance at different levels of the hierarchy, this hurts performance considerably. At the highest level this can result in increased page faults, catastrophic performance degradation due to thrashing, or the program failing to run at all. 

The cache misses can therefore be registered with valgrind.


## WRITEUP 4
When using pointers, we only use the memory address of the array + an offset
for the index. This is much lighter.

$ perf stat -e branch-misses,cycles,instructions ./sort 10000 10
Performance counter stats for './sort 10000 10':
           697,273      branch-misses:u                                             
        65,511,329      cycles:u                                                    
       151,618,252      instructions:u            ##    2.31  insn per cycle         

       0.027317088 seconds time elapsed
       0.023909000 seconds user
       0.002869000 seconds sys
The program is faster but the ratio of insn per cycle remains the same.


## WRITEUP 5
When using isort, another sorting algorithm, we can gain a speedup for smaller arrays.

$ perf stat -e branch-misses,cycles,instructions ./sort 10000 10
Performance counter stats for './sort 10000 10':
           546,892      branch-misses:u                                             
        31,021,060      cycles:u                                                    
        67,203,981      instructions:u            ##    2.17  insn per cycle         

       0.014478695 seconds time elapsed
       0.012907000 seconds user
       0.000992000 seconds sys
Again, the execution time has gone down drastically (factor of 4.5) but the ratio 
of insn per cycle is slightly smaller, regardless of the fact that the absolute
number of instructions and cycles is more than half.

When adding a custom branchless sorting algoritm:
$ perf stat -e branch-misses,cycles,instructions ./sort 10000 10
Performance counter stats for './sort 10000 10':
           185,953      branch-misses:u                                             
        41,881,764      cycles:u                                                    
        78,398,709      instructions:u            ##    1.87  insn per cycle         

       0.018087820 seconds time elapsed
       0.016512000 seconds user
       0.000971000 seconds sys
The number of branches has now been reduced heavily, but more instructions and cycles 
were needed.

Tried to incorporate a quicksort but it's actually really slow...


## WRITEUP 6
As a start, the classic pointer merge from sort_p was adjusted to only used one 
scratch space. Now we just keep using a reference to the original array. That is not
necessarily always a good thing.Because of the command to 'free()' the compiler 
cannot optimise on this scratch space.

$ perf stat -e branch-misses,cycles,instructions ./sort 10000 10
Performance counter stats for './sort 10000 10':
           539,242      branch-misses:u                                             
        32,704,193      cycles:u                                                    
        70,108,886      instructions:u            ##    2.14  insn per cycle         

       0.016385369 seconds time elapsed
       0.013937000 seconds user
       0.001858000 seconds sys

That's not bad, but it's also not a real improvement. 

But we can see there is another merging algorithm suggested in other writeups:
$ perf stat -e branch-misses,cycles,instructions ./sort 10000 10
Performance counter stats for './sort 10000 10':

           186,618      branch-misses:u                                             
        35,973,761      cycles:u                                                    
        78,647,102      instructions:u            ##    2.19  insn per cycle         

       0.015707568 seconds time elapsed
       0.012989000 seconds user
       0.002164000 seconds sys
With an additional filtering of the sorting criteria, this is a very fast method.


## WRITEUP 7
Allocate one buffer at the beginning and free it after all sorting is over. Pass the 
memory buffer between all functions.

$ perf stat -e branch-misses,cycles,instructions ./sort 10000 10
Performance counter stats for './sort 10000 10':

           186,251      branch-misses:u                                             
        35,695,915      cycles:u                                                    
        77,855,968      instructions:u            ##    2.18  insn per cycle         

       0.017212960 seconds time elapsed
       0.015579000 seconds user
       0.000973000 seconds sys

The numbers are only slightly better, and execution time is really dependant on other
users, but good job!

