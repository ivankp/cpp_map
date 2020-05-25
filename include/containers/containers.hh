#ifndef IVANP_CONTAINERS_HH
#define IVANP_CONTAINERS_HH

#include <functional>

#include <containers/concepts.hh>

namespace ivanp::containers {

template <List L, typename F>
inline void map(L&& l, F&& f) requires InvocableForElementsOfIterable<F,L> {
  for (auto&& x : l)
    std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) );
}

template <Tuple T, typename F>
inline void map(T&& t, F&& f) requires InvocableForElementsOfTuple<F,T> {
  std::apply([&](auto&&... x){
    ( std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) ), ... );
  },t);
}

namespace operators { // --------------------------------------------

template <Container C, typename F>
inline void operator|(C&& c, F&& f) requires InvocableForElements<F,C> {
  return map(std::forward<C>(c),std::forward<F>(f));
}

template <Tuple T, typename F>
inline constexpr decltype(auto) operator%(F&& f, T&& t) {
  return std::apply(std::forward<F>(f),std::forward<T>(t));
}

} // end namespace operators
} // end namespace containers

#endif
