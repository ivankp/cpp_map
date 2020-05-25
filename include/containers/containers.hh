#ifndef IVANP_CONTAINERS_HH
#define IVANP_CONTAINERS_HH

#include <functional>
#include <array>
#include <vector>

#include <containers/concepts.hh>

namespace ivanp::containers {

namespace impl {

template <bool Forward, typename C, typename F>
inline decltype(auto) map(C&& c, F&& f) {
  if constexpr (Tuple<C>) { // is a tuple
    if constexpr (
      !is_for_each_element<C, curry<returns_not_void,F&&>>
    ) { // returns void
      std::apply([&](auto&&... x){ ( std::invoke(
        std::forward<F>(f), std::forward<decltype(x)>(x) ), ... );
      },c);
    } else { // returns not void
      return std::apply([&](auto&&... x){
        if constexpr (
          elements_transform_to_same<C,
            compose<std::decay_t, curry<std::invoke_result_t,F&&> > >
          && !( Forward &&
            std::is_reference_v<
              std::invoke_result_t<F&&,std::tuple_element_t<0,C>>> )
        ) {
          return std::array { std::invoke(
            std::forward<F>(f), std::forward<decltype(x)>(x) )...
          };
        } else {
          if constexpr (Forward) {
            return std::forward_as_tuple( std::invoke(
              std::forward<F>(f), std::forward<decltype(x)>(x) )...
            );
          } else {
            return std::tuple { std::invoke(
              std::forward<F>(f), std::forward<decltype(x)>(x) )...
            };
          }
        }
      },c);
    }
  } else { // not a tuple
    if constexpr (
      returns_void<F&&,decltype(*std::begin(c))>::value
    ) { // returns void
      for (auto&& x : c)
        std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) );
    } else { // returns not void
      std::vector<std::decay_t<std::invoke_result_t<
        F, decltype(*std::begin(c)) >>> out;
      if constexpr (Sizable<C>)
        out.reserve(std::size(c));
      for (auto&& x : c)
        out.push_back( std::invoke(
          std::forward<F>(f), std::forward<decltype(x)>(x) ) );
      return out;
    }
  }
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
requires Applyable<F&&,C>
inline constexpr decltype(auto) operator%(C&& c, F&& f) {
  return std::apply(std::forward<F>(f),std::forward<C>(c));
}

} // end namespace operators
} // end namespace containers

#endif
