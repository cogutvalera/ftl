/*
 * Copyright (c) 2013, 2016 Björn Aili
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 */
#ifndef FTL_TYPE_TRAITS_H
#define FTL_TYPE_TRAITS_H

#include <utility>
#include <iterator>
#include <functional>
#include "type_functions.h"

#define FTL_GEN_PREUNOP_TEST(op, name)\
	template<typename T>\
	auto test_ ## name (typename std::remove_reference<\
			decltype(op std::declval<T&>())\
	>::type*)\
	-> decltype(op std::declval<T&>());\
	\
	template<typename T>\
	no test_ ## name (...)

#define FTL_GEN_POSTUNOP_TEST(op, name)\
	template<typename T>\
	auto test_ ## name (typename std::remove_reference<\
			decltype(std::declval<T&>() op)\
	>::type*)\
	-> decltype(std::declval<T&>() op);\
	\
	template<typename T>\
	no test_ ## name (...)

#define FTL_GEN_BINOP_TEST(op, name)\
	template<typename T>\
	auto test_ ## name (decltype(std::declval<T>() op std::declval<T>())*)\
	-> decltype(std::declval<T>() op std::declval<T>());\
	\
	template<typename T>\
	no test_ ## name (...)

#define FTL_GEN_UNFN_TEST(fn, name)\
	template<typename T>\
	bool test_ ## name (decltype( fn (std::declval<T>()))*);\
	\
	template<typename T>\
	no test_ ## name (...)

#define FTL_GEN_BINFN_TEST(fn, name)\
	template<typename T>\
	bool test_ ## name (decltype( fn (std::declval<T>(), std::declval<T>()))*);\
	\
	template<typename T>\
	no test_ ## name (...)

#define FTL_GEN_METH0_TEST(name)\
	template<typename T>\
	bool test_ ## name (decltype(std::declval<T>() . name ())*);\
	\
	template<typename T, typename U>\
	no test_ ## name (...)

#define FTL_GEN_METH1_TEST(name)\
	template<typename T, typename U>\
	bool test_ ## name (decltype(std::declval<T>() . name (std::declval<U>()))*);\
	\
	template<typename T, typename U>\
	no test_ ## name (...)

#define FTL_GEN_METH2_TEST(name)\
	template<typename T, typename U, typename V>\
	auto test_ ## name (\
		decltype(std::declval<T>() . name (std::declval<U>(), std::declval<V>()))*\
	)\
	-> decltype(std::declval<T>() . name (std::declval<U>(), std::declval<V>()));\
	\
	template<typename, typename, typename>\
	no test_ ## name (...)

#define FTL_GEN_TYPEMEM_TEST(name)\
	template<typename T>\
	auto test_typemem_ ## name (\
		typename std::remove_reference<\
			typename T :: name\
		>::type *\
	) -> typename T :: name ;\
	\
	template<typename T>\
	no test_typemem_ ## name (...)

namespace ftl {
	/**
	 * \defgroup typetraits Type Traits
	 *
	 * Collection of useful type traits.
	 *
	 * The main difference between this module and \ref typelevel is that
	 * this module is concerned with finding out particular properties of types,
	 * \ref typelevel is concerned with _modifying_ types.
	 *
	 * \code
	 *   #include <ftl/type_traits.h>
	 * \endcode
	 *
	 * \par Dependencies
	 * - <utility>
	 * - <iterator>
	 * - <functional>
	 * - \ref typelevel
	 */

	namespace _dtl {
		struct no {};

		FTL_GEN_BINOP_TEST(==, eq);
		FTL_GEN_BINOP_TEST(!=, neq);
		FTL_GEN_BINOP_TEST(<, lt);
		FTL_GEN_BINOP_TEST(<, gt);

		FTL_GEN_UNFN_TEST(begin, begin);
		FTL_GEN_METH0_TEST(rbegin);
		FTL_GEN_UNFN_TEST(end, end);
		FTL_GEN_METH0_TEST(rend);

		FTL_GEN_METH1_TEST(push_back);
	}

	// The type functions below could have been autogenerated, but this way
	// they're easier to document.

	/**
	 * Type trait to test for `operator==()`
	 *
	 * Example:
	 * \code
	 *   template<typename T>
	 *   auto foo() -> Requires<ftl::has_eq<T>::value> {
	 *       // ...
	 *       if(some_t == other_t)
	 *       // ...
	 *   }
	 * \endcode
	 *
	 * \ingroup typetraits
	 */
	template<typename T>
	struct has_eq {
		static constexpr bool value =
			std::is_convertible<
				decltype(_dtl::test_eq<T>(nullptr)),
				bool
			>::value;
	};

	/**
	 * Test a type for `operator!=()`.
	 *
	 * \tparam T To satisfy `has_new`, there must be an `operator!=` accepting
	 *           either two const references or two values of type `T` and
	 *           returning something that is contextually convertible to `bool`.
	 *
	 * Example:
	 * \code
	 *   template<typename T>
	 *   auto foo() -> Requires<ftl::has_neq<T>::value> {
	 *       // ...
	 *       if(some_t != other_t)
	 *       // ...
	 *   }
	 * \endcode
	 *
	 * \ingroup typetraits
	 */
	template<typename T>
	struct has_neq {
		static constexpr bool value =
			std::is_convertible<
				decltype(_dtl::test_neq<T>(nullptr)),
				bool
			>::value;
	};


	/**
	 * Test a type for `operator<()`
	 *
	 * \tparam T Satisfies the predicate if there exists an `operator<`
	 *           taking either two `T` or two `const T&` and returning something
	 *           contextually convertible to `bool`.
	 *
	 * Example:
	 * \code
	 *   template<typename T>
	 *   auto foo() -> Requires<ftl::has_lt<T>::value> {
	 *       // ...
	 *       if(some_t < other_t)
	 *       // ...
	 *   }
	 * \endcode
	 *
	 * \ingroup typetraits
	 */
	template<typename T>
	struct has_lt {
		static constexpr bool value =
			std::is_convertible<
				decltype(_dtl::test_lt<T>(nullptr)),
				bool
			>::value;
	};

	/**
	 * Test a type for `operator>()`
	 *
	 * Example:
	 * \code
	 *   template<typename T>
	 *   auto foo() -> Requires<ftl::has_gt<T>::value> {
	 *       // ...
	 *       if(some_t > other_t)
	 *       // ...
	 *   }
	 * \endcode
	 *
	 * \ingroup typetraits
	 */
	template<typename T>
	struct has_gt {
		static constexpr bool value =
			std::is_convertible<
				decltype(_dtl::test_gt<T>(nullptr)),
				bool
			>::value;
	};

	/**
	 * Test a type for `operator++()`.
	 *
	 * Example:
	 * \code
	 *   template<typename T>
	 *   auto foo() -> Requires<ftl::has_pre_inc<T>::value> {
	 *       // ...
	 *       ++some_t;
	 *   }
	 * \endcode
	 *
	 * \ingroup typetraits
	 */
	template<typename T>
	struct has_pre_inc {
	private:
		template<typename X>
		static auto check(X& x) -> decltype(++x);
		static _dtl::no check(...);

	public:
		static constexpr bool value =
			!std::is_same<_dtl::no, decltype(check(std::declval<T&>()))>::value;
	};

	/**
	 * Test a type for `operator++(int)`.
	 *
	 * Example:
	 * \code
	 *   template<typename T>
	 *   auto foo()
	 *   -> Requires<ftl::has_post_inc<T>::value> {
	 *       // ...
	 *       some_t++;
	 *   }
	 * \endcode
	 *
	 * \ingroup typetraits
	 */
	template<typename T>
	struct has_post_inc {
	private:
		template<typename X>
		static auto check(X& x) -> decltype(x++);
		static _dtl::no check(...);

	public:
		static constexpr bool value =
			!std::is_same<_dtl::no, decltype(check(std::declval<T&>()))>::value;
	};

	/**
	 * Test a type for `std::begin()` compatibility.
	 *
	 * Example:
	 * \code
	 *   template<
	 *   	typename T,
	 *   	typename = Requires<
	 *   	    ftl::has_begin<T>::value
	 *   	    && ftl::has_end<T>::value
	 *   	>
	 *   >
	 *   void foo() {
	 *       for(auto& e : some_t) {
	 *           // ...
	 *       }
	 *   }
	 * \endcode
	 *
	 * \ingroup typetraits
	 */
	template<typename T>
	struct has_begin {
		static constexpr bool value =
			!std::is_same<
				_dtl::no,
				decltype(_dtl::test_begin<T>(nullptr))
			>::value;
	};

	// TODO: C++14 - std::rbegin
	/**
	 * Test a type for `T::rbegin()` compatibility.
	 *
	 * Example:
	 * \code
	 *   template<
	 *   	typename T,
	 *   	typename = Requires<
	 *   	    ftl::has_rbegin<T>::value
	 *   	    && ftl::has_rend<T>::value
	 *   	>
	 *   >
	 *   void foo() {
	 *       for(auto it = some_t.rbegin(); it != some_t.rend(); ++it) {
	 *           // ...
	 *       }
	 *   }
	 * \endcode
	 *
	 * \ingroup typetraits
	 */
	template<typename T>
	struct has_rbegin {
		static constexpr bool value =
			!std::is_same<
				_dtl::no,
				decltype(_dtl::test_rbegin<T>(nullptr))
			>::value;
	};

	/**
	 * Test a type for `std::end()` compatibility.
	 *
	 * Example:
	 * \code
	 *   template<
	 *   	typename T,
	 *   	typename = Requires<
	 *   	    ftl::has_begin<T>::value
	 *   	    && ftl::has_end<T>::value
	 *   	>
	 *   >
	 *   void foo() {
	 *       for(auto& e : some_t) {
	 *           // ...
	 *       }
	 *   }
	 * \endcode
	 *
	 * \ingroup typetraits
	 */
	template<typename T>
	struct has_end {
		static constexpr bool value =
			!std::is_same<
				_dtl::no,
				decltype(_dtl::test_end<T>(nullptr))
			>::value;
	};

	/**
	 * Test a type for `T::rend()` compatibility.
	 *
	 * Example:
	 * \code
	 *   template<
	 *   	typename T,
	 *   	typename = Requires<
	 *   	    ftl::has_rbegin<T>::value
	 *   	    && ftl::has_rend<T>::value
	 *   	>
	 *   >
	 *   void foo() {
	 *       for(auto it = some_t.rbegin(); it != some_t.rend(); ++it) {
	 *           // ...
	 *       }
	 *   }
	 * \endcode
	 *
	 * \ingroup typetraits
	 */
	template<typename T>
	struct has_rend {
		static constexpr bool value =
			!std::is_same<
				_dtl::no,
				decltype(_dtl::test_rend<T>(nullptr))
			>::value;
	};

	template<typename T, typename U>
	struct has_push_back {
		static constexpr bool value =
			!std::is_same<
				_dtl::no,
				decltype(_dtl::test_push_back<T,U>(nullptr))
			>::value;
	};

	template<typename>
	class function;

	/**
	 * Checks if a certain type is a monomorphic function object.
	 *
	 * This check can be used to distinguish objects that have several
	 * `operator()` overloads from objects that have exactly one. Note that
	 * this check does not work on arbitrary user types; it must be specialised
	 * for each case.
	 *
	 * Built in specialisations include:
	 * - `std::function`
	 * - function pointers and pointers to member functions
	 * - `ftl::function`
	 *
	 * For everything else, `is_monomorphic::value` will be `false` by
	 * default.
	 *
	 * \ingroup typetraits
	 */
	template<typename>
	struct is_monomorphic : std::false_type {};

	template<typename R, typename...Args>
	struct is_monomorphic<std::function<R(Args...)>> : std::true_type {};

	template<typename R, typename...Args>
	struct is_monomorphic<ftl::function<R(Args...)>> : std::true_type {};

	template<typename R, typename...Args>
	struct is_monomorphic<R(*)(Args...)> : std::true_type {};

	template<typename C, typename R, typename...Args>
	struct is_monomorphic<R (C::*)(Args...)> : std::true_type {};

	template<typename F, typename...Args>
	struct is_callable {
	private:

		template<typename G, typename...Qs>
		static auto check(G g, Qs...qs) -> decltype(g(qs...));

		static _dtl::no check(...);

	public:
		static constexpr bool value =
			!std::is_same<
				_dtl::no,
				decltype(check(std::declval<F>(), std::declval<Args>()...))
			>::value;

		using type =
			decltype(check(std::declval<F>(), std::declval<Args>()...));
	};

	/**
	 * Determine various traits of functions and function objects.
	 *
	 * By default, tries to check the type of the function call operator in `F`,
	 * if such is defined. Also specialized for function pointers, references,
	 * and all relevant FTL function objects.
	 *
	 * Example:
	 * \code
	 *   void foo(int, char);
	 *
	 *   is_same<typename fn_traits<foo>::type, void(int,char)>::value == true
	 *   is_same<typename fn_traits<foo>::return_type, void>::value == true
	 *   is_same<typename fn_traits<foo>::argument_type<0>, int>::value == true
	 * \endcode
	 *
	 * \ingroup typetraits
	 **/
	template<class F>
	struct fn_traits : fn_traits<decltype(&F::operator())> {};

	template<class R, class...Args>
	struct fn_traits<R(Args...)>
	{
		typedef R (type) (Args...);

		using return_type = R;

		template<size_t I>
		using argument_type = ::std::tuple_element_t<I, ::std::tuple<Args...>>;
	};

	template<class R, class...Args>
	struct fn_traits<R(*)(Args...)> : fn_traits<R(Args...)> {};

	template<class R, class...Args>
	struct fn_traits<R(&)(Args...)> : fn_traits<R(Args...)> {};

	template<class C, class R, class...Args>
	struct fn_traits<R(C::*)(Args...)> : fn_traits<R(Args...)> {};

	template<class C, class R, class...Args>
	struct fn_traits<R(C::*)(Args...) const> : fn_traits<R(Args...)> {};

	template<class C, class R, class...Args>
	struct fn_traits<R(C::*)(Args...) volatile> : fn_traits<R(Args...)> {};

	template<class C, class R, class...Args>
	struct fn_traits<R(C::*)(Args...) const volatile> : fn_traits<R(Args...)> {};

	/**
	 * Convenience type alias for `fn_traits<F>::type`.
	 *
	 * \ingroup typetraits
	 **/
	template<class F>
	using fn_type = typename fn_traits<::std::remove_reference_t<F>>::type;

	/**
	 * Convenience type alias for `fn_traits<F>::argument_type`.
	 *
	 * \ingroup typetraits
	 **/
	template<class F, size_t I>
	using argument_type =
		typename fn_traits<::std::remove_reference_t<F>>::template argument_type<I>;

	/**
	 * Check that a variadic list of compile time predicates all hold.
	 *
	 * \tparam Ps must contain a static, compile time member named `value`, of
	 *            type `bool` (or be convertible to it).
	 *
	 * \par Examples
	 *
	 * Statically enforce equality comparability on a variadic type list:
	 * \code
	 *   template<typename...Ts>
	 *   void example() {
	 *       static_assert(conjunction<Eq<Ts>...>::value, "All types in Ts must satisfy Eq");
	 *   }
	 * \endcode
	 *
	 * \see conjunction_v, disjunction, disjunction_v
	 *
	 * \ingroup type_traits
	 */
	template<class...Ps>
	struct conjunction;

	// TODO: c++17 should have a built-in conjunction

	template<>
	struct conjunction<> : ::std::true_type {};

	template<class P, class...Ps>
	struct conjunction<P, Ps...>
		: if_<P::value, conjunction<Ps...>, P> {};

	/**
	 * Static convenience instance of `conjunction`.
	 *
	 * \see conjunction, disjunction, disjunction_v
	 *
	 * \ingroup type_traits
	 */
	template<class...Ps>
	static constexpr bool conjunction_v = conjunction<Ps...>::value;

	/**
	 * Check that at least one of a variadic list of compile time predicates hold.
	 *
	 * \tparam Ps must contain a static, compile time member named `value`, of
	 *            type `bool` (or be convertible to it).
	 *
	 * \par Examples
	 *
	 * Statically enforce equality comparability on at least one type
	 * \code
	 *   template<typename...Ts>
	 *   void example() {
	 *       static_assert(disjunction<Eq<Ts>...>::value, "One of Ts must satisfy Eq");
	 *   }
	 * \endcode
	 *
	 * \see conjunction_v, disjunction, disjunction_v
	 *
	 * \ingroup type_traits
	 */
	template<class...Ps>
	struct disjunction;

	template<>
	struct disjunction<> : ::std::false_type {};

	template<class P, class...Ps>
	struct disjunction<P, Ps...>
		: if_<P::value, P, disjunction<Ps...>> {};

	/**
	 * Static convenience instance of `disjunction`.
	 *
	 * \see conjunction, conjunction_v, disjunction
	 *
	 * \ingroup type_traits
	 */
	template<class...Ps>
	static constexpr bool disjunction_v = disjunction<Ps...>::value;
}

#endif

