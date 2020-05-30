#ifndef IVANP_CONTAINERS_TRAITS_HH
#define IVANP_CONTAINERS_TRAITS_HH

#include <type_traits>
#include <utility>
#include <tuple>

namespace ivanp::containers {

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

} // end namespace containers

#endif
