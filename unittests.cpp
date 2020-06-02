#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "contrib/doctest.h"
#define SECTION(...) SUBCASE(__VA_ARGS__)
#define SKIPTEST  * doctest::skip(true)
#define CHECK_THAT(a, b) 

#define NOMAIN 1
#include "mpmp7-unique-distances.cpp"

TEST_CASE("math") {
    CHECK(pow(2,8) == 256);
    CHECK(pow(3,4) == 81);
    CHECK(square(3) == 9);
}

TEST_CASE("fixedset") {
    for (int j = 1 ; j < 256 ; j++)
        for (int n = 0 ; n < 2 ; n++)
        {
            int setmax = j<128 ? j : 256-j;

            FixedSet s(setmax);
            for (int i=0 ; i < setmax ; i++) {
                CHECK( s.add(i) );
                CHECK( !s.add(i) );
            }
            for (int i=0 ; i < setmax ; i++)
                CHECK( !s.add(i) );
        }
}
TEST_CASE("point") {

    CHECK( make<Point>(1,1) == make<Point>(1,1) );
    CHECK( make<Point>(1,1) < make<Point>(1,2) );
    CHECK( make<Point>(1,2) > make<Point>(1,1) );
    CHECK( make<Point>(1,1) < make<Point>(2,1) );
    CHECK( make<Point>(2,1) > make<Point>(1,1) );
    CHECK( make<Point>(1,2) < make<Point>(2,1) );
    CHECK( make<Point>(2,1) > make<Point>(1,2) );

    CHECK( make<Point>(1,2) != make<Point>(2,1) );
    CHECK_FALSE( make<Point>(1,2) == make<Point>(2,1) );
    
    CHECK( make<Point>(2,2) == make<Point>(2,2) );
    CHECK_FALSE( make<Point>(2,2) != make<Point>(2,2) );

    CHECK( make<Point>(2) == make<Point>(2) );
    CHECK( make<Point>(1) < make<Point>(2) );
    CHECK( make<Point>(2) > make<Point>(1) );
    CHECK( make<Point>() == make<Point>() );
    //CHECK( make<Point>(9, 9) < make<Point>(3, 3, 3) );
    //
    CHECK( dist2(make<Point>(3,4), make<Point>(0,0)) == 25 );
    CHECK( dist2(make<Point>(3,4,0), make<Point>(0,0,0)) == 25 );
    CHECK( dist2(make<Point>(0,3,4), make<Point>(0,0,0)) == 25 );
}
TEST_CASE("arrangement") {
    CHECK( Arrangement::make() == Arrangement::make() );
    CHECK( Arrangement::make(make<Point>(1,1)) == Arrangement::make(make<Point>(1,1)) );
    CHECK_FALSE( Arrangement::make(make<Point>(1,1)) == Arrangement::make(make<Point>(0,0)) );
    CHECK( Arrangement::make(make<Point>(1,1,2)) == Arrangement::make(make<Point>(1,1,2)) );
    CHECK_FALSE( Arrangement::make(make<Point>(1,1,2)) == Arrangement::make(make<Point>(1,2,1)) );
    CHECK( Arrangement::make(make<Point>(1,1,2), make<Point>(1,2,0)) == Arrangement::make(make<Point>(1,1,2), make<Point>(1,2,0)) );
    CHECK( Arrangement::make(make<Point>(1,1,2), make<Point>(1,2,0)).contains(make<Point>(1,2,0))  );
    CHECK( Arrangement::make(make<Point>(1,1,2), make<Point>(1,2,0)).contains(make<Point>(1,1,2))  );
    CHECK( !Arrangement::make(make<Point>(1,1,2), make<Point>(1,2,0)).contains(make<Point>(0,0,0))  );

    // order of points should not matter
    CHECK( Arrangement::make( make<Point>(0, 0),  make<Point>(0, 1),  make<Point>(1, 2) ) == Arrangement::make( make<Point>(0, 1),  make<Point>(0, 0),  make<Point>(1, 2) ) );
    CHECK( Arrangement::make( make<Point>(0, 1),  make<Point>(0, 0),  make<Point>(1, 2) ) == Arrangement::make(  make<Point>(1, 2), make<Point>(0, 1),  make<Point>(0, 0) ) );
}
TEST_CASE("generate") {
    int i = 0;
    for (auto a : generatearrangements(Size(4, 3), 2))
        i++;
    CHECK( i == generatearrangements::totalarrangements(Size(4, 3), 2) );
}

TEST_CASE("uniquedist")
{
    CHECK( hasuniquedistance(Size(2, 3), Arrangement::make()) );
    CHECK( hasuniquedistance(Size(7, 7), Arrangement::make()) );
    CHECK( hasuniquedistance(Size(4, 4), Arrangement::make(make<Point>(1,1,1,1))) );
    CHECK( hasuniquedistance(Size(0, 0), Arrangement::make(make<Point>())) );

    CHECK( hasuniquedistance(Size(2, 3), Arrangement::make(make<Point>(0,0), make<Point>(0,1))) );
    CHECK( hasuniquedistance(Size(2, 3), Arrangement::make(make<Point>(0,0), make<Point>(0,0))) );
    CHECK( hasuniquedistance(Size(2, 3), Arrangement::make(make<Point>(1,1), make<Point>(0,0))) );


    CHECK_FALSE( hasuniquedistance(Size(2, 3), Arrangement::make(make<Point>(0,0), make<Point>(0,1), make<Point>(1,0))) );
    CHECK_FALSE( hasuniquedistance(Size(2, 3), Arrangement::make(make<Point>(0,0), make<Point>(0,1), make<Point>(1,1))) );
    CHECK( hasuniquedistance(Size(2, 3), Arrangement::make(make<Point>(0,0), make<Point>(0,1), make<Point>(2,0))) );
    CHECK( hasuniquedistance(Size(2, 3), Arrangement::make(make<Point>(0,0), make<Point>(0,1), make<Point>(2,1))) );
    CHECK( hasuniquedistance(Size(2, 3), Arrangement::make(make<Point>(0,0), make<Point>(0,1), make<Point>(2,2))) );
}
TEST_CASE("rotatepoint")
{
    CHECK( rotatepoint(Size(2, 3), 0, make<Permutation>(0, 1), make<Point>(1, 2)) == make<Point>(1, 2) );
    CHECK( rotatepoint(Size(2, 3), 0, make<Permutation>(1, 0), make<Point>(1, 2)) == make<Point>(2, 1) );
    CHECK( rotatepoint(Size(2, 3), 3, make<Permutation>(1, 0), make<Point>(1, 2)) == make<Point>(0, 1) );
    CHECK( rotatepoint(Size(2, 3), 3, make<Permutation>(0, 1), make<Point>(1, 2)) == make<Point>(1, 0) );

    CHECK( rotatepoint(Size(2, 4), 0, make<Permutation>(0, 1), make<Point>(1, 2)) == make<Point>(1, 2) );
    CHECK( rotatepoint(Size(2, 4), 0, make<Permutation>(1, 0), make<Point>(1, 2)) == make<Point>(2, 1) );
    CHECK( rotatepoint(Size(2, 4), 3, make<Permutation>(1, 0), make<Point>(1, 2)) == make<Point>(1, 2) );
    CHECK( rotatepoint(Size(2, 4), 3, make<Permutation>(0, 1), make<Point>(1, 2)) == make<Point>(2, 1) );
}
TEST_CASE("rotatearrangement")
{
    auto a = Arrangement::make( make<Point>(0, 0),  make<Point>(0, 1),  make<Point>(1, 2) );

    CHECK( rotatearrangement(Size(2,4), 0, make<Permutation>(0, 1), a) == a );
    CHECK( rotatearrangement(Size(2,4), 0, make<Permutation>(1, 0), a) == Arrangement::make( make<Point>(0, 0),  make<Point>(1, 0),  make<Point>(2, 1) ) );
    CHECK( rotatearrangement(Size(2,4), 3, make<Permutation>(1, 0), a) == Arrangement::make( make<Point>(3, 3),  make<Point>(2, 3),  make<Point>(1, 2) ) );


    CHECK( rotatearrangement(Size(2,3), 0, make<Permutation>(0, 1), a) == Arrangement::make( make<Point>(0, 0),  make<Point>(0, 1),  make<Point>(1, 2) ) );
    CHECK( rotatearrangement(Size(2,3), 0, make<Permutation>(1, 0), a) == Arrangement::make( make<Point>(0, 0),  make<Point>(1, 0),  make<Point>(2, 1) ) );
    CHECK( rotatearrangement(Size(2,3), 3, make<Permutation>(1, 0), a) == Arrangement::make( make<Point>(2, 2),  make<Point>(1, 2),  make<Point>(0, 1) ) );
}
TEST_CASE("istransform")
{
    CHECK( istransformof(Size(4,3), Arrangement::make(make<Point>(0,0,0,0),  make<Point>(1,1,1,1)), Arrangement::make(make<Point>(2,2,2,2),  make<Point>(1,1,1,1))) );
}
TEST_CASE("contains")
{
    CHECK( containstransform(Size(4,3), { Arrangement::make(make<Point>(0,0,0,0),  make<Point>(1,1,1,1)) }, Arrangement::make(make<Point>(2,2,2,2),  make<Point>(1,1,1,1))) );
    CHECK( containstransform(Size(4,3), { Arrangement::make(make<Point>(2,2,2,2),  make<Point>(1,1,1,1)) }, Arrangement::make(make<Point>(2,2,2,2),  make<Point>(1,1,1,1))) );
    CHECK_FALSE( containstransform(Size(4,3), {  }, Arrangement::make(make<Point>(2,2,2,2),  make<Point>(1,1,1,1))) );
    CHECK_FALSE( containstransform(Size(4,3), { Arrangement::make(make<Point>(2,2,2,2),  make<Point>(1,1,1,0)) }, Arrangement::make(make<Point>(2,2,2,2),  make<Point>(1,1,1,1))) );
    CHECK( containstransform(Size(4,3), { Arrangement::make(make<Point>(2,2,2,2),  make<Point>(1,1,1,0)),  Arrangement::make(make<Point>(0,0,0,0),  make<Point>(1,1,1,1)) }, Arrangement::make(make<Point>(2,2,2,2),  make<Point>(1,1,1,1))) );
    CHECK( containstransform(Size(4,3), { Arrangement::make(make<Point>(0,0,0,0),  make<Point>(1,1,1,1)), Arrangement::make(make<Point>(2,2,2,2),  make<Point>(1,1,1,0))  }, Arrangement::make(make<Point>(2,2,2,2),  make<Point>(1,1,1,1))) );
}
