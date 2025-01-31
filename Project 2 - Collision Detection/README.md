# Project 2 Writeup:

## 0. Performance improvements:
The most important result in this writeup is the performance we obtain with every new algorithm. As we saw from our measurements, the `koch.in` animation takes the longest time to run: at 1000 frames it takes 449.35 seconds. Therefore, test performance with `./screensaver 1000 input/koch.in`.

Here, the performance and speedup is given per adjustment.

### Serial performance improvements:

| Description | Execution time (s) | Speedup (relative) | Speedup (absolute) |
| - | - | - | - |
| SnailSpeed's implementation (naive) | 449.35 | x 1 | x 1 |
| Precompute line lengths | 449.35 | x 1 | x 1 |
| Check bounding boxes | 241.50 | x 1.86 | x 1.86 |
| Inline vec.h | 241.50 | x 1 | x 1.86 |
| Quadtree | 230.90 | x 1.05 | x 1.95 |
| Update Quadtree without destroying it | | | |
| More efficient way of checking intersections | | | |

### Parallel performance improvements:

| Description | Execution time (s) | Speedup (relative) | Speedup (absolute) |
| - | - | - | - |
| SnailSpeed's implementation (naive) | | | |
| | | | |

--- 

## Testing the number of collisions:
As to verify that our improved algorithms are correct, we need to know how many collisions were detected for every animation. 

These measurements are taken on 1000 frames:

| Animation Name | Execution Time [s] | Line-Wall Collisions | Line-Line Collisions |
| - | - | - | - |
| apple.in_new | 8.685269s | 631 | 19729 | 
| apple.in_old | 9.635677s | 655 | 18718 | 
| beaver.in | 2.710521s | 7 | 758 | 
| box.in | 5.078032s | 1379 | 36965 |
| explosion.in | 11.922028s | 206 | 16837 |
| koch.in | 449.353282 s | 1088 | 5256 |
| mit.in | 20.651894s | 170 | 2097 | 
| sin_wave.in | 52.263763s | 504 | 280083 |
| smalllines.in | 8.939444s | 5837 | 77802 | 


These measurements are taken on 4000 frames:

| Animation Name | Execution Time [s] | Line-Wall Collisions | Line-Line Collisions |
| - | - | - | - |
| apple.in_new | 37.828197 s | 2877 | 100951 | 
| apple.in_old | 40.042296 s | 3116 | 103117 | 
| beaver.in | 11.172757 s | 219 | 1798 |
| box.in | 20.647474 s | 6236 | 101037 | 
| explosion.in | 47.628453 s | 1310 | 42918 | 
| koch.in | - | - | - | 
| mit.in | - | – | - | 
| sin_wave.in | - | - | - | 
| smalllines.in | 36.266379s | 23895 | 338592 | 

Unfortunately some of these animations took way too long to run at 4000 frames. A short test can just use some of the quick animations, to test if there is a priori any mistake at all.

## Precomputed line lengths
A simple innovation is to precompute the line lengths and store them in the `Line` struct. In the `CollisionWorld_collisionSolver` the line lengths are used as masses for the computation of kinetic energy. Here, we can use a precomputation. If we also remove the computation of color from the script, we could also achieve a speedup. However, none of these ideas seem to measurably increase the speed of the script.

## Bounding boxes
Before computing the line intersections, we should compute whether the bounding boxes of the two lines and their parallellograms overlap. If not, then they are too far away.

## The QuadTree
We implement the quadtree as a doubly linked data structure, where each node in the tree points to its parent and (if allocated) an array of children. When populating the tree with lines, we first need to fit lines into the existing tree structure. When that buffer becomes too large, we need to create the children and redistribute the lines across the original tree (if that line is too large for one of the children) and the children. 

Checking collisions starts at the tree root, where every line is compared to the other one. Then, we check the lines in each of the children, and each of those is compared to its parents as well. Hence, we can check all lines in the world.

We need to better understand what is going wrong: what is the slowest part of our program? Let's take a look at pprof:

    (base) MacBook-Pro-van-Simon:screensaver SimonAertssen$ CPUPROFILE=screensaver.prof ./screensaver 1000
    Input file path is: input/mit.in
    ---- RESULTS ----
    Elapsed execution time: 10.981296s
    170 Line-Wall Collisions
    2097 Line-Line Collisions
    ---- END RESULTS ----
    PROFILE: interrupts/evictions/bytes = 843/45/50824
    (base) MacBook-Pro-van-Simon:screensaver SimonAertssen$ pprof --text screensaver screensaver.prof
    Using local file screensaver.
    Using local file screensaver.prof.
    Total: 843 samples
     400  47.4%  47.4%      408  48.4% _Vec_makeFromLine
     284  33.7%  81.1%      728  86.4% _intersect
      51   6.0%  87.2%      838  99.4% _detect_collisions
      41   4.9%  92.1%       41   4.9% _Vec_add
      25   3.0%  95.0%       25   3.0% _Vec_subtract
      23   2.7%  97.7%       23   2.7% _Vec_multiply
      15   1.8%  99.5%       15   1.8% _intersectLines
       3   0.4%  99.9%        4   0.5% _insert_line
       1   0.1% 100.0%        1   0.1% 0x00007fff64425e24
       0   0.0% 100.0%        2   0.2% 0x0000000100006119
       ...
       ProfileHandler::SignalHandler
       0   0.0% 100.0%      843 100.0% _CollisionWorld_detectIntersection
       0   0.0% 100.0%      843 100.0% _CollisionWorld_updateLines
       0   0.0% 100.0%      843 100.0% _LineDemo_update
       0   0.0% 100.0%      843 100.0% __mh_execute_header
       0   0.0% 100.0%      843 100.0% _main

So it's clear that the collision detection should be made faster!