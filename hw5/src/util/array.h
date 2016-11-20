/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0), _isSorted(true) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*this); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () { ++_node; return (*this); }
      iterator operator ++ (int) { iterator clone(_node); ++_node; return clone; }
      iterator& operator -- () { --_node; return (*this); }
      iterator operator -- (int) { iterator clone(_node); --_node; return clone; }

      iterator operator + (int i) const { return _node + i; }
      iterator& operator += (int i) { _node += i; return (*this); }

      iterator& operator = (const iterator& i) { _node = i._node; return (*this); }

      bool operator != (const iterator& i) const { return (_node != i._node); }
      bool operator == (const iterator& i) const { return (_node == i._node); }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_data); }
   iterator end() const { return iterator(_data + size()); }
   bool empty() const { return !_size; }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) {
      if(_capacity == 0){
         _capacity = 1;
         _data = new T[1];
      }
      else if(_size == _capacity){
         T* origi = _data;
         _data = new T[_size * 2];
         copy(origi, origi + _size, _data);
         delete [] origi;
         _capacity *= 2;
      }
      else{
      }
      _data[_size] = x;
      ++_size;
   }
   void pop_front() {
      for(size_t i = 0; i < _size - 1; ++i){
         _data[i] = _data[i+1];
      }
      --_size;
   }
   void pop_back() { --_size; }

   bool erase(iterator pos) {
      if(empty()){ return false; }
      for(iterator it = pos; it != end();){
         *(it._node) = *((++it)._node);
      }
      --_size;
      return true;
   }
   bool erase(const T& x) {
      for(size_t i = 0; i < _size; ++i){
         if(*(_data + i) == x){
            erase(iterator(_data + i));
            return true;
         }
      }
      return false;
   }

   void clear() { _size = 0; }

   // This is done. DO NOT change this one.
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
