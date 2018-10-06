#define BOOST_TEST_MODULE integration_tests
#include <boost/test/included/unit_test.hpp>
#include <quetzal.h>
#include <boost/math/special_functions/binomial.hpp> // binomial coefficient

BOOST_AUTO_TEST_CASE( xorigin_simulation_framework )
/* Compare with void free_test_function() */
{
  using time_type = int;
  using landscape_type = quetzal::geography::DiscreteLandscape<std::string,time_type>;
  using coord_type = landscape_type::coord_type;

  landscape_type env( {{"suitability", "../test/test_data/europe_precipitation.tif"}},
  {1000} );

  // some test
  auto space = env.geographic_definition_space();

  // initializing the iddc simulator
  using quetzal::demography::strategy::mass_based;

  // problem: we did not verify, but 40-10 was a see cell !
  coord_type x_0(40.0, -3.0);
  x_0 = env.reproject_to_centroid(x_0);
  time_type t_0 = 1000;
  mass_based::value_type N_0 = 1;

  using simulator_type = quetzal::simulators::SpatiallyExplicitCoalescenceSimulator<coord_type, time_type, mass_based>;
	using wright_fisher_model = quetzal::simulators::DiscreteTimeWrightFisher;

  simulator_type simulator(x_0, t_0, N_0);

  // Niche functions and growth functions
  using quetzal::expressive::literal_factory;
  using quetzal::expressive::use;
  literal_factory<coord_type, time_type> lit;

  auto sref = env["suitability"];
  auto s = [sref](coord_type x, time_type){return sref(x, 1000);};
  auto K = use(s) * lit(1000);

  auto r = lit(100);
  auto pop_sizes = simulator.pop_size_history();
  auto N = use( [pop_sizes](coord_type x, time_type t){ return pop_sizes(x,t);} );

  auto g = N * ( lit(1) + r ) / ( lit(1) + ( (r * N)/K ));
  auto sim_children = [g](auto& gen, coord_type const&x, time_type t){
    std::poisson_distribution<unsigned int> poisson(g(x,t));
    return poisson(gen);
  };

  std::random_device rd;
  std::mt19937 gen(rd());

  // Dispersal
  auto transition_matrix = quetzal::make_neighbor_migration(env, K);

  unsigned int t_sampling = 2000;
  simulator.expand_demography(t_sampling, sim_children, transition_matrix, gen);

  // Coalescence setting
  using mutation_model = quetzal::mutation_kernel::standard_data;
  using cell_type = typename mutation_model::cell_type;
  using tree_type = quetzal::coalescence::Tree<cell_type>;
  using forest_type = quetzal::coalescence::Forest<coord_type, tree_type>;
  forest_type forest;

	// Replace this code with SNP dataset specific code
  coord_type s1(40.0, -3.);
  s1 = env.reproject_to_centroid(s1);
  coord_type s2(45.0, 2.35);
  s2 = env.reproject_to_centroid(s2);

  forest.insert(s1, std::vector<tree_type>(100, tree_type(t_sampling)));

  auto branch = []( auto& parent , auto const& child ){
    return parent.add_child(child);
  };

  auto new_forest = simulator.coalesce_along_history(forest, branch, gen, [](coord_type, time_type const& t){return tree_type(t);});

	time_type t_curr = t_0;
	auto init = [&t_curr](time_type const& t)
	{
		t_curr -= t;
		return tree_type(t_curr);
	};
	auto tree = wright_fisher_model::coalesce(new_forest, 10000, gen, branch, init);

	// substitution rate
	double mu = 0.001;
	tree.cell().allelic_state();

	auto visitor =  [mu, &gen](auto & node){
		if(node.has_parent()){
			unsigned int t = std::abs(node.cell().time() - node.parent().cell().time());
			std::poisson_distribution<unsigned int> dist(mu*t);
			unsigned int nb_mutations = dist(gen);
			node.cell().allelic_state( mutation_model::mute(node.parent().cell().allelic_state(), nb_mutations, gen) );
		}

	};

	tree.visit_subtrees_by_pre_order_DFS(visitor);

}
