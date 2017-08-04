
//               math::expressive library
//          Copyright Pierre Marques 2016 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "expressive.h"

#include <iostream>
#include <string>
#include <type_traits>

using namespace math::expressive;
using std::cout;
using std::endl;


namespace test {
/*constexpr indicates that the value, or return value, is constant and, if possible, will be computed at compile time.
A constexpr integral value can be used wherever a const integer is required, such as in template arguments 
and array declarations.And when a value can be computed at compile time instead of run time, 
it can help your program can run faster and use less memory.
*/

template <typename T = int>
constexpr T add(T const& a, T const& b) {return a+b;}

template <typename T = int>
constexpr T duplicate(T const& t) {return t*2;}

template <typename T = int>
constexpr T identity(T const& t) {return t;}

template<typename T>
constexpr
void literal_check() {
	constexpr auto arg = 3;
	static_assert( use(identity<T>)(arg)==arg, "use identity failed");
	
	static_assert( (void(use(add<T>)), true), "use add failed");
	static_assert( (void(use(add<T>)(arg, arg)), true), "call use(add) failed");
	static_assert( (void( add<T>(arg, arg) ), true), "straight add call failed");
	static_assert( use(add<T>)(arg, arg)==add<T>(arg, arg), "use straight call failed");
	
    static_assert(literal_factory<T>{}(0)(2*arg)==0, "literal failure");
}


template <typename T>
constexpr
void constexpr_check() {
	constexpr auto arg = 3;
	constexpr auto f = expression(duplicate<T>);
	constexpr auto g = expression(identity<T>);
	
	static_assert((f+g)(arg) == f(arg)+g(arg), "constexpr sum failure");
	static_assert((f-g)(arg) == f(arg)-g(arg), "constexpr sub failure");
	static_assert((f*g)(arg) == f(arg)*g(arg), "constexpr mul failure");
	static_assert((f/g)(arg) == f(arg)/g(arg), "constexpr div failure");
	
	
	static_assert((f >> g)(arg) == g(f(arg)), "constexpr chaining failure");
	static_assert((f << g)(arg) == f(g(arg)), "constexpr back chaining failure");
}

//les memes tests que constexpr_check, mais avec des lambdas
template <typename T>
void lambda_check() {
	constexpr auto arg = 3;
    const auto f = [](T const& t) {return t * 2;};
    const auto g = [](T const& t) {return t;};

	if ( (f+g)(arg) != f(arg)+g(arg) ) cout << "+ failed" << endl;
	if ( (f-g)(arg) != f(arg)-g(arg) ) cout << "- failed" << endl;
	if ( (f*g)(arg) != f(arg)*g(arg) ) cout << "* failed" << endl;
	if ( (f/g)(arg) != f(arg)/g(arg) ) cout << "/ failed" << endl;

	if ((f >> g)(arg) != g(f(arg)) ) cout << "chaining failed" << endl;
	if ((f << g)(arg) != f(g(arg)) ) cout << "back chaining failed" << endl;
}

template <typename T>
void constructions() {
	literal_factory<T, unsigned int> l;
	
	constexpr T arg1{7};
	constexpr unsigned int arg2{3u};
	
	auto a = [](T x, unsigned int t) { return x*t; };
	auto b = [](T x, unsigned int) { return x/2; };
	
	auto expression = ((a-b)/l(T{8}));

	if (expression(T{7}, 3u) != (a(arg1, arg2)-b(arg1, arg2))/T{8}   ) cout << "complex expression failure" << endl;
}

template <typename T>
void check() {
	literal_check<T>();
	constexpr_check<T>();
	lambda_check<T>();
	constructions<T>();
}

}//test::



int main() {
	test::check<int>();
	test::check<long>();
	test::check<unsigned char>();
	test::check<double>();
	{
		constexpr literal_factory<double, int> lit;
	    static_assert(lit(1u)(7.,12)==1u, "complex literal failure");
    	static_assert(lit(1u)(7,12.)==1u, "argument casting literal failure");
	}
	{
		literal_factory<std::string> l;
		cout << "string operation: " << (l("hello ") + [](std::string a){return a;}) ("world") << endl;
	}
	{
		auto f1 = [](int){return std::string{"coin coin"};};
		auto f2 = [](std::string s){return s.size();};
		auto f3 = [](std::string::size_type i){return i*2;};
		
		auto alpha = f1 >> f2;

		cout << "chaining and sum:" << (alpha+[](int i){return i;})(1) << endl;
		cout << "triple composition:" << (f1 >> f2 >> f3)(1) << endl;
		
		constexpr auto id = test::identity<int>;
		constexpr auto dupl = test::duplicate<int>;
		constexpr auto addi = test::add<int>;
		
		auto gamma = (expression(id) >> dupl) + compose(addi, id, id);

		constexpr literal_factory<int> lit;
		
		auto sigma = [](int a, int b) {return a+b;};

		cout << "wide composing: " << (
			-gamma >> compose(sigma, f1>>f2, lit(1)>>f3) >> f3
		) (1) << endl;
	}
}
