#include <iostream>
#include <vector>
#include <set>

#ifndef PROBLEM_HPP_
#define PROBLEM_HPP_

struct problem{
public:
  short rows, cols;
  std::vector<double> col_costs;
  std::vector<std::vector<int> > col_covers;
  std::vector<int> col_indices;
  void init();
  void parse(std::istream& is);
  void remove_col(int i, bool is_active);
  bool solvable() const;
private: 
  void remove_row(int i);
  void remove_inactive_cols();
};

#endif
