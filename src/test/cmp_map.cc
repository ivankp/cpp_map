#include <span>
#include <cstdio>

#include "map/map.hh"

using ivanp::map::map;
using namespace ivanp::map::operators;

int main(int argc, char* argv[]) {
  std::span(argv+1,argc-1) | [](auto x){ printf(x); };

  /*
  map<
    ivanp::map::flags::no_dynamic_size_check
    // ivanp::map::flags::prefer_iteration
  > (
    [](auto... x){ printf(x...); },
    std::array{"%s","%s","%s"},
    std::span(argv+1,argc-1)
  );
  */

  // return std::get<1>(std::tuple{1,2,3} | [](auto x){ return x; });
}
