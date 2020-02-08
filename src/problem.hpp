#include <iostream>
#include <vector>
#include <set>

#ifndef PROBLEM_HPP_
#define PROBLEM_HPP_

// TODO: Unification of 0-origin and 1-origin
// col idx: 0-origin
// row idx: 1-origin

typedef int extern_c_idx;
typedef int r_idx;

namespace scp{
  struct set{
    extern_c_idx ext_idx;
    double cost;
    std::vector<r_idx> member;
  };
}


struct problem{
public:
  short rows, cols;
  std::vector<scp::set> sets;
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
