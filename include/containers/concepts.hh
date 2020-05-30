#ifndef IVANP_CONTAINERS_CONCEPTS_HH
#define IVANP_CONTAINERS_CONCEPTS_HH

#include <concepts>

#include <containers/traits.hh>

namespace ivanp::containers {

/*
template <auto> struct require_constant;

template <typename T>
concept ConstSizable =
// TODO: isn't working
  requires {
    { T::size() } -> std::convertible_to<std::size_t>;
    typename require_constant<T::size()>;
  };
*/

template <typename C>
concept Tuple =
  requires {
    { std::tuple_size<std::remove_reference_t<C>>::value }
      -> std::convertible_to<std::size_t>;
  };

template <typename C>
concept Incrementable =
  requires(C& a) {
    ++a;
  };

template <typename C>
concept Iterable =
  requires(C& a) {
    std::begin(a) != std::end(a);
    requires Incrementable<decltype(std::begin(a))>;
  };

template <typename C>
concept Container = Iterable<C> || Tuple<C>;

template <typename C>
concept Sizable =
  requires(C& a) {
    { std::size(a) } -> std::convertible_to<std::size_t>;
  };

template <typename C>
concept List = Iterable<C> && Sizable<C>;

template <Container C>
struct container_traits {
  using type = std::remove_reference_t<C>;
  template <size_t I=0>
  using element_ref_type = decltype(*std::begin(std::declval<C>()));
  static constexpr size_t size = 0;
};

template <Tuple C>
struct container_traits<C> {
  using type = std::remove_reference_t<C>;
  template <size_t I>
  using element_ref_type = decltype(std::get<I>(std::declval<C>()));
  static constexpr size_t size = std::tuple_size_v<type>;
};

template <typename C, size_t I>
using container_element_t = typename container_traits<C>::element_ref_type<I>;

template <typename C>
constexpr size_t container_size = container_traits<C>::size;

template <typename... C>
constexpr size_t min_container_size =
  []() -> size_t {
    size_t n = 0;
    for (auto [s, t] : std::initializer_list<std::pair<size_t,bool>>{
      { container_size<C>, Tuple<C> } ...
    }) {
      if (s==0) {
        if (t) return 0;
      } else {
        if (n==0 || s<n) n = s;
      }
    }
    return n==0 ? 1 : n;
  }();

template <typename... C>
using container_index_sequence =
  std::make_index_sequence<min_container_size<C...>>;

template <typename F, typename... Args>
concept Invocable = std::is_invocable_v<F,Args...>;

template <typename F, typename C>
static constexpr bool can_apply =
  []<size_t... I>(std::index_sequence<I...>) {
    return Invocable<F,container_element_t<C,I>...>;
  }(container_index_sequence<C>{});

template <typename F, typename C>
concept Applyable = can_apply<F,C>;

template <typename F, typename... C>
constexpr bool is_invocable_for_elements =
  []<size_t... I>(std::index_sequence<I...>) {
    auto impl = []<size_t J>(std::integral_constant<size_t,J>) {
      return Invocable<F,container_element_t<C,J>...>;
    };
    return (impl(std::integral_constant<size_t,I>{}) && ...);
  }(container_index_sequence<C...>{});

template <typename F, typename... C>
concept InvocableForElements = is_invocable_for_elements<F,C...>;

template <typename C, typename Pred>
static constexpr bool is_for_each_element =
[]<size_t... I>(std::index_sequence<I...>) {
  return (apply_type<Pred,container_element_t<C,I>>::value && ...);
}(container_index_sequence<C>{});

template <typename C, typename Pred>
static constexpr bool elements_transform_to_same =
[]<size_t I0,size_t... I>(std::index_sequence<I0,I...>) {
  return (std::is_same_v<
    apply_type<Pred,container_element_t<C,I0>>,
    apply_type<Pred,container_element_t<C,I>>
  > && ...);
}(container_index_sequence<C>{});

} // end namespace containers

#endif
