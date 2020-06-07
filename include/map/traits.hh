#ifndef IVANP_MAP_TRAITS_HH
#define IVANP_MAP_TRAITS_HH

#include <type_traits>
#include <utility>
#include <tuple>

namespace ivanp::map {

// https://ldionne.com/2015/11/29/efficient-parameter-pack-indexing/
template <size_t I, typename T, typename... Ts>
struct pack_element { using type = typename pack_element<I-1, Ts...>::type; };
template <typename T, typename... Ts>
struct pack_element<0, T, Ts...> { using type = T; };
template <size_t I, typename... Ts>
using pack_element_t = typename pack_element<I, Ts...>::type;

template <typename T, typename... Ts>
constexpr T&& head_value(T&& x, Ts&&...) noexcept { return x; }

template <typename T>
struct type_constant { using type = T; };

template <typename... T>
struct type_sequence { };

template <size_t I>
using index_constant = std::integral_constant<size_t,I>;

template <typename...>
struct are_same: std::true_type { };
template <typename A, typename... B>
struct are_same<A,B...>: std::bool_constant<(... && std::is_same_v<A,B>)> { };
template <typename... T>
constexpr bool are_same_v = are_same<T...>::value;

} // end namespace map

#endif
