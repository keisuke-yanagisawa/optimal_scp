#include <iostream>
#include <vector>
#include <set>
#include <cassert>
#include <numeric>   //std::iota
#include <algorithm> //std::sort

#include "utils.hpp"
#include "state.hpp"
#include "problem.hpp"

state::state(const problem& pr){
  this->pr = pr;
  Z_UB = 1e8;
  Z_LB = -1e8;
  init_t();
  init_P();
}

void state::init_t(){
  t = std::vector<double>(pr.rows, 1e8);
  for(int i=0; i<pr.rows; i++){
    for(int j=0; j<pr.cols; j++){
      if(pr.col_covers[j].find(i) != pr.col_covers[j].end()
	 && t[i] > pr.col_costs[j]) t[i] = pr.col_costs[j];
    }
  }
}

void state::init_P(){
  for(const auto& elem: pr.col_costs){
    P.push_back(elem);
  }
}
