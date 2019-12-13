#include <iostream>
#include <vector>
#include <bitset>
#include <cassert>
#include <numeric>   //std::iota
#include <algorithm> //std::sort

template <unsigned MAX_SIZE>
struct problem{
public:
  short rows, cols;
  std::vector<int> col_costs;
  std::vector<std::bitset<MAX_SIZE> > col_covers;
  std::bitset<MAX_SIZE> is_active_col;
  std::vector<int> col_indices;
  void init();
  void parse(std::istream& is);
private: 
  void detect_active_cols();
  void verify();
};

// --------------------Implementation---------------------

namespace {
  bool issubset(const std::bitset<param::MAX_NUM>& lhs, const std::bitset<param::MAX_NUM>& rhs){
    return (lhs & (~rhs)).count()==0;
  }
}

template <unsigned MAX_SIZE>
void problem<MAX_SIZE>::init(){
  std::vector<int> col_indices(cols);
  std::iota(col_indices.begin(), col_indices.end(), 0);
  sort(col_indices.begin(), col_indices.end(), 
       [this](size_t i1, size_t i2){
	 return col_costs[i1] == col_costs[i2] ? 
	   col_covers[i1].count() > col_covers[i2].count() : 
	   col_costs[i1] < col_costs[i2];
       });
  std::vector<std::bitset<MAX_SIZE> > new_col_covers;
  std::vector<int> new_col_costs;
  for(const auto& elem: col_indices){
    new_col_covers.push_back(col_covers[elem]);
    new_col_costs.push_back(col_costs[elem]);
  }
  col_covers = new_col_covers;
  col_costs = new_col_costs;

  detect_active_cols();
  verify();

  for(const auto& elem: col_covers){
    std::cout << elem << std::endl;
  }
}

template <unsigned MAX_SIZE>
void problem<MAX_SIZE>::detect_active_cols(){
  is_active_col = std::bitset<MAX_SIZE>();
  for(int i=0; i<cols; i++){
    is_active_col[i] = true;
  }
  
  for(int i=0; i<col_covers.size(); i++){
    for(int j=0; j<col_covers.size(); j++){
      if(i==j) continue;
      if(col_costs[i] <= col_costs[j] && is_active_col[i]
	 && issubset(col_covers[j], col_covers[i])){
	is_active_col[j] = false;
      }
    }
  }
}

template <unsigned MAX_SIZE>
void problem<MAX_SIZE>::verify(){
  std::bitset<MAX_SIZE> test_val;
  for(int i=0; i<cols; i++){
    test_val |= col_covers[i];
  }
  assert(rows == test_val.count());
  
  test_val = std::bitset<MAX_SIZE>();
  for(int i=0; i<cols; i++){
    if(!is_active_col[i]) continue;
    test_val |= col_covers[i];
  }
  assert(rows == test_val.count());

}

template <unsigned MAX_SIZE>
void problem<MAX_SIZE>::parse(std::istream& is){
  is >> rows >> cols;
  for(int i=0; i<cols; i++){
    int temp;
    is >> temp;
    col_costs.push_back(temp);
  }

  col_covers = std::vector<std::bitset<MAX_SIZE> >(cols);
  
  for(int i=0; i<rows; i++){
    int num;
    is >> num;
    for(int j=0; j<num; j++){
      int temp;
      is >> temp;
      col_covers[temp-1][i] = true;
    }
  }

  init();
}
