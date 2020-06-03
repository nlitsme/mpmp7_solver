-include Makefile.local

CXXFLAGS+=-std=c++17 -g $(if $(D),-O0,-O3)

all:: mpmp7-unique-distances unittests

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $^

%: %.o
	$(CXX) $(LDFLAGS) -o $@ $^

clean::
	$(RM) mpmp7-unique-distances unittests
	$(RM) $(wildcard *.o)

