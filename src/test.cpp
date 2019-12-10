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

const int MAX_NUM=150;

struct problem{
  short rows, cols;
  std::vector<int> col_costs;
  std::vector<std::bitset<MAX_NUM> > col_covers;
  std::bitset<MAX_NUM> is_active_col;
};

struct State{  
  std::bitset<MAX_NUM> satisfied_rows;
  std::bitset<MAX_NUM> active_cols;
  int cost = 0;
  State activate_col(int col_id, const problem& pr) const{
    State new_state = *this;
    new_state.active_cols[col_id] = true;
    new_state.satisfied_rows |= pr.col_covers[col_id];
    new_state.cost += pr.col_costs[col_id];
    return new_state;
  }
};

bool issubset(const std::bitset<MAX_NUM>& lhs, const std::bitset<MAX_NUM>& rhs){
  return (lhs & (~rhs)).count()==0;
}

void parse(std::istream& is, problem& p){
  is >> p.rows >> p.cols;
  for(int i=0; i<p.cols; i++){
    int temp;
    is >> temp;
    p.col_costs.push_back(temp);
  }

  p.col_covers = std::vector<std::bitset<MAX_NUM> >(p.cols);
  
  for(int i=0; i<p.rows; i++){
    int num;
    is >> num;
    for(int j=0; j<num; j++){
      int temp;
      is >> temp;
      p.col_covers[temp-1][i] = true;
    }
  }
  for(const auto& elem: p.col_covers){
    std::cout << elem << std::endl;
  }

  p.is_active_col = std::bitset<MAX_NUM>();
  for(int i=0; i<p.cols; i++){
    p.is_active_col[i] = true;
  }

  for(int i=0; i<p.col_covers.size(); i++){
    for(int j=0; j<p.col_covers.size(); j++){
      if(i==j) continue;
      if(p.col_costs[i] <= p.col_costs[j] && p.is_active_col[i]
	 && issubset(p.col_covers[j], p.col_covers[i])){
	p.is_active_col[j] = false;
      }
    }
  }

  std::bitset<MAX_NUM> test_val;
  for(int i=0; i<p.cols; i++){
    test_val |= p.col_covers[i];
  }
  assert(p.rows == test_val.count());

  test_val = std::bitset<MAX_NUM>();
  for(int i=0; i<p.cols; i++){
    if(!p.is_active_col[i]) continue;
    test_val |= p.col_covers[i];
  }
  assert(p.rows == test_val.count());
  
}

int main(void){
  problem pr;
  parse(std::cin, pr);

  int best = 1e8;
  std::bitset<MAX_NUM> best_list;
  //std::set<short> best_list;
  //std::priority_queue<State> pq;
  std::stack<State> stk;
  State st;
  stk.push(st);

  int c = pr.is_active_col.count();
  std::cout << c << std::endl;
  std::cout << pr.is_active_col << std::endl;


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
      best = now.cost;
      best_list = now.active_cols;
      std::cout << n_checked << " " << best << " " << best_list << std::endl;
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
  std::cout << "the best set is: " << std::endl;
  for(int i=0; i<pr.cols; i++){
    if(best_list[i]) std::cout << i << " ";
  }
  std::cout << std::endl;
  return 0;
}
