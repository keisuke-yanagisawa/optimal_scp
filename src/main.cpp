#include <iostream>
#include <set>

#include "utils.hpp"
#include "problem.hpp"
#include "state.hpp"
#include "beasley1990.hpp"


int main(void){
  problem pr;
  pr.parse(std::cin);

  std::set<int> actives;
  double active_cost = 0;

  state result = primal_dual(pr, actives, active_cost);
  utils::dump(result.Z_UB_set);
  std::cout << pr.rows << " " << pr.cols << std::endl;
  return 0;
}
