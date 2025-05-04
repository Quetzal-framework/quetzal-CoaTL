// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#ifndef __POPULATION_FLOW_H_INCLUDED__
#define __POPULATION_FLOW_H_INCLUDED__

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream> // output operator
#include <unordered_map>

namespace quetzal
{
namespace demography
{
/*!
 * \brief Defines a template class to store the demographic flows number across a landscape along time.
 *
 * \tparam Space    Demes identifiers.
 * \tparam Time     EqualityComparable, CopyConstructible.
 * \tparam Value    The type of the population size variable (int, unsigned int, double...)
 * \ingroup demography
 */
template <typename Space, typename Time, typename Value> class FlowHashMapImplementation
{
  public:
    //! \typedef time type
    using time_type = Time;
    //! \typedef space type
    using coord_type = Space;
    //! \typedef type of the population size variable
    using value_type = Value;
    /**
     * \brief Default constructor
     */
    FlowHashMapImplementation() = default;
    /**
     * \brief Retrieves value of the flow from deme i to deme j at time t.
     */
    value_type flow_from_to(coord_type const &from, coord_type const &to, time_type t) const
    {
        assert(m_flows.find(key_type(t, from, to)) != m_flows.end());
        return m_flows.at(key_type(t, from, to));
    }
    /**
     * \brief Retrieves value of the flow from deme i to deme j at time t.
     * \return a reference on the value, initialized with value_type default constructor
     */
    void set_flow_from_to(coord_type const &from, coord_type const &to, time_type t, value_type v)
    {
        m_flows[key_type(t, from, to)] = v;
        m_reverse_flows[reverse_key_type(to, t)][from] = v;
    }
    /**
     * \brief Adds value v to the flow from deme i to deme j at time t.
     * \return a reference on the value, initialized with value_type default constructor
     */
    void add_to_flow_from_to(coord_type const &from, coord_type const &to, time_type t, value_type v)
    {
        m_flows[key_type(t, from, to)] += v;
        m_reverse_flows[reverse_key_type(to, t)][from] += v;
    }
    /**
     * \brief Retrieves the distribution of the value of the flow converging to deme x at time t.
     * \return a const reference on an unordered_map with giving the origin deme and the value
     *         giving the value of the flow
     */
    std::unordered_map<coord_type, value_type> const &flow_to(coord_type const &x, time_type t) const
    {
        assert(flow_to_is_defined(x, t));
        return m_reverse_flows.at(reverse_key_type(x, t));
    }
    /**
     * \brief Check if the distribution of the value of the flow converging to deme x at time t is defined
     * \return true if the distribution is defined, else returns false
     */
    bool flow_to_is_defined(coord_type const &to, time_type const &t) const
    {
        auto it = m_reverse_flows.find(reverse_key_type(to, t));
        return it != m_reverse_flows.end();
    }
    /**
     * \brief A class representing the spatio-temporal coordinates of the flow vector (time, origin and destination)
     */
    struct key_type
    {
        //! time of the dispersal event
        time_type time;
        //! origin of the flow
        coord_type from;
        //! destination of the flow
        coord_type to;
        //! Constructor
        key_type(time_type const &t, coord_type const &origin, coord_type const &destination)
            : time(t), from(origin), to(destination)
        {
        }
        //! EqualityComparable
        bool operator==(key_type const &other) const
        {
            return (other.time == this->time && other.from == this->from && other.to == this->to);
        }
    }; // end struct Flow::key_type
    /**
     * \brief Read-only access to the migration history.
     * \return An iterator on an object of type map_type
     */
    auto begin() const
    {
        return m_flows.cbegin();
    }
    /**
     * \brief Read-only access to the migration history.
     * \return An iterator on an object of type map_type
     */
    auto end() const
    {
        return m_flows.cend();
    }

  private:
    /**
     * \brief A class representing the spatio-temporal coordinates of the reverse flow vector (time, origin and
     * destination)
     */
    struct reverse_key_type
    {
        //! time of the dispersal event
        time_type time;
        //! destination of the flow
        coord_type to;
        //! Constructor
        reverse_key_type(coord_type const &x, time_type const &t) : time(t), to(x)
        {
        }
        //! EqualityComparable
        bool operator==(reverse_key_type const &other) const
        {
            return (other.time == this->time && other.to == this->to);
        }
    }; // end Flow::reverse_key_type
    /**
     * \brief Makes reverse_key_type hashable (can be stored in hash maps)
     */
    struct reverse_key_hash
    {
        std::size_t operator()(const reverse_key_type &k) const
        {
            size_t res = 17;
            res = res * 31 + std::hash<time_type>()(k.time);
            res = res * 31 + std::hash<coord_type>()(k.to);
            return res;
        }
    }; // end Flow::reverse_key_hash
    /**
     * \brief Makes key_type hashable (can be stored in hash maps)
     */
    struct key_hash
    {
        std::size_t operator()(const key_type &k) const
        {
            size_t res = 17;
            res = res * 31 + std::hash<time_type>()(k.time);
            res = res * 31 + std::hash<coord_type>()(k.from);
            res = res * 31 + std::hash<coord_type>()(k.to);
            return res;
        }
    }; // end key_hash
  public:
    //! \typedef type of the historical database
    using map_type = std::unordered_map<const key_type, value_type, key_hash>;

  private:
    //! \typedef type of the reverse-time historical database
    using reverse_flow_type =
        std::unordered_map<const reverse_key_type, std::unordered_map<coord_type, value_type>, reverse_key_hash>;
    //! Time forward historical database
    map_type m_flows;
    //! Reverse-time historical database
    reverse_flow_type m_reverse_flows;
}; // end class Flow
//! Stream operator
template <typename Space, typename Time, typename Value>
std::ostream &operator<<(std::ostream &stream, const FlowHashMapImplementation<Space, Time, Value> &flows)
{
    stream << "time"
           << "\t"
           << "from"
           << "\t"
           << "to"
           << "\t"
           << "flow"
           << "\n";
    for (auto const &it : flows)
    {
        stream << it.first.time << "\t" << it.first.from << "\t" << it.first.to << "\t" << it.second << "\n";
    }
    return stream;
} // end ostream operator
} // namespace demography
} // namespace quetzal
#endif
