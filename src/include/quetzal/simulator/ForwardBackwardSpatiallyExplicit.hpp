// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#ifndef __SPATIALLY_EXPLICIT_H_INCLUDED__
#define __SPATIALLY_EXPLICIT_H_INCLUDED__

#include "../coalescence/container/Forest.hpp"
#include "../coalescence/merger_policy.hpp"
#include "../coalescence/occupancy_spectrum/sampling_policy.hpp"
#include "../demography/History.hpp"

#include <boost/math/special_functions/binomial.hpp>
#include <map>

namespace quetzal::simulator
{
/**
 * @brief Discrete-time coalescence simulator in a discrete spatially explicit landscape.
 *
 * @details This simulator consists in a forward demographic simulation of population
 *          growth and migration, after what a backward coalescent simulation is performed
 *          that traces back the ancestry of a sample of gene copies sampled in one
 *          or more populations. Most Recent Common ancestor is not always fond in the
 *          time span of the spatial history, so if the chosen CoalescencePolicy implements
 *          the required behavior, ancestry can be simulated in the more distant past using another model
 *          like the DiscreteTimeWrightFisher model, to find the MRCA.
 *          Details concerning the mergers (binary mergers, simultaneous multiple mergers)
 *          can be controlled using the relevant Merger policy class when calling a
 *          coalescence function of the class interface.
 * @tparam Space deme identifiers (like the populations geographic coordinates)
 * @tparam Strategy a template class argument (e.g. mass_based, individual_based)
 *                  that indicates how dispersal should be defined in terms of
 *                  demographic expansion algorithms. Strategy class
 *                  implements the details (computing migrations probabilites as a function of the
 *                  distance for example) whereas template specialization of the
 *                  internal history_type class defines different migration algorithms.
 * @tparam CoalescencePolicy a policy class for the demographic expansion.
 *
 * @ingroup simulator
 *
 */
template <typename Space, typename DispersalPolicy, typename CoalescencePolicy,
          typename Memory = quetzal::demography::memory_policy::on_RAM>
class ForwardBackwardSpatiallyExplicit : public CoalescencePolicy
{
  public:
    //! \typedef space type
    using coord_type = Space;
    //! \typedef strategy type
    using dispersal_policy_type = DispersalPolicy;
    //! \typedef value type to represent populations size
    using N_value_type = typename DispersalPolicy::value_type;
    //! \typedef Memory policy type
    using memory_policy = Memory;
    //! \typedef forest type
    template <typename tree_type> using forest_type = quetzal::coalescence::container::Forest<coord_type, tree_type>;

  private:
    using history_type = demography::History<coord_type, DispersalPolicy, memory_policy>;
    history_type m_history;

  public:
    /**
     * @brief Constructor
     *
     * @param x_0 Initialization coordinate.
     * @param N_0 Population size at intialization
     * @param nb_generations number of generations of the spatial process
     */
    ForwardBackwardSpatiallyExplicit(coord_type x_0, N_value_type N_0, unsigned int nb_generations)
        : m_history(x_0, N_0, nb_generations)
    {
    }
    /*!
       \brief Return demes with non-zero population size
    */
    auto distribution_area(unsigned int t) const
    {
        assert(t >= 0 && t < m_history.nb_generations());
        return m_history.distribution_area(t);
    }
    /**
     * @brief Read-only access to the population size history.
     * @details Designed to be used for composing growth expression.
     * @return a functor with signature `N_value_type fun(coord_type const& x,
     * unsigned int t)` giving the population size in deme \f$x\f$ at time \f$t\f$.
     */
    auto get_functor_N() const noexcept
    {
        return m_history.get_functor_N();
    }
    /**
     * @brief Simulate the forward-in-time demographic expansion.
     * \tparam Growth a growth functor, see param growth for details.
     * \tparam Dispersal a dispersal kernel, see param kernel for details.
     * @param growth a functor simulating \f$\tilde{N}_{x}^{t}\f$. The functor
     *                   can possibly make use of get_functor_N. The signature
     *                   of the function should be equivalent to the following:
     *                   `unsigned int sim_growth(V &gen, const coord_type &x, unsigned int t);`
     * @param kernel a functor representing the dispersal location kernel that
     *               simulates the coordinate of the next location conditionally
     *               to the current location \f$x\f$. The signature should be equivalent
     *               to `coord_type kernel(V &gen, const coord_type &x, unsigned int t);`
     */
    template <typename Generator, typename Growth, typename Dispersal>
    void simulate_forward_demography(Growth growth, Dispersal kernel, Generator &gen)
    {
        m_history.simulate_forward(growth, kernel, gen);
    }
    /**
     * @brief Initlize a forest based on sample counts then coalesce the forest along the spatial
     *        history, then stops and return an incomplete forest.
     * @return A possibly incompletely coalesced forest.
     */
    template <typename Merger, typename Generator>
    auto make_forest_and_coalesce_along_spatial_history(std::map<coord_type, unsigned int> sample, Generator &gen)
    {
        test_sample_consistency(sample);
        auto forest = this->make_forest(sample);
        return coalesce_along_spatial_history<Merger>(forest, this->branch(), gen, this->init());
    }
    /**
     * @brief Initialize a forest based on sample counts, forwarding the sampling time
     *        to the forest contructor. Then coalesce the forest along the spatial
     *        history, then stops and return a possibly incomplete forest.
     * @return A possibly incompletely coalesced forest.
     */
    template <typename Merger, typename Generator>
    auto make_forest_using_sampling_time_and_coalesce_along_spatial_history(std::map<coord_type, unsigned int> sample,
                                                                            unsigned int sampling_time, Generator &gen)
    {
        test_sample_consistency(sample);
        auto forest = this->make_forest(sample, sampling_time);
        return coalesce_along_spatial_history<Merger>(forest, this->branch(), gen, this->init());
    }

    /**
     * @brief Coalesce a forest of nodes conditionally to the simulated demography.
     *
     */
    template <typename Merger, typename Generator, typename F, typename Tree, typename U>
    auto coalesce_along_spatial_history(forest_type<Tree> forest, F binary_op, Generator &gen, U make_tree)
    {
        this->check_consistency(m_history, forest);
        auto t = m_history.nb_generations();
        while ((forest.nb_trees() > 1) && (t > 0))
        {
            may_coalesce_colocated<Merger>(forest, t, gen, binary_op, make_tree);
            migrate_backward(forest, t, gen);
            --t;
        }
        may_coalesce_colocated<Merger>(forest, t, gen, binary_op, make_tree);
        return forest;
    }
    /**
     * @brief Create a forest from a sample and coalesce it conditionally to the simulated demography.
     *
     * @tparam Merger a coalescence merger to use
     * @tparam Generator a random number Generator
     *
     * @param sample the number of gene copies at each location
     * @param sampling_time the time of sampling_time to build the make_forest coalescence policy function
     * @gen a random number generator
     *
     */
    template <typename Merger = quetzal::coalescence::merger_policy::SimultaneousMultipleMerger<
                  quetzal::coalescence::occupancy_spectrum::sampling_policy::on_the_fly>,
              typename Generator>
    auto coalesce_to_mrca(std::map<coord_type, unsigned int> sample, unsigned int sampling_time, Generator &gen)
    {
        test_sample_consistency(sample);
        auto forest = this->make_forest(sample, sampling_time);
        auto new_forest = coalesce_along_spatial_history<Merger>(forest, this->branch(), gen, this->init());
        auto tree = this->find_mrca(new_forest, 0, gen);
        return this->treat(tree);
    }

    /**
     * @brief Create a forest from a sample and coalesce it conditionally to the simulated demography.
     *
     * @tparam Merger a coalescence merger to use
     * @tparam Generator a random number Generator
     *
     * @param sample the number of gene copies at each location
     * @gen a random number generator
     *
     */
    template <typename Merger = quetzal::coalescence::merger_policy::SimultaneousMultipleMerger<
                  quetzal::coalescence::occupancy_spectrum::sampling_policy::on_the_fly>,
              typename Generator>
    auto coalesce_to_mrca(std::map<coord_type, unsigned int> sample, Generator &gen)
    {
        test_sample_consistency(sample);
        auto forest = this->make_forest(sample);
        auto new_forest = coalesce_along_spatial_history<Merger>(forest, this->branch(), gen, this->init());
        auto tree = this->find_mrca(new_forest, 0, gen);
        return this->treat(tree);
    }

    /**
     * @brief Create a forest from a sample giving name to tips, then coalesce them conditionally to the simulated
     * demography.
     *
     * @tparam Generator a random number Generator
     * @tparam F a unary operation with signature equivalent to 'std::string fun(coord_type const& x,
     * unsigned int t)'
     *
     * @param sample the number of gene copies at each location
     * @param sampling_time the time of sampling_time
     * @param leaf_name a functor assigning a name to a tip as a function of its location and of sampling time.
     * @gen a random number generator
     *
     */
    template <typename Merger = quetzal::coalescence::merger_policy::SimultaneousMultipleMerger<
                  quetzal::coalescence::occupancy_spectrum::sampling_policy::on_the_fly>,
              typename Generator, typename F>
    auto coalesce_to_mrca(std::map<coord_type, unsigned int> sample, unsigned int sampling_time, F leaf_name,
                          Generator &gen)
    {
        test_sample_consistency(sample);
        auto forest = this->make_forest(sample, sampling_time, leaf_name);
        auto new_forest = coalesce_along_spatial_history<Merger>(forest, this->branch(), gen, this->init());
        auto tree = this->find_mrca(new_forest, 0, gen);
        return this->treat(tree);
    }

    /**
     * @brief Create a forest from a sample then coalesce them conditionally to the simulated demography.
     *
     * @tparam T type of an individual
     * @tparam F1 a unary operation with signature equivalent to 'coord_type fun(const T& i)'
     * @tparam F2 a unary operation with signature equivalent to 'std::string fun(const T& i)'
     * @tparam Generator a random number Generator
     *
     * @param sample a collection of individuals (gene copies)
     * @param sampling_time the time of sampling_time
     * @param get_location a unary operation taking a sampled individual of type T as an argument and returning its
     * geographic location
     * @param get_name a functor taking a sampled individual of type T as an argument and returning an identifier
     * @gen a random number generator
     *
     */
    template <typename Merger = quetzal::coalescence::merger_policy::SimultaneousMultipleMerger<
                  quetzal::coalescence::occupancy_spectrum::sampling_policy::on_the_fly>,
              typename T, typename F1, typename F2, typename Generator>
    auto coalesce_to_mrca(std::vector<T> sample, unsigned int sampling_time, F1 get_location, F2 get_name,
                          Generator &gen)
    {
        auto forest = this->make_forest(sample, sampling_time, get_location, get_name);
        auto new_forest = coalesce_along_spatial_history<Merger>(forest, this->branch(), gen, this->init());
        auto tree = this->find_mrca(new_forest, 0, gen);
        return this->treat(tree);
    }

  private:
    template <typename T, typename U> void check_consistency(T &history, U const &forest)
    {
        auto t = history.nb_generations();
        for (auto const &it : forest.positions())
        {
            if (history.pop_size(it, t) < forest.nb_trees(it))
            {
                throw std::domain_error("Simulated population size inferior to sampling size");
            }
        }
    }

    void test_sample_consistency(std::map<coord_type, unsigned int> const &sample) const
    {
        if (sample.size() == 0)
        {
            throw std::logic_error("Sample to coalesce is empty");
        }
        else if (sample.size() == 1)
        {
            if (sample.begin()->second < 2)
            {
                throw std::logic_error("Sample to coalesce only contains one gene copy");
            }
        }
    }

    template <typename Generator, typename Forest>
    void migrate_backward(Forest &forest, unsigned int t, Generator &gen) const noexcept
    {
        Forest new_forest;
        for (auto const it : forest)
        {
            coord_type x = it.first;
            new_forest.insert(m_history.backward_kernel(x, t, gen), it.second);
        }
        assert(forest.nb_trees() == new_forest.nb_trees());
        forest = new_forest;
    }

    template <typename Merger, typename Generator, typename Tree, typename F, typename U>
    void may_coalesce_colocated(forest_type<Tree> &forest, unsigned int t, Generator &gen, F binop,
                                U make_tree) const noexcept
    {
        auto N = get_functor_N();
        for (auto const &x : forest.positions())
        {
            auto range = forest.trees_at_same_position(x);
            std::vector<Tree> v;
            for (auto it = range.first; it != range.second; ++it)
            {
                v.push_back(it->second);
            }
            if (v.size() >= 2)
            {
                assert(N(x, t) >= 1 && "Population size should be positive for evaluating coalescence probability");
                auto last = Merger::merge(v.begin(), v.end(), N(x, t), make_tree(x, t), binop, gen);
                forest.erase(x);
                for (auto it = v.begin(); it != last; ++it)
                {
                    forest.insert(x, *it);
                }
            }
        }
    }
}; // end ForwardBackwardSpatiallyExplicit
} // namespace quetzal::simulator

#endif
