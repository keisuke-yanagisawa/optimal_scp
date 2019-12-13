#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>
#include <queue>
#include <stack>
#include <algorithm>
#include <ctime>
#include <bitset>
#include <cassert>

#include "const.hpp"
#include "problem.hpp"

struct State{  
  std::bitset<param::MAX_NUM> satisfied_rows;
  std::bitset<param::MAX_NUM> active_cols;
  int cost = 0;
  State activate_col(int col_id, const problem<param::MAX_NUM>& pr) const{
    State new_state = *this;
    new_state.active_cols[col_id] = true;
    new_state.satisfied_rows |= pr.col_covers[col_id];
    new_state.cost += pr.col_costs[col_id];
    return new_state;
  }
};

State solve(const problem<param::MAX_NUM>& pr){

  // initial state
  std::stack<State> stk;
  State st;
  stk.push(st);

  int best = 1e8;
  State best_state;
  long long int n_checked = 0;
  while(!stk.empty()){
    State now = stk.top();
    n_checked++;
    if(n_checked % 100000 == 0){
      std::cout << now.active_cols << std::endl;
      std::cout << std::time(nullptr) << " "
		<< n_checked << " "
		<< stk.size() << " "
		<< best << " "
		<< now.satisfied_rows.count() << " "
		<< now.active_cols.count() << " "
		<< now.cost << std::endl;
    }
    stk.pop();

    // check the possibility to improve the result
    if(best <= now.cost) continue;
    
    if(now.satisfied_rows.count() == pr.rows){
      best_state = now;
      best = now.cost;
      std::cout << n_checked << " " << best << " " << best_state.active_cols << std::endl;
      continue;
    }

    int largest = 0;
    for(int i=0; i<now.active_cols.size(); i++){
      if(now.active_cols[i]) largest = i;
    }

    float largest_efficiency = 0;
    std::vector<State> tmp;
    for(int i=pr.cols-1; i>largest; i--){
      // check the col is useful
      if(pr.is_active_col[i] == false) continue;

      State new_st = now.activate_col(i, pr);
      // check whether does the number of satisfied rows increase
      if(new_st.satisfied_rows.count() == now.satisfied_rows.count()) continue;
      // check the possibility to improve the result
      if(new_st.cost >= best) continue;

      tmp.push_back(new_st);
      float efficiency = (new_st.satisfied_rows.count() - now.satisfied_rows.count())
	/ float(new_st.cost - now.cost);
      if(largest_efficiency < efficiency) largest_efficiency = efficiency;
      //break;
      //stk.push(new_st);
    }
    for(const auto& elem: tmp){
      if((pr.rows - elem.satisfied_rows.count()) >
	 largest_efficiency * (best-1 - elem.active_cols.count())) continue;
      stk.push(elem);
    }
  }
  std::cout << "total loops: " << n_checked << std::endl;
  std::cout << "==== solve end ====" << std::endl;

  return best_state;
}

int main(void){
  problem<param::MAX_NUM> pr;
  pr.parse(std::cin);
  std::cout << pr.is_active_col.count() << std::endl;
  std::cout << pr.is_active_col << std::endl;

  State best_state = solve(pr);
  std::cout << "the best set is: " << std::endl;
  for(int i=0; i<pr.cols; i++){
    if(best_state.active_cols[i]) std::cout << i << " ";
  }

  std::cout << std::endl;
  return 0;
}
