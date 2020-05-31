#ifndef IVANP_MAP_HH
#define IVANP_MAP_HH

#include <functional>
#include <array>
#include <vector>

#include <map/concepts.hh>
#include <enum_class_bitmask.hh>

namespace ivanp::map {
enum class flags {
  none = 0,
  forward = 1 << 0,
  no_static_size_check  = 1 << 1,
  no_dynamic_size_check = 1 << 2,
  no_size_check = no_static_size_check | no_dynamic_size_check,
  prefer_tuple  = 1 << 3,
  prefer_iteration = 1 << 4
};
}

namespace ivanp {
template <>
constexpr bool enable_bitmask_operators<map::flags> = true;
}

namespace ivanp::map {
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
  using dimensions = std::make_index_sequence<sizeof...(C)>;
  static constexpr bool some_tuples = (... || Tuple<C>);

  if constexpr (!(flags & flags::no_static_size_check) && sizeof...(C)>1) {
    (..., []<typename _C>(type_constant<_C>) {
      static_assert(
        !Tuple<_C> || indices::size() == container_size<_C>,
        "tuples of unequal size given to map");
    }(type_constant<C>{}));
  }
  if constexpr (!(flags & flags::no_dynamic_size_check) && sizeof...(C)>1) {
    auto impl = [
      first = !some_tuples,
      s = indices::size()
    ] <typename _C> (_C&& _c) mutable {
      if constexpr (
        Sizable<_C> && !(!(flags & flags::no_static_size_check) && Tuple<_C>)
      ) {
        if (first) {
          first = false;
          s = std::size(_c);
        } else if (std::size(_c) != s)
          throw std::length_error("containers of unequal size given to map");
      }
    };
    (..., impl(c));
  }

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

  if constexpr ( // map to tuple
    !(!!(flags & flags::prefer_iteration) && (... && Iterable<C>))
    && some_tuples
  ) {
    // Note: evaluation order is sequential for list-initialization
    // https://en.cppreference.com/w/cpp/language/eval_order   Rule 10
    return [&]<size_t... I>(std::index_sequence<I...>) {
      std::tuple iterators {
        []<typename _C>(_C&& _c) {
          if constexpr (Tuple<_C>)
            return tagged<_C&&,_C&&> { std::forward<_C>(_c) };
          else return tagged<_C&&,std::pair<
              decltype(std::forward<_C>(_c).begin()),
              decltype(std::forward<_C>(_c).end())
            >> {
            std::pair {
              std::forward<_C>(_c).begin(),
              std::forward<_C>(_c).end()
            }};
        }(std::forward<C>(c)) ...
      };

      auto impl = [&]<size_t J>(index_constant<J>) -> decltype(auto) {
        return [&]<size_t... Ks>(std::index_sequence<Ks...>) -> decltype(auto) {
          return std::invoke(
            std::forward<F>(f),
            [&]<size_t K>(index_constant<K>) -> decltype(auto) {
              decltype(auto) iter = std::get<K>(iterators);
              using _C = typename std::decay_t<decltype(iter)>::tag;
              if constexpr (Tuple<_C>) {
                return std::get<J>(iter.value);
              } else {
                auto& it = iter.value.first;
                if constexpr (
                  !(flags & flags::no_dynamic_size_check) && !Sizable<_C>
                ) {
                  if (it == iter.value.second) throw std::length_error(
                    "in map: container reached end before others");
                }
                decltype(auto) x = *it;
                ++it;
                return x;
              }
            }(index_constant<Ks>{}) ...
          );
        }(dimensions{});
      };

      if constexpr ( ret.has_void )
        ( ..., impl(index_constant<I>{}) );
      else if constexpr (
        !(flags & flags::prefer_tuple) &&
        ret.constructible &&
        ret.same && ( !(flags & flags::forward) || !ret.refs )
      )
        return std::array { impl(index_constant<I>{}) ... };
      else if constexpr ( !(flags & flags::forward) && ret.constructible )
        return std::tuple { impl(index_constant<I>{}) ... };
      else {
        // can't use std::forward_as_tuple() because the order of evaluation
        // of function arguments is not specified
        using return_type = decltype(
          []<typename... T>(type_sequence<T...>) -> std::tuple<T&&...> {
            return { };
          }(result_types{}));
        return return_type{ impl(index_constant<I>{}) ... };
      }
    }(indices{});
  } else { // map to vector
    std::tuple iterators {
      std::pair {
        std::forward<C>(c).begin(),
        std::forward<C>(c).end()
      } ...
    };
    return [&]<size_t... K>(std::index_sequence<K...>) -> decltype(auto) {
      if constexpr ( ret.has_void ) {
        for (;;) {
          if constexpr (!!(flags & flags::no_dynamic_size_check)) {
            if ((... || (
              std::get<K>(iterators).first == std::get<K>(iterators).second
            ))) return;
          } else {
            const size_t n_ended = (... + (
              std::get<K>(iterators).first == std::get<K>(iterators).second
            ));
            if (n_ended == sizeof...(K)) return;
            else if (n_ended != 0) throw std::length_error(
              "in map: container reached end before others");
          }

          std::invoke( std::forward<F>(f), *std::get<K>(iterators).first ... );
          ( ..., ++std::get<K>(iterators).first );
        }
      } else {
        // return result_types{};
        return type_constant<result_types>{};
        // using result_t = type_sequence_head_t<result_types>;
        // return type_constant<result_t>{};
        // std::vector<
        //   std::conditional_t<
        //     !(flags & flags::forward)
        //     || !std::is_lvalue_reference_v<result_t>,
        //     std::decay_t<result_t>,
        //     std::reference_wrapper<std::remove_reference_t<result_t>>
        //   >
        // > out;
        // return out;
        // if constexpr (Sizable<pack_element_t<0,C&...>>)
        //   out.reserve(std::size(head_value(c...)));

        // for (;;) {
        //   if constexpr (!!(flags & flags::no_dynamic_size_check)) {
        //     if ((... || (
        //       std::get<K>(iterators).first == std::get<K>(iterators).second
        //     ))) return out;
        //   } else {
        //     const size_t n_ended = (... + (
        //       std::get<K>(iterators).first == std::get<K>(iterators).second
        //     ));
        //     if (n_ended == sizeof...(K)) return out;
        //     else if (n_ended != 0) throw std::length_error(
        //       "in map: container reached end before others");
        //   }
        //
        //   out.push_back( std::invoke(
        //     std::forward<F>(f), *std::get<K>(iterators).first ... ) );
        //   ( ..., ++std::get<K>(iterators).first );
        // }
      }
    }(dimensions{});
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

template <flags flags=flags::none, typename F>
requires Invocable<F&&>
inline decltype(auto) map(F&& /*f*/) {
  // return impl::map<flags>(std::forward<F>(f));
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
} // end namespace map

#endif
