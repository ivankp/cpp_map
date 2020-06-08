#ifndef IVANP_MAP_TRAITS_HH
#define IVANP_MAP_TRAITS_HH

#include <type_traits>
#include <utility>
#include <tuple>

namespace ivanp::map {

template <typename T>
struct type_constant { using type = T; };

template <typename...>
struct type_sequence { };

template <size_t I>
using index_constant = std::integral_constant<size_t,I>;

// https://ldionne.com/2015/11/29/efficient-parameter-pack-indexing/
template <size_t I, typename T, typename... Ts>
struct pack_element { using type = typename pack_element<I-1, Ts...>::type; };
template <typename T, typename... Ts>
struct pack_element<0, T, Ts...> { using type = T; };
template <size_t I, typename... Ts>
using pack_element_t = typename pack_element<I, Ts...>::type;

template <typename T, typename... Ts>
constexpr T&& head_value(T&& x, Ts&&...) noexcept { return x; }

template <typename...>
struct are_same: std::true_type { };
template <typename A, typename... B>
struct are_same<A,B...>: std::bool_constant<(... && std::is_same_v<A,B>)> { };
template <typename... T>
constexpr bool are_same_v = are_same<T...>::value;

template <typename T, size_t I>
struct indexed_type {
  using type = T;
  static constexpr size_t index = I;
};

template <typename, typename>
struct lcat_type_sequence;
template <typename A, typename... B>
struct lcat_type_sequence<A,type_sequence<B...>> {
  using type = type_sequence<A,B...>;
};

template <size_t I, typename T, typename... Ts>
struct indexed_type_sequence {
  using type = typename lcat_type_sequence<
    indexed_type<T,I>,
    typename indexed_type_sequence<I+1,Ts...>::type
  >::type;
};
template <size_t I, typename T>
struct indexed_type_sequence<I,T> {
  using type = type_sequence<indexed_type<T,I>>;
};
template <typename... Ts>
using make_indexed_type_sequence =
  typename indexed_type_sequence<0,Ts...>::type;

} // end namespace map

#endif
