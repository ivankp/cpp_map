#ifndef IVANP_CONTAINERS_HH
#define IVANP_CONTAINERS_HH

#include <functional>
#include <vector>

#include <containers/concepts.hh>

namespace ivanp::containers {

namespace impl {

template <Iterable C, typename F>
inline void map(C&& c, F&& f)
requires InvocableForElementsOfIterable<F,C>
&& ReturnsVoidForElementsOfIterable<F,C>
&& (!Tuple<C>)
{
  for (auto&& x : c)
    std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) );
}

template <Iterable C, typename F>
inline decltype(auto) map(C&& c, F&& f)
requires InvocableForElementsOfIterable<F,C>
&& (!ReturnsVoidForElementsOfIterable<F,C>)
&& (!Tuple<C>)
{
  std::vector<std::invoke_result_t<
    F, decltype(*std::begin(std::declval<C&>())) >> out;
  if constexpr (Sizable<C>)
    out.reserve(std::size(c));
  for (auto&& x : c)
    out.push_back(
      std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) ) );
  return out;
}

template <Tuple C, typename F>
inline void map(C&& c, F&& f)
requires InvocableForElementsOfTuple<F,C>
{
  std::apply([&](auto&&... x){
    ( std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) ), ... );
  },c);
}

template <Tuple C, typename F>
inline decltype(auto) map(C&& c, F&& f)
requires InvocableForElementsOfTuple<F,C>
&& (!ReturnsVoidForElementsOfTuple<F,C>)
{
  return std::apply([&](auto&&... x){
    if constexpr (same_tuple_types<C>) {
      return std::array {
        std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) )...
      };
    } else {
      return std::tuple {
        std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) )...
      };
    }
  },c);
}

} // end namespace impl

template <Container C, typename F>
inline decltype(auto) map(C&& c, F&& f)
requires InvocableForElements<F,C>
{
  return impl::map(std::forward<C>(c),std::forward<F>(f));
}

template <typename T, typename F>
inline decltype(auto) map(std::initializer_list<T> c, F&& f)
requires Invocable<F,T>
{
  return impl::map(c,std::forward<F>(f));
}

namespace operators { // --------------------------------------------

template <Container C, typename F>
inline decltype(auto) operator|(C&& c, F&& f)
requires InvocableForElements<F,C>
{
  return impl::map(std::forward<C>(c),std::forward<F>(f));
}

template <Tuple C, typename F>
inline constexpr decltype(auto) operator%(C&& c, F&& f) {
  return std::apply(std::forward<F>(f),std::forward<C>(c));
}

} // end namespace operators
} // end namespace containers

#endif
