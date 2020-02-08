#include <iostream>
#include <vector>
#include <set>

#ifndef PROBLEM_HPP_
#define PROBLEM_HPP_

typedef int extern_c_idx;
typedef int intern_c_idx;
typedef int r_idx;

namespace scp{
  struct set{
    double cost;
    std::vector<r_idx> member;
  };
}


struct problem{
public:
  short rows, cols;
  std::vector<scp::set> sets;
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
