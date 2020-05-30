#ifndef IVANP_CONTAINERS_HH
#define IVANP_CONTAINERS_HH

#include <functional>
#include <array>
#include <vector>

#include <containers/concepts.hh>
#include <enum_class_bitmask.hh>

namespace ivanp::containers {
enum class flags {
  none = 0,
  forward = 1,
  check_length = 2
};
}

namespace ivanp {
template <>
constexpr bool enable_bitmask_operators<containers::flags> = true;
}

namespace ivanp::containers {
namespace impl {

template <flags flags, typename F, typename C>
inline decltype(auto) map(F&& f, C&& c) {
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
          && !( (flags & flags::forward)!=flags::none &&
            std::is_reference_v<
              std::invoke_result_t<F&&,std::tuple_element_t<0,C>>> )
        ) {
          return std::array { std::invoke(
            std::forward<F>(f), std::forward<decltype(x)>(x) )...
          };
        } else {
          if constexpr ((flags & flags::forward) != flags::none) {
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

template <flags flags, typename F, typename... C>
requires (sizeof...(C) > 1)
inline decltype(auto) map(F&& f, C&&... c) {
}

} // end namespace impl

template <flags flags=flags::none, Container... C, typename F>
requires InvocableForElements<F&&,C&&...>
inline decltype(auto) map(F&& f, C&&... c) {
  return impl::map<flags::forward>(std::forward<F>(f),std::forward<C>(c)...);
}

template <flags flags=flags::none, typename... T, typename F>
requires Invocable<F&&,T...>
inline decltype(auto) map(F&& f, std::initializer_list<T>... c) {
  return impl::map<flags::forward>(std::forward<F>(f),c...);
}

namespace operators { // --------------------------------------------

template <Container C, typename F>
requires InvocableForElements<F&&,C>
inline decltype(auto) operator|(C&& c, F&& f) {
  return impl::map<flags::none>(std::forward<F>(f),std::forward<C>(c));
}

template <Container C, typename F>
requires InvocableForElements<F&&,C>
inline decltype(auto) operator||(C&& c, F&& f) {
  return impl::map<flags::forward>(std::forward<F>(f),std::forward<C>(c));
}

template <Tuple C, typename F>
requires Applyable<F&&,C>
inline constexpr decltype(auto) operator%(C&& c, F&& f) {
  return std::apply(std::forward<F>(f),std::forward<C>(c));
}

} // end namespace operators
} // end namespace containers

#endif
