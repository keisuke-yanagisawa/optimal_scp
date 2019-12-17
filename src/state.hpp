#include <vector>
#include <set>

#include "utils.hpp"
#include "problem.hpp"

#ifndef STATE_HPP_
#define STATE_HPP_

struct state{
public: 
  problem pr;
  double Z_UB, Z_LB;
  std::vector<double> t, P;
  std::set<int> X;
  std::set<int> Z_UB_set;
  state(const problem& pr);
private:
  void init_t();
  void init_P();
};

#endif