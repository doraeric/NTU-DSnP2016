/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) {
      _data.push_back(d);
      if (_data.size() == 1) return;
      size_t i = size() - 1;
      while(i && d < _data[(i + 1)/2 - 1]) {
         _data[i] = _data[(i+1)/2 - 1];
         i = (i + 1) / 2 -1;
      }
      _data[i] = d;
   }
   void swap(size_t a, size_t b) {
      Data tmp = _data[a];
      _data[a] = _data[b];
      _data[b] = tmp;
   }
   void delMin() { delData(0); }
   void delData(size_t i) {
      size_t p = i, t = (p+1)*2-1, sz = size();
      while (t < sz) {
         if (t < sz-1) // t has right fellow
            if (!(_data[t] < _data[t+1]))
               ++t;
         if (_data[sz-1] < _data[t])
            break;
         _data[p] = _data[t];
         p = t;
         t = (t+1)*2 - 1;
      }
      _data[p] = _data[sz-1];
      _data.pop_back();
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
