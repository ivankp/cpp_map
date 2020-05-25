#ifndef IVANP_CONTAINERS_TRAITS_HH
#define IVANP_CONTAINERS_TRAITS_HH

#include <type_traits>
#include <utility>
#include <tuple>
#include <array>

namespace ivanp::containers {

template <template<typename,typename...> typename F, typename A>
struct bind_first_param {
  template <typename... T> using type = F<A,T...>;
};

template <typename T>
using tuple_index_sequence = std::make_index_sequence<std::tuple_size_v<T>>;

template <typename T, template<typename...> typename Pred>
static constexpr bool is_for_each_element =
[]<size_t... I>(std::index_sequence<I...>) {
  return (Pred<std::tuple_element_t<I,T>>::value && ...);
}(tuple_index_sequence<T>{});

template <typename F, typename... T>
using returns_void = std::is_void< std::invoke_result_t<F,T...> >;

template <typename F, typename... T>
using returns_not_void = std::negation< returns_void<F,T...> >;

template <typename T>
static constexpr bool same_tuple_types =
[]<size_t I0,size_t... I>(std::index_sequence<I0,I...>) {
  return (std::is_same<
    std::tuple_element_t<I0,T>,
    std::tuple_element_t<I,T>
  >::value && ...);
}(tuple_index_sequence<T>{});

} // end namespace containers

#endif
