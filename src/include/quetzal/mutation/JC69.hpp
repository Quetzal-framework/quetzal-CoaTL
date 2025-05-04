// Copyright 2021 Arnaud Becheler    <abechele@umich.edu>

/*********************************************************************** * This program is free software; you can
 *redistribute it and/or modify * it under the terms of the GNU General Public License as published by * the Free
 *Software Foundation; either version 2 of the License, or    * (at your option) any later version. *
 *                                                                      *
 ***************************************************************************/

#ifndef __JC69_H_INCLUDED__
#define __JC69_H_INCLUDED__

namespace quetzal::mutation::JC69
{

class cell_JC69
{
  private:
    int m_t = 0;
    char m_allelic_state;

  public:
    cell_JC69(int t) : m_t(t)
    {
    }
    cell_JC69() = default;
    int time() const
    {
        return m_t;
    }
    char allelic_state() const
    {
        return m_allelic_state;
    }
    auto &allelic_state(char a)
    {
        m_allelic_state = a;
        return *this;
    }
};

/* @brief Mute allelic states in the Arlequin standard data format.
 */
class standard_data
{
  public:
    // @typedef typde of the mutational state
    using state_type = bool;
    class cell_type
    {
      private:
        int m_time = 0;
        state_type m_allelic_state = false;

      public:
        cell_type(int t) : m_time(t)
        {
        }
        cell_type() = default;
        int time() const
        {
            return m_time;
        }
        state_type allelic_state() const
        {
            return m_allelic_state;
        }
        auto &allelic_state(state_type a)
        {
            m_allelic_state = a;
            return *this;
        }
    };
    /* @brief Sample a new mutational state
     * @param state the initial mutational state
     * @param nb_mutations the number of mutational event to simulate
     * @return the updated mutational state
     */
    template <typename UnusedRNG> static bool mute(state_type state, unsigned int nb_mutations, UnusedRNG &)
    {
        if (nb_mutations % 2 != 0)
        {
            state = !state;
        }
        return state;
    }
};

/* @brief JC69 mutational kernel
 */
class JC69
{
    static const std::map<char, std::string> dico;

  public:
    // @typedef allelic state type
    using state_type = char;
    /*
     * @brief Sample a new mutational state
     * @param state the initial mutational state
     * @param nb_mutations the number of mutational events to simulate
     * @param gen a random number generator
     * @tparam Generator a random number generator
     * @return
     */
    template <typename Generator> static state_type mute(state_type state, unsigned int nb_mutations, Generator &gen)
    {
        assert(dico.find(state) != dico.end());
        for (unsigned int i = 0; i < nb_mutations; ++i)
        {
            std::uniform_int_distribution dist(0, 2);
            state = dico.at(state)[dist(gen)];
        }
        return state;
    }
};

} // namespace quetzal::mutation::JC69

// const std::map<char, std::string> quetzal::mutation::JC69::dico = { {'A', "TGC"}, {'T', "ACG"}, {'G', "ATC"}, {'C',
// "ATG"} };

#endif
