#ifndef ACM_TICKETSYSTEM_UTILS
#define ACM_TICKETSYSTEM_UTILS

#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>

template<class T, class cmp = std::less<T>>
void sort(T* beg, T* end) {
  
}

template<class T, class cmp = std::less<T>>
void sort(vector<T>& vec, int beg, int end) {
  if (beg >= end - 1) return;
  T x=vec[(beg + end) >> 1];
  cmp comp;
  int i = beg - 1, j = end;
  while (1) {
    do { i++; } while (comp(vec[i], x));
    do { j--; } while (comp(x, vec[j]));
    if (i >= j) break;
    std::swap(vec[i], vec[j]);
  }
  sort<T, cmp>(vec, beg, j + 1); sort<T, cmp>(vec, j + 1, end);
}

#endif