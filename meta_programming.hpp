#ifndef META_PROGRAMMING_HELPERS
#define META_PROGRAMMING_HELPERS

#include <type_traits>

/// Meta programming helper functions.
/// Inspired by GCC meta programming helper functions for type_traits

template<typename...>
  struct __or_;

template<>
  struct __or_<>
  : public std::false_type
  { };

template<typename T1>
   struct __or_<T1>
   : public T1
   { };

template<typename T1, typename T2>
  struct __or_<T1, T2>
  : public std::conditional<T1::value, T1, T2>::type
  { };

template<typename T1, typename T2, typename T3, typename... Tn>
   struct __or_<T1, T2, T3, Tn...>
   : public std::conditional<T1::value, T1, __or_<T2, T3, Tn...>>::type
   { };

template<typename...>
  struct __and_;

template<>
  struct __and_<>
  : public std::true_type
  { };

template<typename T1>
  struct __and_<T1>
  : public T1
  { };

template<typename T1, typename T2>
  struct __and_<T1, T2>
  : public std::conditional<T1::value, T2, T1>::type
  { };

template<typename T1, typename T2, typename T3, typename... Tn>
  struct __and_<T1, T2, T3, Tn...>
  : public std::conditional<T1::value, __and_<T2, T3, Tn...>, T1>::type
  { };

template<bool T>
  using __bool_constant = std::integral_constant<bool, T>;

template<typename T>
  struct __not_
  : public __bool_constant<!bool(T::value)>
  { };

#endif // META_PROGRAMMING_HELPERS
