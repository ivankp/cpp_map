#ifndef IVANP_CONTAINERS_TRAITS_HH
#define IVANP_CONTAINERS_TRAITS_HH

namespace ivanp::containers {

template <template<typename,typename...> typename F, typename A>
struct bind_first_param {
  template <typename... T> using type = F<A,T...>;
};

template <typename T, template<typename...> typename Pred>
struct is_for_each_element {
  template <size_t... I>
  static constexpr bool impl(std::index_sequence<I...>) {
    return (Pred<std::tuple_element_t<I,T>>::value && ...);
  }
  static constexpr bool value =
    impl(std::make_index_sequence<std::tuple_size_v<T>>{});
};

} // end namespace containers

#endif
