# Solver for n-dimensional variants of the MPMP7 Unique Distancing problem.

Project with tools for the 7th [Matt Parker Math Puzzle problem](http://think-maths.co.uk/uniquedistance).

 * Solution for the problem stated in the MPMP7 youtube video.
 * Solutions for smaller and larger grids.
 * Solutions in 3 or more dimensional grids
 * Can I fit more markers on the grid?
 * Can I solve this on an 8x8 grid?
 * Do solutions exist for larger 2D grids?

# The problem

Arrange N counters on an NxN grid, such that all the
distances between the counters are different.

# First a solution to the problem stated in the MPMP7 youtube video:

[The video](https://www.youtube.com/watch?v=M_YOCQaI5QI)

    python3 mpmp7_unique_distances.py --width 6 --verbose

This will output these two solutions.

    *.....
    *.....
    .....*
    .*....
    ......
    ...*.*

or

    *.....
    ......
    *.....
    ...*..
    ....**
    *.....

The script will not terminate immediately, since it will keep searching for more solutions, 
which it will not find.

I also wrote a C++ program to do the same, but much faster:

    ./mpmp7-unique-distances -p 6


# Solutions for smaller and larger grids.

First, as stated by Matt in his video, for the 3x3 case there are 5 solutions:

    ..*    ...    .**    ...    *.*
    **.    ..*    ...    *.*    *..
    ...    **.    *..    *..    ...

Though you could argue that the first two, and also the last two look identical,
with respect to translation. I did not look into counting those as duplicates.


Then the 2x2 grid, this one is pretty obvious, these are the two solutions:

    *.   *.
    *.   .*

The simplest grid, being the 1x1 grid has 1 solution:

    *

Or is that the simplest?  what about a 0x0 grid:


Well, I am not sure if that counts as 0 or 1 solution.


Here is a table listing the number of solutions for the remaining grid sizes:

| size  |  solution count    |
| -----:| ------------------:|
|    0  |     0 or 1         |
|    1  |       1            |
|    2  |       2            |
|    3  |       5            |
|    4  |      23            |
|    5  |      35            |
|    6  |       2            |
|    7  |       1            |
|    8  |       0            |

Here is the solution for the 7x7 grid:

    *..*...
    .......
    **.....
    .......
    .......
    .....*.
    ..*...*



# Solutions in 3 or more dimensional grids

Now Why stop at flat grids, you can solve this for 3-D 'grids' as well:


| size  |  3D solution count    |
| -----:| ------------------:|
|    2  |            3 |
|    3  |           50 |
|    4  |         3983 |
|    5  |       >=1185 |
|    6  |              |
|    7  |       >=3446 |

Then in 4-D I was only able to solve this for 2x2x2x2 and 3x3x3x3 grids.
The following table list the number of solutions found for the various higher
dimensional grids:

| size  |    4D     |       5D  |       6D  |       7D  |
| -----:| ---------:| ---------:| ---------:| ---------:|
|    2  |         4 |       5   |       6   |       7   |
|    3  |       261 |    >=255  |    >= 37  |    >=16   |
|    4  |     >=766 |    >=81   |           |           |

in 5-D there are 5 solutions for the 2-sized grid. and more than 800 for the 3-sized grid,
I did not wait for my program to complete it's search.



# Can I fit more markers on the grid?

For the 3D and higher dimensional grids you can,
here is a table of the most markers you can fit for a given size and dimension:

| size/dim  |    2   |      3     |    4     |   5  |   6   |   7   |
|  ----:|   --------:| ----------:| --------:| ----:| -----:| -----:|    
| 2     |        2   |      3     |    3     |  3   |  4    |  4    |
| 3     |        3   |      4     |    5     | >=6  |  >=6  |  4    |
| 4     |        4   |      6     |   >=7    | >=4  |       |       |
| 5     |        5   |     >=7    |          |      |       |       |
| 6     |        6   |     >=8    |          |      |       |       |
| 7     |        7   |     >=7    |          |      |       |       |


The empty slots in the table were computationally too expensive to determine.


# Can I solve this on an 8x8 grid?

Not with 8 markers, but there are 927 ways you can solve this with 7 markers.

Here is one solution:

    *......*
    *.......
    ........
    *.......
    .*......
    .......*
    ..*.....
    ........


# Do solutions exist for larger 2D grids?

With less markers you can solve this ( obviously ).
Here is a table listing the maximum number of markers you can fit on 2D grids:

| grid size   |  max counters |
|  -----:|  -----------:|
|  2     |    2      |
|  3     |    3      |
|  4     |    4      |
|  5     |    5      |
|  6     |    6      |
|  7     |    7      |
|  8     |    7      |
|  9     |   >=8     |
| 10     |   >=8     |
| 11     |   >=9     |
| 12     |   >=9     |
| 13     |   >=8     |
| 14     |   >=8     |
| 15     |   >=8     |


# Build the C++ project

Simple really: just pass the c++14 flag, there are no other dependencies.

    clang++ -O3 -std=c++14 mpmp7-unique-distances.cpp -o mpmp7-unique-distances


# BUGS ( that may never be fixed )

 * parameters  2 7 2  take really long --> most time spent rotating arrangements.
 * most solutions are found in the first 10% of the running time --> maybe i am generating way too many arrangements,
   and i could stop searching way earlier.
 * some, like 6 3 6,  make a very wrong 'approxpersecond' estimate.
 * Everything i wrote in the document might be wrong.

# AUTHOR

Willem Hengeveld <itsme@xs4all.nl>

