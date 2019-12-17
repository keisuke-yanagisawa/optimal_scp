#include <set>

#include "problem.hpp"
#include "state.hpp"

#ifndef BEASLEY1990_HPP_
#define BEASLEY1990_HPP_

state primal_dual(problem& pr, std::set<int>& actives);

#endif
