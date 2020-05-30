#ifndef IVANP_MAP_TRAITS_HH
#define IVANP_MAP_TRAITS_HH

#include <type_traits>
#include <utility>
#include <tuple>

namespace ivanp::map {

template <typename T>
struct type_constant {
  using type = T;
};

template <typename... T>
struct type_sequence {
  // static constexpr size_t size = sizeof...(T);
};

template <size_t I>
using index_constant = std::integral_constant<size_t,I>;

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

} // end namespace map

#endif
