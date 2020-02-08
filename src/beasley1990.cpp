#include <iostream>
#include <iomanip>
#include <cassert>
#include <thread>
#include <chrono>

#include "const.hpp"
#include "problem.hpp"
#include "utils.hpp"
#include "state.hpp"

namespace {
double construct_solution(const problem& pr, state& st){
//double construct_solution(const problem& pr, std::set<int>& X){
  std::vector<int> num_included(pr.rows);
  // check satisfaction by X
  for(int j=0; j<pr.cols; j++){
    if(st.X.find(j) == st.X.end()) continue;
    for(int i=0; i<pr.rows; i++){
      if(pr.col_covers[j][i]) num_included[i]++;
    }
  }

  // increment
  for(int i=0; i<pr.rows; i++){
    if(num_included[i]) continue;
    for(int j=0; j<pr.cols; j++){
      if(pr.col_covers[j][i]){
	st.X.insert(j);
	for(int i2=0; i2<pr.rows; i2++){
	  if(pr.col_covers[j][i2]) num_included[i2]++;
	}
      }
    }
  }

  // decrement
  for(int j=pr.cols-1; j>=0; j--){
    if(st.X.find(j) == st.X.end()) continue;

    bool flag = true;
    for(int i=0; i<pr.rows; i++){
      if(pr.col_covers[j][i]) flag &= num_included[i]>=2;
    }
    if(!flag) continue;
    st.X.erase(j);
    for(int i=0; i<pr.rows; i++){
      if(pr.col_covers[j][i]) num_included[i]--;
    }
  }

  //calculate total cost
  double cost = 0;
  for(int j=0; j<pr.cols; j++){
    if(st.X.find(j) == st.X.end()) continue;
    cost += pr.sets[j].cost;
  }

  return cost;

}

double llbp(const problem& pr, state& st){
//double llbp(const problem& pr, const std::vector<double>& t, std::set<int>& X){  

  // calculate C
  std::vector<double> C(pr.cols);
  for(int j=0; j<pr.cols; j++){
    C[j] = pr.sets[j].cost;
    for(int i=0; i<pr.rows; i++){
      if(pr.col_covers[j][i]) C[j]-=st.t[i];
    }
  }

  // calculate X
  st.X = std::set<int>();
  for(int i=0; i<pr.cols; i++){
    if(C[i]<=0) st.X.insert(i);
  }

  // calculate Z_LB
  double Z_LB = 0;
  for(int j=0; j<pr.cols; j++){
    if(st.X.find(j) != st.X.end()) Z_LB += C[j];
  }
  for(int i=0; i<pr.rows; i++){
    Z_LB += st.t[i];
  }

  return Z_LB;

}

std::vector<double> init_t(const problem& pr){
  std::vector<double> t(pr.rows, 1e8);
  for(int r=0; r<pr.rows; r++){
    for(int c=0; c<pr.cols; c++){
      if(pr.col_covers[c][r] && t[r] > pr.sets[c].cost)
	t[r] = pr.sets[c].cost;
    }
  }
  return t;
}
std::vector<double> init_P(const problem& pr){
  std::vector<double> P;
  for(int c=0; c<pr.sets.size(); c++){
    P.push_back(pr.sets[c].cost);
  }
  return P;
}

void update_P(const problem& pr, state& st){
//void update_P(const problem& pr, const std::set<int>& X, double Z_LB, std::vector<double>& P){
  for(int c=0; c<pr.sets.size(); c++){
    if(st.Z_UB_set.find(c) != st.Z_UB_set.end()) 
      st.P[c] = std::max(st.P[c], st.Z_LB);
    else   
      st.P[c] = std::max(st.P[c], st.Z_LB + pr.sets[c].cost);
  }
  //utils::dump(st.Z_UB_set);
  //utils::dump(pr.col_indices);
  //utils::dump(st.P);
}

std::pair<std::set<int>, int> remove_cols(problem& pr, state& st){
  // remove inactives
  
  //for(int j=pr.cols-1; j>=0; j--){
  //  if(st.P[j] > st.Z_UB + 1e-4){
  //    std::cout << "remove col " << j << std::endl;
  //    pr.remove_col(j, false);
  //    st.P.erase(st.P.begin()+j);
  //  }
  //}

  std::set<int> actives;
  int actives_cost = 0;
  bool flag = true;

  // activation can chain thus while loop is needed
  // TODO korehonntou?
  while(flag){
    flag = false;
    std::vector<int> num_included(pr.rows, 0);
    for(int j=0; j<pr.cols; j++){
      if(actives.find(pr.col_indices[j]) != actives.end()) break;
      for(int i=0; i<pr.rows; i++){
	if(pr.col_covers[j][i]) num_included[i]++;
      }
    }
    //utils::dump(num_included);
    for(int i=pr.rows-1; i>=0; i--){
      //assert(num_included[i] != 0);
      if(num_included[i] > 1) continue;
      for(int j=0; j<pr.cols; j++){
	if(pr.col_covers[j][i] && actives.find(pr.col_indices[j]) == actives.end()){
	  std::cout << j << " " << i << std::endl;
	  actives.insert(pr.col_indices[j]);
	  actives_cost += pr.sets[j].cost;
	  //pr.remove_col(j, true);
	  flag = true;
	  break;
	}
      }
      if(flag) break;
    }
  }

  return std::make_pair(actives, actives_cost);
}

void update_t(const problem& pr, state& st, double f){
//void update_t(const problem& pr, const std::set<int>& X, double Z_LB, double Z_UB, double f, std::vector<double>& t){
  std::vector<double> G(pr.rows, 1);
  for(int j=0; j<pr.cols; j++){
    if(st.X.find(j) == st.X.end()) continue;
    for(int i=0; i<pr.rows; i++){
      if(pr.col_covers[j][i]) G[i]--;
    }
  }

  double denominator = 0;
  for(int i=0; i<pr.rows; i++){
    if(st.t[i] == 0 && G[i]<0) G[i] = 0;
    denominator += G[i] * G[i];
  }
  if(denominator == 0) return;

  double T = f * (st.Z_UB - st.Z_LB) / denominator;

  for(int i=0; i<pr.rows; i++){
    st.t[i] += T*G[i];
    if(st.t[i] < 0) st.t[i] = 0;
  }
}
}

state primal_dual(problem& pr, state& st){
  st.actives = std::set<int>();

  int loops = 0;
  double f = 0.5; // update rate
  int last_Z_max_updated = -1;
  int last_pr_cols = pr.cols;
  while(f > 0.005){
    double Z_LB = llbp(pr, st) + st.actives.size();
    if(Z_LB > st.Z_LB){
      st.Z_LB = Z_LB;
      last_Z_max_updated = loops;
    }else{
      if(loops - last_Z_max_updated >= 30){
	f /= 2;
	last_Z_max_updated = loops;
      }
    }
    update_t(pr, st, f);
    double Z_UB = construct_solution(pr, st) + st.actives.size();
    if(st.Z_UB > Z_UB){
      st.Z_UB = Z_UB;
      st.Z_UB_set = std::set<int>();
      for(const auto& elem: st.X){
	st.Z_UB_set.insert(pr.col_indices[elem]);
      }
      for(const auto& elem: st.actives){
	st.Z_UB_set.insert(elem);
      }
      //utils::dump(st.Z_UB_set);
    }
    update_P(pr, st);

    loops++;
    std::cout << "Step " << loops << ": " 
	      << std::setprecision(12)
	      << st.Z_LB << " <= z <= " << st.Z_UB << std::endl;
    if(st.Z_LB > st.Z_UB + constants::SMALL_VAL) break;

    auto data = remove_cols(pr, st);
    for(const auto& elem: data.first){
      st.actives.insert(elem);
    }
    
    if(last_pr_cols != pr.cols){
      // re-initialize
      std::cout << "Problem has been shrinked" << std::endl;
      std::set<int> tmp_actives = st.actives;
      st = state(pr);
      st.actives = tmp_actives;
      last_pr_cols = pr.cols;
      //f = 2;
      int last_Z_max_updated = -1;
      std::cout << st.actives.size() << ": ";
      utils::dump(st.actives);
    }
  }
  return st;
}

state primal_dual(problem& pr){
  state st(pr);
  return primal_dual(pr, st);
}

