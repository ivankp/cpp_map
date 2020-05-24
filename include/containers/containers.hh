#ifndef IVANP_CONTAINERS_HH
#define IVANP_CONTAINERS_HH

#include <functional>

#include <containers/concepts.hh>

namespace ivanp::containers {

template <List C, typename F>
inline void map(C&& c, F&& f) requires InvocableWithElement<F,C> {
  for (const auto& x : c) std::invoke(f,x);
  // TODO: move correctly
}

template <Tuple C, typename F>
inline void map(C&& c, F&& f) requires InvocableWithElement<F,C> {
  [&]<size_t... I>(std::index_sequence<I...>) {
    ( std::invoke(f,std::get<I>(c)), ... );
  }(std::make_index_sequence<std::tuple_size_v<C>>{});
}

template <Container C, typename F>
inline void operator|(C&& c, F&& f) requires InvocableWithElement<F,C> {
  return map(std::forward<C>(c),std::forward<F>(f));
}

template <Tuple T, typename F>
inline constexpr decltype(auto) operator%(T&& t, F&& f) {
  return std::apply(std::forward<F>(f),std::forward<T>(t));
}


} // end namespace containers

#endif
