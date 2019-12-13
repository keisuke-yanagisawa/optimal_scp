#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

#include "const.hpp"
#include "problem.hpp"
#include "utils.hpp"

double construct_solution(const problem& pr, std::set<int>& X){
  std::vector<int> num_included(pr.rows);
  // check satisfaction by X
  for(int j=0; j<pr.cols; j++){
    if(X.find(j) == X.end()) continue;
    for(int i=0; i<pr.rows; i++){
      if(pr.col_covers[j].find(i) != pr.col_covers[j].end()) num_included[i]++;
    }
  }

  // increment
  for(int i=0; i<pr.rows; i++){
    if(num_included[i]) continue;
    for(int j=0; j<pr.cols; j++){
      if(pr.col_covers[j].find(i) != pr.col_covers[j].end()){
	X.insert(j);
	for(int i2=0; i2<pr.rows; i2++){
	  if(pr.col_covers[j].find(i2) == pr.col_covers[j].end()) continue;
	  num_included[i2]++;
	}
      }
    }
  }

  // decrement
  for(int j=pr.cols-1; j>=0; j--){
    if(X.find(j) == X.end()) continue;

    bool flag = true;
    for(int i=0; i<pr.rows; i++){
      if(pr.col_covers[j].find(i) != pr.col_covers[j].end()) flag &= num_included[i]>=2;
    }
    if(!flag) continue;
    X.erase(j);
    for(int i=0; i<pr.rows; i++){
      if(pr.col_covers[j].find(i) != pr.col_covers[j].end()) num_included[i]--;
    }
  }

  //calculate total cost
  double cost = 0;
  for(int j=0; j<pr.cols; j++){
    if(X.find(j) == X.end()) continue;
    cost += pr.col_costs[j];
  }

  return cost;

}

double llbp(const problem& pr, const std::vector<double>& t, std::set<int>& X){  

  // calculate C
  std::vector<double> C(pr.cols);
  for(int j=0; j<pr.cols; j++){
    C[j] = pr.col_costs[j];
    for(int i=0; i<pr.rows; i++){
      if(pr.col_covers[j].find(i) != pr.col_covers[j].end()) C[j]-=t[i];
    }
  }

  // calculate X
  X = std::set<int>();
  for(int i=0; i<pr.cols; i++){
    if(C[i]<=0) X.insert(i);
  }

  // calculate Z_LB
  double Z_LB = 0;
  for(int j=0; j<pr.cols; j++){
    if(X.find(j) != X.end()) Z_LB += C[j];
  }
  for(int i=0; i<pr.rows; i++){
    Z_LB += t[i];
  }

  return Z_LB;

}

std::vector<double> init_t(const problem& pr){
  std::vector<double> t(pr.rows, 1e8);
  for(int i=0; i<pr.rows; i++){
    for(int j=0; j<pr.cols; j++){
      if(pr.col_covers[j].find(i) != pr.col_covers[j].end() 
	 && t[i] > pr.col_costs[j]) t[i] = pr.col_costs[i];
    }
  }
  return t;
}
std::vector<double> init_P(const problem& pr){
  std::vector<double> P;
  for(const auto& elem: pr.col_costs){
    P.push_back(elem);
  }
  return P;
}

void update_P(const problem& pr, const std::set<int>& X, double Z_LB, std::vector<double>& P){
  double cost = 0;

  for(int i=0; i<pr.cols; i++){
    if(X.find(i) != X.end()) P[i] = std::max(P[i], Z_LB);
    else                     P[i] = std::max(P[i], Z_LB + pr.col_costs[i]);
  }
}

void remove_cols(problem& pr, const double Z_UB, std::vector<double>& P){
  for(int j=pr.cols-1; j>=0; j--){
    if(P[j] > Z_UB + 1e-4){
      std::cout << "remove col " << j << std::endl;
      pr.remove_col(j);
      P.erase(P.begin()+j);
    }
  }
}

void update_t(const problem& pr, const std::set<int>& X, double Z_LB, double Z_UB, double f, std::vector<double>& t){
  std::vector<double> G(pr.rows, 1);
  for(int j=0; j<pr.cols; j++){
    if(X.find(j) == X.end()) continue;
    for(int i=0; i<pr.rows; i++){
      if(pr.col_covers[j].find(i) != pr.col_covers[j].end()) G[i]--;
    }
  }

  double denominator = 0;
  for(int i=0; i<pr.rows; i++){
    if(t[i] == 0 && G[i]<0) G[i] = 0;
    denominator += G[i] * G[i];
  }
  if(denominator == 0) return;

  double T = f * (1.05*Z_UB - Z_LB) / denominator;

  for(int i=0; i<pr.rows; i++){
    t[i] += T*G[i];
    if(t[i] < 0) t[i] = 0;
  }
}
int main(void){
  problem pr;
  pr.parse(std::cin);
  int loops = 0;
  std::vector<double> t = init_t(pr);
  double Z_max = -1e8, Z_UB = 1e8;
  std::vector<double> P = init_P(pr);

  std::set<int> X;
  double f = 2;
  int last_Z_max_updated = -1;
  std::set<int> best_set;
  while(f > 0.005){
    double Z_LB = llbp(pr, t, X);
    if(Z_LB > Z_max){
      Z_max = Z_LB;
      last_Z_max_updated = loops;
    }else{
      if(loops - last_Z_max_updated >= 30){
	f /= 2;
	last_Z_max_updated = loops;
      }
    }
    update_t(pr, X, Z_LB, Z_UB, f, t);
    double res_val = construct_solution(pr, X);
    if(Z_UB > res_val){
      Z_UB = res_val;
      best_set = std::set<int>();
      for(const auto& elem: X){
	best_set.insert(pr.col_indices[elem]);
      }

    }
    update_P(pr, X, Z_LB, P);

    loops++;
    std::cout << "Step " << loops << ": " 
	      << std::setprecision(12)
	      << Z_max << " <= z <= " << Z_UB << std::endl;
    if(Z_max > Z_UB + 1e-4) break;

    remove_cols(pr, Z_UB, P);
  }

  utils::dump(best_set);
  std::cout << pr.rows << " " << pr.cols << std::endl;
  return 0;
}
