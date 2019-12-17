#include <iostream>
#include <vector>
#include <set>
#include <cassert>
#include <numeric>   //std::iota
#include <algorithm> //std::sort

#include "utils.hpp"
#include "problem.hpp"
namespace {
  template <typename T>
  bool issubset(const std::vector<T>& lhs, const std::vector<T>& rhs){
    assert(lhs.size() == rhs.size());
    for(int i=0; i<lhs.size(); i++){
      if(lhs[i]>rhs[i]) return false;
    }
    return true;
  }
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
	   std::accumulate(col_covers[i1].begin(), col_covers[i1].end(), 0) 
	     >= std::accumulate(col_covers[i2].begin(), col_covers[i2].end(), 0) : 
	   col_costs[i1] < col_costs[i2];
       });
  std::vector<std::vector<int> > new_col_covers;
  std::vector<double> new_col_costs;
  for(const auto& elem: col_indices){
    new_col_covers.push_back(col_covers[elem]);
    new_col_costs.push_back(col_costs[elem]);
  }
  col_covers = new_col_covers;
  col_costs = new_col_costs;

  remove_inactive_cols();
}

void problem::remove_row(int i){
  std::cout << "remove_row(" << i << ")" << std::endl;
  for(int j=0; j<cols; j++){
    col_covers[j].erase(col_covers[j].begin()+i);
  }
  rows--;
}
void problem::remove_col(int i, bool is_active){
  std::cout << "remove_col(" << i << ", " << is_active << ")" << std::endl;
  std::cout << "col_indices["<<i<<"] = " << col_indices[i] << std::endl;
  std::vector<int> removed_cover = col_covers[i];
  col_costs.erase(col_costs.begin()+i);
  col_covers.erase(col_covers.begin()+i);
  col_indices.erase(col_indices.begin()+i);
  cols--;

  if(is_active){
    for(int i=rows-1; i>=0; i--){
      if(removed_cover[i] == 1) remove_row(i);
    }
  }
}
void problem::remove_inactive_cols(){
  std::vector<bool> is_possible_col = std::vector<bool>(cols, true);

  for(int i=0; i<col_covers.size(); i++){
    for(int j=i+1; j<col_covers.size(); j++){
      if(col_costs[i] <= col_costs[j] && is_possible_col[i]
	 && issubset(col_covers[j], col_covers[i])){
	is_possible_col[j] = false;
      }
    }
  }

  for(int i=cols-1; i>=0; i--){
    if(is_possible_col[i]) continue;
    is_possible_col.erase(is_possible_col.begin()+i);
    remove_col(i, false);
  }
}

bool problem::solvable() const{
  std::set<int> test_val;
  for(int j=0; j<cols; j++){
    for(int i=0; i<rows; i++){
      if(col_covers[j][i]) test_val.insert(i);
    }
  }
  return rows == test_val.size();
}

void problem::parse(std::istream& is){
  is >> rows >> cols;
  for(int i=0; i<cols; i++){
    int temp;
    is >> temp;
    col_costs.push_back(temp);
  }

  col_covers = std::vector<std::vector<int> >(cols, std::vector<int>(rows, 0));
  
  for(int i=0; i<rows; i++){
    int num;
    is >> num;
    for(int j=0; j<num; j++){
      int temp;
      is >> temp;
      col_covers[temp-1][i] = 1;
    }
  }
  init();
}
