#include <iostream>
#include <vector>
#include <set>
#include <cassert>
#include <numeric>   //std::iota
#include <algorithm> //std::sort

#include "utils.hpp"

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

// --------------------Implementation---------------------

namespace {
  template <typename T>
  bool issubset(const std::set<T>& lhs, const std::set<T>& rhs){
    std::set<T> tmp = rhs;
    for(const auto& elem: lhs) tmp.insert(elem);
    return (tmp.size() - rhs.size()) == 0;
  }
}

void problem::init(){
  for(const auto& elem: col_covers){
    utils::dump(elem);
  }

  col_indices = std::vector<int>(cols);
  std::iota(col_indices.begin(), col_indices.end(), 0);
  sort(col_indices.begin(), col_indices.end(), 
       [this](size_t i1, size_t i2){
	 return col_costs[i1] == col_costs[i2] ? 
	   col_covers[i1].size() >= col_covers[i2].size() : 
	   col_costs[i1] < col_costs[i2];
       });
  std::vector<std::set<int> > new_col_covers;
  std::vector<int> new_col_costs;
  for(const auto& elem: col_indices){
    new_col_covers.push_back(col_covers[elem]);
    new_col_costs.push_back(col_costs[elem]);
  }
  col_covers = new_col_covers;
  col_costs = new_col_costs;

  remove_inactive_cols();
  //detect_active_cols();
  verify();
}

void problem::remove_col(int i){
  col_costs.erase(col_costs.begin()+i);
  col_covers.erase(col_covers.begin()+i);
  col_indices.erase(col_indices.begin()+i);
  cols--;
}
void problem::remove_inactive_cols(){
  std::vector<bool> is_active_col = std::vector<bool>(cols, true);

  for(int i=0; i<col_covers.size(); i++){
    for(int j=i+1; j<col_covers.size(); j++){
      if(col_costs[i] <= col_costs[j] && is_active_col[i]
	 && issubset(col_covers[j], col_covers[i])){
	is_active_col[j] = false;
      }
    }
  }

  for(int i=cols-1; i>=0; i--){
    if(is_active_col[i]) continue;
    is_active_col.erase(is_active_col.begin()+i);
    remove_col(i);
  }
}

void problem::verify(){
  std::set<int> test_val;
  for(int i=0; i<cols; i++){
    for(const auto& elem: col_covers[i]) test_val.insert(elem);
  }
  assert(rows == test_val.size());
}

void problem::parse(std::istream& is){
  is >> rows >> cols;
  for(int i=0; i<cols; i++){
    int temp;
    is >> temp;
    col_costs.push_back(temp);
  }

  col_covers = std::vector<std::set<int> >(cols);
  
  for(int i=0; i<rows; i++){
    int num;
    is >> num;
    for(int j=0; j<num; j++){
      int temp;
      is >> temp;
      col_covers[temp-1].insert(i);
    }
  }
  init();
}
