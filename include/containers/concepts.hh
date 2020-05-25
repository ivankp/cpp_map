#ifndef IVANP_CONTAINERS_CONCEPTS_HH
#define IVANP_CONTAINERS_CONCEPTS_HH

#include <concepts>

#include <containers/traits.hh>

namespace ivanp::containers {

template <auto> struct require_constant;

template <typename T>
concept Incrementable =
  requires(T& a) {
    ++a;
  };

template <typename T>
concept Iterable =
  requires(T& a) {
    std::begin(a) != std::end(a);
    requires Incrementable<decltype(std::begin(a))>;
  };

template <typename T>
concept Sizable =
  requires(T& a) {
    { std::size(a) } -> std::convertible_to<std::size_t>;
  };

template <typename T>
concept ConstSizable =
// TODO: isn't working
  requires {
    { T::size() } -> std::convertible_to<std::size_t>;
    typename require_constant<T::size()>;
  };

template <typename T>
concept List = Iterable<T> && Sizable<T>;

template <typename T>
concept Tuple =
  requires {
    { std::tuple_size<T>::value } -> std::convertible_to<std::size_t>;
  };

template <typename T>
concept Container = Iterable<T> || Tuple<T>;

template <typename F, typename... Args>
concept Invocable = std::is_invocable_v<F,Args...>;

template <typename F, typename C>
concept InvocableForElementsOfIterable =
  Invocable<F, decltype(*std::begin(std::declval<C&>()))>;

template <typename F, typename C>
concept InvocableForElementsOfTuple =
  is_for_each_element<C, curry<std::is_invocable,F>>;

template <typename F, typename C>
concept InvocableForElements =
  ( Iterable<C> && InvocableForElementsOfIterable<F,C> ) ||
  ( Tuple<C> && InvocableForElementsOfTuple<F,C> );

template <typename F, typename C>
concept ReturnsVoidForElementsOfIterable =
  returns_void<F,decltype(*std::begin(std::declval<C&>()))>::value;

template <typename F, typename C>
concept ReturnsVoidForElementsOfTuple =
  !is_for_each_element<C, curry<returns_not_void,F>>;

} // end namespace containers

#endif
