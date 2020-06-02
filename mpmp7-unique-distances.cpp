/*

This program solves the MPMP problem described below
for multi-dimensional NxNx..xN grids.

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


This program is a much faster variant of my python program

    mpmp_ndimensional_unique_distance.py

Author: Willem Hengeveld <itsme@xs4all.nl>
*/


#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <time.h>

#define MAXDIM 8
#define MAXCOUNTERS 10
#define MAXSETSIZE (1024*1024)


/*
 * Some simple integer arithmetic functions.
 */
uint64_t pow(int a, int b)
{
    uint64_t result = 1;
    if (a==0)
        return 0;
    while (b--)
        result *= a;
    return result;
}
int square(int x) { return x*x; }



/*
 *  `FixedSet` replaces std::set but is much more efficient, since
 *  it does no memory allocations.
 */
struct FixedSet {
    unsigned int bits[MAXSETSIZE];
    FixedSet(int nmax)
    {
        memset(bits, 0, sizeof(int)*(nmax/(8*sizeof(int))+1));
    }
    static int maxsize() { return MAXSETSIZE*sizeof(int); }
    bool add(int value)
    {
        int shift = value % (8*sizeof(int));
        int index = value / (8*sizeof(int));
        if (bits[index] & (1<<shift))
            return false;
        bits[index] |= 1<<shift;
        return true;
    }
};


/*
 * Holds the parameters for the current grid:
 */
struct Size {
    int dim;    // the number of spatial dimensions
    int width;  // the width in one direction.
    Size() : dim(0), width(0) { }
    Size(int dim, int width) : dim(dim), width(width) { }

    // a method for outputting a Size object.
    friend std::ostream& operator<<(std::ostream& os, const Size& size)
    {
        return os << "<" << size.dim << ":" << size.width << ">";
    }
};


/*
 * Set of templates for convenient construction of Point and Permutation objects:
 *    make<Point>(1, 2, 3) 
 */
template<typename POINT, int ix, typename T, typename...ARGS>
static void setcoord(POINT& p, T x, ARGS...args)
{
    p[ix] = x;
    if constexpr (sizeof...(ARGS)>0)
        setcoord<POINT, ix+1>(p, args...);
}
template<typename POINT, typename...ARGS>
auto make(ARGS...args)
{
    POINT p(sizeof...(ARGS));
    if constexpr (sizeof...(ARGS)>0)
        setcoord<POINT, 0>(p, args...);
    return p;
}

/*
 * a Point keeps the coordinates to a single point in the grid.
 */
struct Point {
    int x[MAXDIM];
    int n;        // the number of spatial dimensions of this point.

    Point() : n(0) { }
    Point(int n)
        : n(n)
    {
    }
    Point(const Point& p)
    {
        n = p.n;
        memcpy(x, p.x, n*sizeof(int));
    }

    // index access to the point's coordinates.
    int& operator[](int i) { return x[i]; }
    int operator[](int i) const { return x[i]; }

    // begin, end for iterating of the coordinates of the point.
    int *begin() { return &x[0]; }
    int *end() { return &x[0]+n; }

    // a method for outputting a Point.
    friend std::ostream& operator<<(std::ostream& os, const Point& p)
    {
        os << '(';
        for (int i=0 ; i<p.n ; i++) {
            if (i) os << ',';
            os << p[i];
        }
        os << ')';
        return os;
    }

    // various ways of comparing points.

    friend int compare(const Point& p, const Point& q)
    {
        int i = 0;
        while (i<p.n && p[i]==q[i])
            ++i;

        if (i == p.n) return 0;
        if (p[i] < q[i]) return -1;
        return 1;
    }
    friend bool operator<(const Point& p, const Point& q)
    {
        return compare(p, q) < 0;
    }
    friend bool operator>(const Point& p, const Point& q)
    {
        return compare(p, q) > 0;
    }
    friend bool operator==(const Point& p, const Point& q)
    {
        return compare(p, q) == 0;
    }
    friend bool operator!=(const Point& p, const Point& q)
    {
        return compare(p, q) != 0;
    }

    /*
     * Calculate the square of the distance between two points.
     */
    friend int dist2(const Point& p, const Point& q)
    {
        int total = 0;
        for (int i=0 ; i<p.n ; i++) {
            total += square(p[i]-q[i]);
        }
        return total;
    }
};


/*
 * An Arrangement of Counters, is a collection of points.
 */
struct Arrangement {
    Point counters[MAXCOUNTERS];
    int n;        // the number of counters in this arrangement.
    Arrangement() : n(0) { }

    template<typename T, typename...ARGS>
    static void addpoints(Arrangement& a, T x, ARGS...args)
    {
        a.add(x);
        if constexpr (sizeof...(ARGS)>0)
            addpoints(a, args...);
    }

    template<typename...ARGS>
    static auto make(ARGS...args)
    {
        Arrangement a;
        if constexpr (sizeof...(ARGS)>0)
            addpoints(a, args...);
        return a;
    }

    // add a point to the Arrangement, keeping the points sorted.
    void add(const Point& p)
    {
        auto i = std::upper_bound(begin(), end(), p);
        memmove(i+1, i, (end()-i)*sizeof(Point));
        *i = p;
        n++;
    }

    // check if this point is in this arrangement.
    bool contains(const Point& p) const
    {
        for (int i=0 ; i<n ; i++)
            if (counters[i]==p)
                return true;
        return false;
    }

    // index access.
    Point& operator[](int i) { return counters[i]; }
    const Point& operator[](int i) const { return counters[i]; }

    // iterator access.
    Point*begin() { return &counters[0]; }
    Point*end() { return &counters[0]+n; }

    const Point*begin() const { return &counters[0]; }
    const Point*end() const { return &counters[0]+n; }

    // a method for outputting an arrangement.
    friend std::ostream& operator<<(std::ostream& os, const Arrangement& a)
    {
        os << '{';
        for (int i=0 ; i<a.n ; i++) {
            if (i) os << ", ";
            os << a[i];
        }
        os << '}';
        return os;
    }

    friend bool operator==(const Arrangement& a,const Arrangement& b)
    {
        for (int i=0 ; i<a.n ; i++)
            if (a[i]!=b[i])
                return false;
        return true;
    }  
};


/*
 * Generate all possible Arrangementsof 'n' points on the grid in lexicographical order.
 */
struct generatearrangements {
    struct iter {
        Size size;
        int ncounters;       // the number of counter to place on the grid.
        int totalpositions;  // the number of positions a counter can be in the grid.
        uint64_t ix;         // index of the current arrangement

        std::vector<int> c;

        iter(Size size, int ncounters, uint64_t ix)
            : size(size), ncounters(ncounters), totalpositions(pow(size.width, size.dim)), ix(ix)
        {
            c.resize(ncounters);
            for (int i=0 ; i < ncounters ; i++)
                c[i] = i;
        }
        Arrangement operator*()
        {
            Arrangement a;
            for (int i = 0 ; i < ncounters ; i++)
                a.add(makepoint(c[i]));
            return a;
        }
        Point makepoint(int encodedpoint)
        {
            Point p(size.dim);
            for (int i=0 ; i < size.dim ; i++) {
                p[size.dim-1-i] = encodedpoint % size.width;
                encodedpoint /= size.width;
            }
            return p;
        }
        iter& operator++()
        {
            // algorithm from https://stackoverflow.com/questions/9430568/generating-combinations-in-c
            auto last = c.end();
            auto i = last;

            if (c[0] == totalpositions-ncounters) {
                ++ix;
                return *this;
            }
            while (*(--i) == totalpositions-(last-i));
            if (i >= c.begin()) {
                (*i)++;
                while (++i != last) *i = *(i-1)+1;
            }

            ix++;
            return *this;
        }
        void state(std::ostream& os)
        {
            for (int i=0 ; i<c.size() ; i++)
            {
                if (i) os << ",";
                os << c[i];
            }
        }
        bool operator!=(const iter& rhs)
        {
            return ix!=rhs.ix;
        }
    };

    Size size; int ncounters;
    generatearrangements(Size size, int ncounters)
        : size(size), ncounters(ncounters)
    {
    }
    static uint64_t totalarrangements(Size size, int ncounters)
    {
        if (size.width==0)
            return 0;
        uint64_t a = 1;
        uint64_t b = pow(size.width, size.dim);
        for (int i = 0 ; i < ncounters ; i++) {
            a *= b;
            a /= i+1;
            b -= 1;
        }
        return a;
    }
    auto begin() { return iter(size, ncounters, 0); }
    auto end() { return iter(size, ncounters, totalarrangements(size, ncounters)); }
};


/*
 * Check if this Arrangement satisfies the 'unique-distance' requirement.
 */
bool hasuniquedistance(Size size, const Arrangement& a)
{
    FixedSet distances(pow(size.width-1, size.dim)*size.dim);
    for (auto i = a.begin() ; i != a.end() ; ++i)
    {
        for (auto j = i+1; j != a.end() ; ++j)
        {
            int d = dist2(*i, *j);
            if (!distances.add(d))
                return false;
        }
    }
    return true;
}


/*
 * Output an arrangement in a possibly readable way.
 */
void printarrangement(Size size, const Arrangement& a)
{
    if (size.dim == 2) {
        for (int y = 0 ; y < size.width ; y++) {
            for (int x = 0 ; x < size.width ; x++)
                std::cout << (a.contains(make<Point>(x, y)) ? '*' : '.');
            std::cout << "\n";
        }
        std::cout << "\n";
    }
    else if (size.dim == 3) {
        for (int y = 0 ; y < size.width ; y++) {
            for (int z = 0 ; z < size.width ; z++) {
                for (int x = 0 ; x < size.width ; x++)
                    std::cout << (a.contains(make<Point>(x, y, z)) ? '*' : '.');
                std::cout << "  ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
    else {
        std::cout << a << "\n";
    }
}


/*
 *  Reusing the Point object to represent a permutation.
 */
struct Permutation {
    uint8_t x[MAXDIM];
    int n;
    Permutation(int n)
        : n(n)
    {
        for (int i=0 ; i<8 ; i++)
            x[i] = i;
    }
    uint8_t operator[](int i) const { return x[i]; }
    uint8_t& operator[](int i) { return x[i]; }

    bool next()
    {
        return std::next_permutation(x, x+n);
    }
};
/*
 * Rotate and reflect a single point `p` according to `perm` and `flip`.
 */
Point rotatepoint(Size size, int flip, const Permutation& perm, const Point& p)
{
    Point q(size.dim);
    for (int i=0 ; i<size.dim ; i++) {
        int bit = flip&1;
        flip >>= 1;

        if (bit)
            q[i] = size.width-1-p[perm[i]];
        else
            q[i] = p[perm[i]];
    }
    return q;
}

/*
 * Return the arrangement `a`, rotated and reflected according to `flip` and `perm`.
 */
Arrangement rotatearrangement(Size size, int flip, const Permutation& perm, const Arrangement& a)
{
    Arrangement b;
    for (auto & p : a)
        b.add(rotatepoint(size, flip, perm, p));
    return b;
}


/*
 *  Checks if the arrangement `a` is a rotated or reflected transformation
 *  of arrangement `b`.
 *
 *  This enumerates all n-dimensional rotations and reflections by
 *  permuting the coordinates ( 'perm' ), and enumerating all possible
 *  reflections ( 'flip' ).
 */
bool istransformof(Size size, const Arrangement& a, const Arrangement& b)
{
    int nrreflections = 1<<size.dim;

    Permutation perm(size.dim);

    for (int flip = 0 ; flip<nrreflections ; flip++)
    {
        do {
            if (rotatearrangement(size, flip, perm, a) == b)
                return true;
        } while (perm.next());
    }
    return false;
}


/*
 *  Check if our `solutions` list already contains solution `a`
 *  in a rotated or reflected transformation.
 */
bool containstransform(Size size, const std::vector<Arrangement>& solutions, const Arrangement& a)
{
    for (auto& b : solutions)
        if (istransformof(size, a, b))
            return true;
    return false;
}

// returns the index of the matching solution, or size(solutions)
auto findprevious(Size size, const std::vector<Arrangement>& solutions, const Arrangement& a)
{
    int i = 0;
    for (auto& b : solutions) {
        if (istransformof(size, a, b))
            break;
        i++;
    }
    return i;
}

/*
 * Generate and print all solutions for a `size` grid with `ncounters` counters.
 */
void solvegrid(bool printall, int verbose, Size size, int ncounters)
{
    std::vector<Arrangement> solutions;
    uint64_t i = 0;
    uint64_t total = generatearrangements::totalarrangements(size, ncounters);

    time_t t0 = time(NULL);

    int approxpersecond = 0;

    for (auto a : generatearrangements(size, ncounters))
    {
        if (hasuniquedistance(size, a))
        {
            if (!containstransform(size, solutions, a)) {
                solutions.emplace_back(a);
                if (printall) {
                    std::cout << "-----\n";
                    printarrangement(size, a);
                }
            }
        }
        i++;

        if (verbose) {
            if (approxpersecond==0) {
                time_t t = time(NULL);
                if (t-t0 > 5) {
                    approxpersecond = i/(t-t0);
                    if (approxpersecond<10)
                        approxpersecond = 1;
                }

            }
            if (approxpersecond && (i%approxpersecond)==0) {
                time_t t = time(NULL);
                uint64_t apersec = t!=t0 ? i/(t-t0) : 0;
                uint64_t estimate = apersec ? (total-i) / apersec : 0;
                std::cout << "Tried " << i << " arrangements, " << apersec << " per second, found " << solutions.size() << " solutions, " << estimate << " seconds to go.\r";
                std::cout.flush();
            }
        }
    }
    time_t t = time(NULL);
    std::cout << "\n";
    std::cout << "Found " << solutions.size() << " solutions in " << generatearrangements::totalarrangements(size, ncounters) << " total arangements, in " << (t-t0) << " seconds.\n";
}

/*
 * Test arrangements
 */
void testgenerator(Size size, int ncounters)
{
    int j = 0;
    std::vector<Arrangement> solutions;
    for (auto a : generatearrangements(size, ncounters))
    {
        auto i = findprevious(size, solutions, a);
        if (i == solutions.size()) {
            // a new arrangement
            solutions.emplace_back(a);
        }
        std::cout << j << ": " << i << " " << a << "\n";

        j++;
    }
}
#ifndef NOMAIN
int main(int argc, char**argv)
{
    Size size;
    int ncounters = -1;
    size.width = 3;
    size.dim = 2;

    int verbose = 0;
    bool printall = false;
    bool dotest = false;

    while (argc>=2 && argv[1][0]=='-') {
        if (argv[1][1] == 'p') {
            printall = true;
            argv++; argc--;
        }
        else if (argv[1][1] == 'v') {
            verbose = strlen(argv[1])-1;
            argv++; argc--;
        }
        else if (argv[1][1] == 't') {
            dotest = true;
            argv++; argc--;
        }
        else {
            std::cout << "Usage: " << argv[0] << " [-p] [-v] [width [dimension [ncounters]]]\n";
            return 0;
        }
    }

    if (argc>=2)
        size.width = strtol(argv[1], 0, 0);

    if (argc>=3)
        size.dim = strtol(argv[2], 0, 0);
    if (argc>=4)
        ncounters = strtol(argv[3], 0, 0);
    if (ncounters==-1)
        ncounters = size.width;

    if (size.dim > MAXDIM) {
        std::cout << "max dimensions is: " << MAXDIM << "\n";
        return 1;
    }
    if (ncounters > MAXCOUNTERS) {
        std::cout << "max counters is: " << MAXCOUNTERS << "\n";
        return 1;
    }
    if ( pow(size.width-1, size.dim)*size.dim > FixedSet::maxsize()) {
        std::cout << "max set size is: " << FixedSet::maxsize() << "\n";
        return 1;
    }
    if ( size.dim * log(size.width) >= 31 * log(2) ) {
        std::cout << "WARNING: integer overflow may make this incorrect\n";
    }

    if (dotest)
        testgenerator(size, ncounters);
    else
        solvegrid(printall, verbose, size, ncounters);
}
#endif
