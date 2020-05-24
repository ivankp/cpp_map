#include <iostream>
#include <string_view>
#include <vector>
#include <string>
#include <iterator>

#include "containers/containers.hh"

#define TEST(var) \
  std::cout << "\033[36m" #var "\033[0m = " << (var) << std::endl;

using std::cout;
using std::endl;

using namespace ivanp::containers;

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
  std::tuple(1,'a',"bc") | [](const auto& x) { cout << x << endl; };

  std::vector<std::string> {
    "hello", "world"
  } | &std::string::size;

  TEST(( \
    std::distance<decltype(vec.begin())> \
    % std::tuple{vec.begin(),vec.end()} \
  ))

  // std::array<std::string,2> {
  //   "hello", "world"
  // } | &std::string::size;
}
