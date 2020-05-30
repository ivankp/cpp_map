#ifndef IVANP_CONTAINERS_HH
#define IVANP_CONTAINERS_HH

#include <functional>
#include <array>
#include <vector>

#include <containers/concepts.hh>
#include <enum_class_bitmask.hh>

namespace ivanp::containers {
enum class flags {
  none         = 0,
  forward      = 1 << 0,
  check_length = 1 << 1,
  prefer_tuple = 1 << 2
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
      !is_for_each_element<curry<returns_not_void,F&&>,C&&>
    ) { // returns void
      std::apply([&](auto&&... x){ ( ..., std::invoke(
        std::forward<F>(f), std::forward<decltype(x)>(x) ) );
      },c);
    } else { // returns not void
      return std::apply([&](auto&&... x){
        if constexpr (
          elements_transform_to_same<C&&,
            compose<std::decay_t, curry<std::invoke_result_t,F&&> > >
          && !( !!(flags & flags::forward) &&
            std::is_reference_v<
              std::invoke_result_t<F&&,std::tuple_element_t<0,C>>> )
        ) {
          return std::array { std::invoke(
            std::forward<F>(f), std::forward<decltype(x)>(x) )...
          };
        } else {
          if constexpr (!!(flags & flags::forward)) {
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
    using element_t = decltype(*std::begin(c));
    if constexpr (
      returns_void<F&&,element_t>::value
    ) { // returns void
      for (auto&& x : c)
        std::invoke( std::forward<F>(f), std::forward<decltype(x)>(x) );
    } else { // returns not void
      using result_t = std::invoke_result_t<F,element_t>;
      std::vector<
        std::conditional_t<
          !(flags & flags::forward) || !std::is_lvalue_reference_v<result_t>,
          std::decay_t<result_t>,
          std::reference_wrapper<std::remove_reference_t<result_t>>
        >
      > out;
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
  using indices = container_index_sequence<C...>;

  using result_types =
    decltype([]<size_t... I>(std::index_sequence<I...>) {
      auto impl = []<size_t J>(index_constant<J>) {
        return type_constant<
          std::invoke_result_t<F&&,container_element_t<C&&,J>...>
        >{};
      };
      return type_sequence<typename decltype(
        impl(index_constant<I>{}) )::type ...>{};
    }(indices{}));

  static constexpr auto ret =
    []<typename... T>(type_sequence<T...>) {
      return (struct {
        bool has_void, same, refs, constructible;
      }) {
        (... || std::is_void_v<T>),
        are_same_v<T...>,
        (... || std::is_reference_v<T>),
        (... || std::is_constructible_v<std::decay_t<T>,T>),
      };
    }(result_types{});

  if constexpr ((... || Tuple<C>)) { // at least one tuple
    return [&]<size_t... I>(std::index_sequence<I...>) {
      auto impl = [&]<size_t J>(index_constant<J>) -> decltype(auto) {
        return std::invoke(
          std::forward<F>(f),
          std::get<J>(std::forward<C>(c))... // TODO: get
        );
      };
      if constexpr ( ret.has_void )
        ( ..., impl(index_constant<I>{}) );
      else if constexpr (
        !(flags & flags::prefer_tuple) &&
        ret.constructible &&
        ret.same && !( !!(flags & flags::forward) && ret.refs )
      )
        // TODO: enforce execution order?
        return std::array { impl(index_constant<I>{}) ... };
      else if constexpr ( !(flags & flags::forward) && ret.constructible )
        return std::tuple { impl(index_constant<I>{}) ... };
      else
        return std::forward_as_tuple( impl(index_constant<I>{}) ... );
    }(indices{});
  } else { // not a tuple
  }
}

} // end namespace impl

template <flags flags=flags::none, Container... C, typename F>
requires InvocableForElements<F&&,C&&...>
inline decltype(auto) map(F&& f, C&&... c) {
  return impl::map<flags>(std::forward<F>(f),std::forward<C>(c)...);
}

template <flags flags=flags::none, typename... T, typename F>
requires Invocable<F&&,T...>
inline decltype(auto) map(F&& f, std::initializer_list<T>... c) {
  return impl::map<flags>(std::forward<F>(f),c...);
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
