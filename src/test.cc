#include <iostream>
#include <string_view>
#include <vector>
#include <string>
#include <iterator>
#include <cstring>

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

  std::vector vec { 7,8,9 };

  // TEST(ConstSizable<decltype(int_arr)>)
  // TEST(ConstSizable<decltype(c_arr)>)
  // TEST(ConstSizable<decltype(std::initializer_list{1,2,3})>)
  // TEST(ConstSizable<decltype(vec)>)

  // [](int (&a)[]) { TEST(std::size(a)); }(c_arr);

  vec | [](float i) { cout << i << endl; };
  // auto t1 = // test void return
  std::tuple(1,'a',"bc") | [](const auto& x) {
    cout << x << endl;
    // if constexpr (
    //   std::is_same_v<std::decay_t<decltype(x)>,int> ||
    //   std::is_same_v<std::decay_t<decltype(x)>,char> ||
    //   std::is_same_v<std::decay_t<decltype(x)>,const char*>
    // ) return x;
  };

  for (auto x : map({ "hello", "world" },strlen)) cout << ' ' << x;
  cout << '\n';

  for (auto x : std::vector<std::string> {
      "hello", "world"
    } | &std::string::size) cout << ' ' << x;
  cout << '\n';

  TEST(( \
    std::tuple{vec.begin(),vec.end()} \
    % std::distance<decltype(vec.begin())> \
  ))

  map(std::tuple{"hello", "world"},strlen)
  % [](auto a, auto b){ cout << a << " + " << b << " = " << (a+b) << endl; };

  // std::array<std::string,2> {
  //   "hello", "world"
  // } | &std::string::size;
}
