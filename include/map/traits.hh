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

template <typename T>
struct type_constant { using type = T; };

template <typename... T>
struct type_sequence { };

template <typename>
struct type_sequence_head;
template <typename T, typename... Ts>
struct type_sequence_head<type_sequence<T,Ts...>> { using type = T; };
template <typename Seq>
using type_sequence_head_t = typename type_sequence_head<Seq>::type;

template <size_t I, typename>
struct type_sequence_element;
template <size_t I, typename... Ts>
struct type_sequence_element<I,type_sequence<Ts...>> {
  using type = pack_element_t<I,Ts...>;
};
template <size_t I, typename Seq>
using type_sequence_element_t = typename type_sequence_element<I,Seq>::type;

template <typename T, typename... Ts>
constexpr T&& head_value(T&& x, Ts&&...) noexcept { return x; }

template <size_t I>
using index_constant = std::integral_constant<size_t,I>;

// ##################################################################

template <template<typename...> typename F, typename... A>
struct curry {
  template <typename... T> using type = F<A...,T...>;
};

template <typename Trait, typename... Args>
using apply_type = typename Trait::type<Args...>;

template <template<typename> typename A, typename B>
struct compose {
  template <typename... T> using type = A<apply_type<B,T...>>;
};

template <typename F, typename... T>
using returns_void = std::is_void< std::invoke_result_t<F,T...> >;

template <typename F, typename... T>
using returns_not_void = std::negation< returns_void<F,T...> >;

template <typename...>
struct are_same: std::true_type { };

template <typename A, typename... B>
struct are_same<A,B...>: std::bool_constant<(... && std::is_same_v<A,B>)> { };

template <typename... T>
constexpr bool are_same_v = are_same<T...>::value;

template <typename Tag, typename T>
struct tagged {
  using tag = Tag;
  using type = T;
  T value;
};

} // end namespace map

#endif
