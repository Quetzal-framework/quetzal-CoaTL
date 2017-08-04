
//               math::expressive library
//          Copyright Pierre Marques 2016 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

/*
ce code compile avec g++ 4.8.2
g++ -std=c++1y -O2 -Wall -Wextra maquette.cpp -o test
*/
#include "expressive.h"

#include <iostream>

#include <functional>
#include <random>
#include <stdexcept>

#include <deque>

#include "utils_map_access.h"

namespace ex = math::expressive;

//Env doit être un foncteur de PropertiesxR²xT -> Values
//il modélise les données environnementales


template < typename Value, typename Space, typename Time >
struct generator {
	using functor_type = std::function<Value(Space, Time)>;
	
	functor_type source;
	std::function<void(std::ostream&)> show;
	
	//constructeur pour les générateurs de base
	generator(std::string const& name, functor_type source):
		source(source), show([name](std::ostream& os){os<<name<< "(x,t)";})
	{}
	
	generator(functor_type source, std::string const& name):
		source(source), show([name](std::ostream& os){os<<name<< "(x,t)";})
	{}
	
	//constructeur pour les combinateurs
	// template <typename V>
	// generator(
		// typename std::enable_if<std::is_assignable<Value, V>::value, V>::type v
	// ): source([v](Space, Time){return v;}), show([v](std::ostream& os){os<<v;}) {}
	
	template <typename F, typename std::enable_if<std::is_assignable<functor_type, F>::value, void>* = nullptr>
	generator(F f): source(f), show([f](std::ostream& os) {os<<f;}) {}
	
	Value operator()(Space x, Time t) const {return source(x,t);}
	
	explicit operator functor_type () const {return source;}
};


template < typename Value, typename Space, typename Time >
inline std::ostream& operator <<(std::ostream& os, generator<Value, Space, Time> const& g) {
	g.show(os); return os;
}


	
template <typename Value = double, typename Space = unsigned int, typename Time = unsigned int>
struct Env {
private:
	using data_functor = generator<Value, Space, Time>;
	
	using data_type = std::vector<data_functor>;
	data_type data;

public:
	Env(std::initializer_list<data_functor> bindings): data(bindings) {}
	Env() {}

	Env& operator << (data_functor binding) {data.push_back(binding); return *this;}
	
	auto size() const {return data.size();}
	
	//hopefully, id is in [ 0, size() )
	auto const& operator[](typename data_type::size_type id) const {return data[id];}
};

//cette classe crée des foncteurs
template <typename Value, typename Space, typename Time>
class FunctorFactory {
private:
	using candidates_type = std::deque<generator<Value, Space, Time>>;
	
	candidates_type candidates;
	mutable std::default_random_engine engine;
    mutable std::uniform_int_distribution<typename candidates_type::size_type> selection_distribution;
	
public:
	FunctorFactory(Env<Value, Space, Time> const& e):
		engine(std::random_device()())
	{
		using namespace ex;
		literal_factory<Space, Time> lit;
		
		switch (e.size()) {
			case 0: throw std::invalid_argument("FunctorFactory requires a non empty Env.");
			case 2:
				candidates.emplace_back(e[1]);
				candidates.emplace_back(e[0]+e[1]);
				candidates.emplace_back(e[0]-e[1]);
				candidates.emplace_back(e[1]-lit(2)*e[0]);
				candidates.emplace_back(e[0]*e[1]);
				candidates.emplace_back( e[0]*e[1] - e[1]+e[0] );

			case 1:
				candidates.emplace_back(e[0]);
				candidates.emplace_back(lit(1)/e[0]);
				candidates.emplace_back(-e[0]);
		}
	}
	
	auto const& operator()() const {
		selection_distribution = std::uniform_int_distribution<typename candidates_type::size_type>{0, candidates.size()-1};
		return candidates[ selection_distribution(engine) ];
	}
};

template <typename Value, typename Coord, typename Time>
auto make_functor_factory(Env<Value, Coord, Time> const& e) {return FunctorFactory<Value, Coord, Time>(e);}

using namespace std;
int main() {
	using value_type = double;
	using coord_type = int;
	using time_type = unsigned int;
	
	//par la suite, on utilisera le chargement des fichiers de données.
	//Pour l'instant, c'est une décroissance (en 1/t) des constantes utilisées à la création. Les coordonnées spatiales ne sont pas utilisées
	Env<value_type, coord_type, time_type> environnement{
		{"rain", [](coord_type, time_type t) -> value_type {return t>0 ? 1. / (1+t) : 0.;} },
		{"coolness", [](coord_type, time_type t) -> value_type {return t>0 ? 10. / (1+t) : 0.;} }
	};
	
	auto factory = make_functor_factory(environnement);
	
	auto R = factory();
	auto K = factory();

	// la formule magique. ici N(t+1) = N(t) * (1+R) / (1+(R*K))
	auto formule = [&R, &K](double N, coord_type x, time_type t) {
		return N *(1+R(x, t)) / (1+R(x, t)*K(x, t));
	};
	
	cout << "formule: N(x,t+1) = N * (1+R(x,t)) / (1+R(x,t)*K(x,t))\n" << endl;
	
	
	cout << "R(x,t)=" << R << endl;
	cout << "K(x,t)=" << K << endl;
	
	cout << "\nsimulation:" << endl;
	// situation initiale
	double N = 1;
	// évolution (et affichage)
	for (time_type t=0; t<10; ++t, N = formule(N, 0, t)) {
		cout<< "R("<<t<<") = " << R(0, t) << '\t'
		    << "K("<<t<<") = " << K(0, t) << '\t'
		    << "N("<<t<<") = "<< N
		 << endl;
	}
}
