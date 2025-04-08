#pragma once

#include <iostream>
#include <vector>

namespace VectorUtils {
  template <typename T>
  int find(std::vector<T> vec, T item) {
    int half = vec.size() / 2;
    int spare = vec.size() % 2 != 0;
  
    if (spare > 0 && *(vec.end()-1) == item)
      return vec.size()-1;
    
    for (int left = 0; left < half; left++) {
      int right = vec.size()-left-spare-1;
      if (vec[left] == item)
        return left;
      if (vec[right] == item)
        return right;
    }
  
    return -1;
  }
}

