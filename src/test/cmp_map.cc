#include <span>
#include <cstdio>

#include "map/map.hh"

using ivanp::map::map;
using namespace ivanp::map::operators;
using flags = ivanp::map::flags;

int main(int argc, char* argv[]) {
  std::span(argv+1,argc-1) | [](auto x){ printf(x); };

  /*
  map<
    // flags::no_dynamic_size_check
    // flags::prefer_iteration
    flags::no_return
  > (
    // [](auto... x){ printf(x...); },
    // std::array{"%s","%s","%s"},
    // std::span(argv+1,argc-1)

    // [](auto x){ printf(x); },
    printf,
    std::span(argv+1,argc-1)
  );
  */

  // return std::get<1>(std::tuple{1,2,3} | [](auto x){ return x; });
}
