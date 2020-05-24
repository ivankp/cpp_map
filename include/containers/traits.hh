#ifndef IVANP_CONTAINERS_TRAITS_HH
#define IVANP_CONTAINERS_TRAITS_HH

#include <utility>
#include <tuple>

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

} // end namespace containers

#endif
