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
  if (beg >= end) return;
  //int mid = (beg + end) >> 1;
  T tmp = vec[beg];
  cmp comp;
  int i = beg, j = end;
  while (i != j) {
    while (comp(tmp, vec[j]) && j > i) j--;
    while (comp(vec[i], tmp) && j > i) i++;
    if (j > i) std::swap(vec[i], vec[j]);
  }
  vec[beg] = vec[i]; vec[i] = tmp;
  sort<T, cmp>(vec, beg, i - 1); sort<T, cmp>(vec, i + 1, end);
}

#endif