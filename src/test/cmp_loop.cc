#include <span>
#include <cstdio>

// #include "ivanp/map/map.hh"
//
// using ivanp::map::map;
// using namespace ivanp::map::operators;

int main(int argc, char* argv[]) {
  for (auto arg : std::span(argv+1,argc-1))
    printf(arg);

  // for (int i=1; i<argc; ++i)
  //   printf(argv[i]);

  // for (auto arg=argv+1, end=argv+argc; arg!=end; ++arg)
  //   printf(*arg);
}
