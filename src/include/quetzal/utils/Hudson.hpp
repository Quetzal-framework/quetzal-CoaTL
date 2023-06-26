class HudsonAlgorithm
{
private:
  // the set of extant ancestors
  std::set<...> P;
  // store the coalescence record
  std::set<...> C;
  // counts the number of extant segments intersecting with a given interval
  std::map<..., ...> S;
  // track the cumulative number of links subtended by each extant segment
  BinaryIndexedTree<...> L;
  // next available node
  ... w;
  // clock
  ... t;

public:

  ///
  /// @brief Constructor
  /// @param sample
  /// @return ?
  /// @remark Implements the routine H1 in S1 Kelleher et al. 2016.
  ///
  HudsonAlgorithm()
  {
    // for each individual in the sample
    for(int j = 1; j <= n; ++j)
    {
      // allocate a segment x covering the interval [0, m) that points to node j
      auto x = Segment(0, m, j);
      // record that this segment subtends m−1 links
      this->L[k] = m - 1;
      // insert it into the set of ancestors P
      this->P.insert(x);
    }

    // initialise the map S stating that the number of extant segments in the interval [0, m) is n
    this->S[0] = n;
    this->S[m] -= 1;
    // set the next available node w to n + 1
    this->w = n+1;
    // set the clock to zero
    this->t = 0;
  } // end constructor


  ///
  /// @brief ?
  /// @param ?
  /// @return ?
  /// @remark Implements the routine H2 in S1 Kelleher et al. 2016.
  ///
  void event()
  {
    // find the total number of links subtended by all segments:
    total = L.total();
    // calculate the current rate of recombination
    lambda_r = r(t)*total;
    // calculate the current rate of common ancestor event
    lambda = lambda_r + P.size()*(P.size()-1);

    // simulate timing and type of next event
    std::exponential_distribution<> exp_dis(lambda);
    auto delta_t = exp_dis(generator);
    t = t + delta_t;
    std::uniform_real_distribution<> unif_dis(0.0,1.0)
     if(unif_dis(gen) < lambda_r / lambda)
     {
       // invoke algorithm R
       recombination_event();
     }else{
       // invoke algorithm C
       common_ancestor_event();
     }
  }

  ///
  /// @brief Choose a link uniformly and break it, resulting in one extra individual in the set of extant ancestors.
  /// @param ?
  /// @return ?
  /// @remark Implements the algorithm R in S1 Kelleher et al. 2016.
  ///
  void recombination_event()
  {
    R1_choose_link();
    R2_break_between_segments();
    R3_break_within_segments();
    R4_update_population();
  }

private:

auto R1_choose_link()
  {
    // pick a link h from the total(L) that are currently being tracked
    std::uniform_real_distribution<> unif_dis(1, L.total());
    auto h = unif_dis(generator);
    // ind the segment y that subtends this link
    auto y = L.find(h);
    // calculate the corresponding breakpoint k
    auto k = right(y) - L.total(y) + h - 1
    x = prev(y)
    // does link h falls within y or between y and its predecessor x?
    if(left(y) < k )
    {
      R3_break_within_segments();
    }else{
      R2_break_between_segments();
    }
  }

  auto R2_break_between_segments()
  {
    // big_lambda is equivalent to null
    // simply break the forward and reverse links in the segment chain between them
    next(x) = big_lambda;
    prev(y) = big_lambda;
    // independent segment chain starting with z
    // which represents the new individual to be added to the set of ancestor
    auto z = y;
    R4_update_population();
  }

  // @brief Split the segment such that the ancestral material from left(y) to k
  //        remains assigned to the current individual and the remainder
  //        is assigned to the new individual z.
  auto R3_break_within_segments()
  {
    // independent segment chain starting with z, which represents the new individual
    // to be added to the set of ancestor
    z = Segment(k, right(y), node(y), big_lambda, next(y));
    if( next(y) != big_lambda)
    {
      prev(next(y)) = z;
    }
    next(y) = big_lambda;
    right(y) = k;
    // update the number of links subtended by the segment y, which has
    // right(z) − k fewer links as a result of this operation
    L[y] += k - right(z);
  }

  auto R4_update_population()
  {
    // update the information about the number of links subtended by this segment
    L[z] = right(z) - left(z) - 1;
    // Insert the segment z into the set of ancestors
    this->P.insert(z)
  }

}; // end class HudsonAlgorithm
