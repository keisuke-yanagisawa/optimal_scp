#include <iostream>

#include "utils.hpp"
#include "problem.hpp"
#include "state.hpp"
#include "beasley1990.hpp"


int main(void){
  problem pr;
  pr.parse(std::cin);
  state result = primal_dual(pr);
  utils::dump(result.Z_UB_set);
  std::cout << pr.rows << " " << pr.cols << std::endl;
  return 0;
}
