#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

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

    bool operator<(const set &o) const{
      if(cost != o.cost) return cost < o.cost;
      int member_counts   = std::accumulate(member.begin(), member.end(), 0);
      int member_counts_o = std::accumulate(o.member.begin(), o.member.end(), 0);
      return member_counts >= member_counts_o;
    }
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
