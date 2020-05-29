"""
Solve "MPMP: Unique Distancing Problem" by Matt Parker
See:  https://www.youtube.com/watch?v=M_YOCQaI5QI

Arrange N counters on an NxN grid, such that all the
distances between the counters are different.

For the 3x3 example there are 5 unique ways,
excluding rotations and reflections of the entire grid.

 ..*    ...    ...    *.*    .**
 **.    ..*    *.*    *..    ...
 ...    **.    *..    ...    *..


The MPMP puzzle:
can you find a way to do this on a 6x6 grid with 6 counters.

This python script solves this problem for many dimensional grids
as well.


Author: Willem Hengeveld <itsme@xs4all.nl>
"""
from itertools import combinations, permutations, product

# single letter variable naming convention:
#    p,q for points
#    a,b for arrangements
#    d   for distance


class Size:
    """
    Struct for keeping the dimension and width of the grid.
    """
    def __init__(self, dimension, width):
        self.dim = dimension
        self.width = width


def distance_squared(p, q):
    """
    Calculate the square of the distance between two points.
    Don't bother taking the square root, this will only slow things down.
    """
    return sum(((p-q)**2 for p, q in zip(p, q)))


def numpositions(size, ncounters):
    """
    Calculate all possible arrangements of `n` counters on grid of `size`.
    """
    total = 1
    choices = size.width**size.dim
    for i in range(ncounters):
        total *= choices
        total //= i+1
        choices -= 1
    return total


def generatepoints(size):
    """
    Generate all points on the grid of `size`.
    """
    for p in product(*[list(range(size.width))] * size.dim):
        yield p


def generatearrangements(size, ncounters):
    """
    Generate all possible arrangements of n counters on the grid of `size`.
    """
    return combinations(generatepoints(size), ncounters)


def hasuniquedistance(pieces):
    """
    Enumerate all pairs of counters and check if all distances are unique.
    """
    distances = set()
    for p, q in combinations(pieces, 2):
        d = distance_squared(p, q)
        if d in distances:
            return False
        distances.add(d)
    return True


def printarrangement(size, pieces):
    """
    Pretty print a given arrangement of counters on a grid.
    Only pretty print the 2 and 3 dimensional grids.
    """
    if size.dim == 2:
        for y in range(size.width):
            for x in range(size.width):
                if (x,y) in pieces:
                    print("*", end="")
                else:
                    print(".", end="")
            print()
    elif size.dim == 3:
        for y in range(size.width):
            for z in range(size.width):
                for x in range(size.width):
                    if (x,y,z) in pieces:
                        print("*", end="")
                    else:
                        print(".", end="")
                print("  ", end="")
            print()
    else:
        # don't bother pretty printing dimensions other than 2 or 3.
        print(pieces)


def rotatepoint(size, flip, perm, p):
    """
    Rotate and reflect a single point `p` according to `perm` and `flip`.
    """
    def bitgen(num):
        """ Yields all bits from the given number. """
        while True:
            yield num & 1
            num >>= 1

    def doflip(x, bit):
        """ reflect a single coordinate conditional on the given `bit` """
        return size.width-1-x if bit else x

    return tuple( doflip(p[i], bit) for bit, i in zip(bitgen(flip), perm) )


def rotatearragement(size, flip, perm, a):
    """
    Return the arragement `a`, rotated and reflected according to `flip` and `perm`.
    """
    return set(rotatepoint(size, flip, perm, p) for p in a)


def istransformof(size, a, b):
    """
    Checks if the arrangement `a` is a rotated or reflected transformation
    of arrangement `b`.

    This enumerates all n-dimensional rotations and reflections by
    permuting the coordinates ( 'perm' ), and enumerating all possible
    reflections ( 'flip' ).

    """
    for flip in range(2**size.dim):
        for perm in permutations(range(size.dim)):
            if rotatearragement(size, flip, perm, a) == b:
                return True


def containstransform(size, solutions, a):
    """
    Check if our `solutions` list already contains solution `a`
    in a rotated or reflected transformation.
    """
    for b in solutions:
        if istransformof(size, a, b):
            return True
    return False


def solvegrid(args, size, ncounters):
    """
    Generate and print all solutions for a `size` grid with `ncounters` counters.
    """
    solutions = []

    for pieces in generatearrangements(size, ncounters):
        pieces = set(pieces)
        if hasuniquedistance(pieces):
            if not containstransform(size, solutions, pieces):
                solutions.append(pieces)
                if args.verbose:
                    print("-----")
                    printarrangement(size, pieces)
                    print()
    return solutions


def main():
    import argparse
    parser = argparse.ArgumentParser(description='find unique-distance solutions.')
    parser.add_argument('--width', '-w', default=3, type=int)
    parser.add_argument('--counters', '-n', type=int)
    parser.add_argument('--dimension', '-d', default=2, type=int)
    parser.add_argument('--verbose', '-v', action='store_true')
    args = parser.parse_args()

    if args.counters is None:
        args.counters = args.width

    d = Size(args.dimension, args.width)
    solutions = solvegrid(args, d, args.counters)

    print("Found %d solutions in %d total arrangements." % (len(solutions), numpositions(d, args.counters)))


if __name__ == '__main__':
    main()
