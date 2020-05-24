#ifndef IVANP_CONTAINERS_HH
#define IVANP_CONTAINERS_HH

#include <functional>

#include <containers/concepts.hh>

namespace ivanp::containers {

template <List L, typename F>
inline void map(L&& l, F&& f) requires InvocableForElements<F,L> {
  for (const auto& x : l) std::invoke(f,x);
  // TODO: forward correctly
}

template <Tuple T, typename F>
inline void map(T&& t, F&& f) requires InvocableForElements<F,T> {
  [&]<size_t... I>(std::index_sequence<I...>) {
    ( std::invoke(f,std::get<I>(t)), ... );
  }(tuple_index_sequence<T>{});
}

template <Container C, typename F>
inline void operator|(C&& c, F&& f) requires InvocableForElements<F,C> {
  return map(std::forward<C>(c),std::forward<F>(f));
}

template <Tuple T, typename F>
inline constexpr decltype(auto) operator%(T&& t, F&& f) {
  return std::apply(std::forward<F>(f),std::forward<T>(t));
}


} // end namespace containers

#endif
