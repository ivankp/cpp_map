#ifndef IVANP_CONTAINERS_HH
#define IVANP_CONTAINERS_HH

#include <functional>
#include <vector>

#include <containers/concepts.hh>

namespace ivanp::containers {

namespace impl {

template <bool Forward, Iterable C, typename F>
requires ReturnsVoidForElementsOfIterable<F&&,C> && (!Tuple<C>)
inline void map(C&& c, F&& f) {
  for (auto&& x : c)
    std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) );
}

template <bool Forward, Iterable C, typename F>
requires (!ReturnsVoidForElementsOfIterable<F&&,C>) && (!Tuple<C>)
inline decltype(auto) map(C&& c, F&& f) {
  std::vector<std::invoke_result_t<
    F, decltype(*std::begin(std::declval<C&>())) >> out;
  if constexpr (Sizable<C>)
    out.reserve(std::size(c));
  for (auto&& x : c)
    out.push_back(
      std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) ) );
  return out;
}

template <bool Forward, Tuple C, typename F>
requires ReturnsVoidForElementsOfTuple<F&&,C>
inline void map(C&& c, F&& f) {
  std::apply([&](auto&&... x){
    ( std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) ), ... );
  },c);
}

template <bool Forward, Tuple C, typename F>
requires (!ReturnsVoidForElementsOfTuple<F&&,C>)
inline decltype(auto) map(C&& c, F&& f) {
  return std::apply([&](auto&&... x){
    if constexpr ( Forward
      ? elements_transform_to_same<C,
          compose<std::remove_cv_t, curry<std::invoke_result_t,F&&> >
        > &&
        !std::is_reference_v<
          std::invoke_result_t<F&&,std::tuple_element_t<0,C>>>
      : elements_transform_to_same<C,
          compose<std::decay_t, curry<std::invoke_result_t,F&&> >
        >
    ) {
      return std::array {
        std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) )...
      };
    } else {
      if constexpr (Forward) {
        return std::forward_as_tuple(
          std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) )...
        );
      } else {
        return std::tuple {
          std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) )...
        };
      }
    }
  },c);
}

} // end namespace impl

template <Container C, typename F>
requires InvocableForElements<F&&,C>
inline decltype(auto) map(C&& c, F&& f) {
  return impl::map<0>(std::forward<C>(c),std::forward<F>(f));
}

template <typename T, typename F>
requires Invocable<F&&,T>
inline decltype(auto) map(std::initializer_list<T> c, F&& f) {
  return impl::map<0>(c,std::forward<F>(f));
}

template <Container C, typename F>
requires InvocableForElements<F&&,C>
inline decltype(auto) map_forward(C&& c, F&& f) {
  return impl::map<1>(std::forward<C>(c),std::forward<F>(f));
}

template <typename T, typename F>
requires Invocable<F&&,T>
inline decltype(auto) map_forward(std::initializer_list<T> c, F&& f) {
  return impl::map<1>(c,std::forward<F>(f));
}

namespace operators { // --------------------------------------------

template <Container C, typename F>
requires InvocableForElements<F&&,C>
inline decltype(auto) operator|(C&& c, F&& f) {
  return impl::map<0>(std::forward<C>(c),std::forward<F>(f));
}

template <Container C, typename F>
requires InvocableForElements<F&&,C>
inline decltype(auto) operator||(C&& c, F&& f) {
  return impl::map<1>(std::forward<C>(c),std::forward<F>(f));
}

template <Tuple C, typename F>
inline constexpr decltype(auto) operator%(C&& c, F&& f) {
  return std::apply(std::forward<F>(f),std::forward<C>(c));
}

} // end namespace operators
} // end namespace containers

#endif
