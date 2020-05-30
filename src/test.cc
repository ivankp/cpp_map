#include <iostream>
#include <string_view>
#include <vector>
#include <string>
#include <iterator>
#include <cstring>
#include <cstdio>

#include "containers/containers.hh"

#define TEST(var) \
  std::cout << "\033[36m" #var "\033[0m = " << (var) << std::endl;

using std::cout;
using std::endl;

using namespace ivanp::containers;
using namespace ivanp::containers::operators;

template <List T>
auto my_list_size(const T& x) {
  TEST(__FUNCTION__)
  return std::size(x);
}

template <Tuple T>
auto my_tuple_size(const T&) {
  TEST(__FUNCTION__)
  return std::tuple_size<T>::value;
}

template <typename T>
auto my_size(const T& x) requires List<T> || Tuple<T> {
  if constexpr (Tuple<T>) return my_tuple_size(x);
  if constexpr (List <T>) return my_list_size(x);
}

int main(int argc, char* argv[]) {
  std::cout << std::boolalpha;

  std::array<int,5> int_arr;

  TEST(my_list_size(int_arr))
  TEST(my_tuple_size(int_arr))
  TEST(my_size(int_arr))

  int c_arr[4];
  TEST(my_size(c_arr))
  TEST(my_size(std::initializer_list{1,2,3}))

  std::vector vec { "vector", "test" };

  // TEST(ConstSizable<decltype(int_arr)>)
  // TEST(ConstSizable<decltype(c_arr)>)
  // TEST(ConstSizable<decltype(std::initializer_list{1,2,3})>)
  // TEST(ConstSizable<decltype(vec)>)

  // [](int (&a)[]) { TEST(std::size(a)); }(c_arr);

  const char* cstrs [] { "char*", "array" };
  // cstrs | printf; // doesn't work
  map(printf,cstrs);

  vec | printf;
  cout << '\n';
  // auto t1 = // test void return
  std::tuple(1,'a',"bc") | [](const auto& x) {
    cout << ' ' << x;
    // if constexpr (
    //   std::is_same_v<std::decay_t<decltype(x)>,int> ||
    //   std::is_same_v<std::decay_t<decltype(x)>,char> ||
    //   std::is_same_v<std::decay_t<decltype(x)>,const char*>
    // ) return x;
  };
  cout << '\n';

  for (auto x : map(strlen,{ "hello", "world" })) cout << ' ' << x;
  cout << '\n';

  for (auto x : std::vector<std::string> {
      "hello", "world"
    } | &std::string::size) cout << ' ' << x;
  cout << '\n';

  TEST(( \
    std::make_pair(vec.begin(),vec.end()) \
    % std::distance<decltype(vec.begin())> \
  ))

  ( std::tuple{"hello", "world"} | strlen | [](auto x) { return x+1; } )
  % [](auto a, auto b){ cout << a << " + " << b << " = " << (a+b) << '\n'; };

  auto show_type = [](const auto&) { cout << __PRETTY_FUNCTION__ << '\n'; };

  show_type(std::pair<std::string,std::string> {
    "hello", "world"
  } | &std::string::size);
  show_type(std::array<std::string,2> {
    "hello", "world"
  } | &std::string::size);
  show_type(std::pair<std::string,std::string_view> {
    "hello", "world"
  } | [](const auto& s){ return std::size(s); });
  show_type(std::pair<std::string,std::string_view> {
    "hello", "world"
  } | [](auto s) -> decltype(s) { return { s.data(), 1 }; });

  show_type(std::pair { 0, 1 }
    | [&](auto i) { return vec[i]; });
  show_type(std::pair { 0, 1 }
    | [&](auto i) -> decltype(auto) { return vec[i]; });

  show_type(std::pair { 0, 1 }
    || [&](auto i) { return vec[i]; });
  show_type(std::pair { 0, 1 }
    || [&](auto i) -> decltype(auto) { return vec[i]; });

  int a1[1] { };
  int a2[2] { };
  show_type(std::tuple{a1,a2});
  show_type(std::array{a1,a2});

  show_type(std::vector { 0, 1 } // tests vector element type
    | [&](auto i) -> decltype(auto) { return vec[i]; });
}
