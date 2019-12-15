#include <iostream>
#include <vector>
#include <set>

#ifndef PROBLEM_HPP_
#define PROBLEM_HPP_

struct problem{
public:
  short rows, cols;
  std::vector<int> col_costs;
  std::vector<std::set<int> > col_covers;
  std::vector<int> col_indices;
  void init();
  void parse(std::istream& is);
  void remove_col(int i);
private: 
  void remove_inactive_cols();
  void verify();
};

#endif
