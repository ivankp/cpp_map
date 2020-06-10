#include <span>
#include <cstdio>

// #include "map/map.hh"
//
// using ivanp::map::map;
// using namespace ivanp::map::operators;

int main(int argc, char* argv[]) {
  for (auto arg : std::span(argv+1,argc-1))
    printf(arg);
}
