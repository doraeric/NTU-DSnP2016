/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>
#include <cstddef> // size_t

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() : _isSorted(true){
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; } // check ???
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next; return *(this); }
      iterator operator ++ (int) { iterator clone(_node); _node = _node->_next; return clone; }
      iterator& operator -- () { _node = _node->_prev; return *(this); }
      iterator operator -- (int) { iterator clone(_node); _node = _node->_prev; return clone; }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return (i._node != this->_node) ? true : false; }
      bool operator == (const iterator& i) const { return (i._node == this->_node) ? true : false; }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head); }
   iterator end() const { return iterator(_head->_prev); }
   bool empty() const { return (_head == _head->_prev); }
   size_t size() const {
      size_t i = 0;
      for(iterator it = begin(); it != end(); ++it, ++i){ }
      return i;
   }

   void push_back(const T& x) {
      _isSorted = false;
      DListNode<T> *newitem = new DListNode<T>(x, _head->_prev->_prev, _head->_prev);
      if(empty()){
         _head = newitem;
         _head->_prev->_next = _head;
         _head->_prev->_prev = _head;
         return;
      }
      _head->_prev->_prev->_next = newitem;
      _head->_prev->_prev = newitem;
   }
   void pop_front() {
      erase(begin());
      //_head = dummy->_prev;
      /*
      DListNode<T>* toDelete = _head;
      _head->_next->_prev = _head->_prev->_prev;
      _head->_prev->_next = _head->_prev->_prev;
      delete toDelete;
      */
   }
   void pop_back() {
      erase(iterator(_head->_prev->_prev));
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      if(empty()){ return false; }
      DListNode<T>* dummy = _head->_prev;
//      std::cout<< "original size: " << size() << std::endl;
//      std::cout<< pos._node->_prev->_prev->_data << std::endl;
//      std::cout<<pos._node->_next->_data << std::endl;
//      pos._node->_prev->_next = pos._node->_prev->_prev;
//      pos._node->_next->_prev = pos._node->_prev->_prev;
      pos._node->_next->_prev = pos._node->_prev;
      pos._node->_prev->_next = pos._node->_next;
      _head = dummy->_next;
      //_head = _head->_next->_prev;
      delete pos._node;
//      std::cout << "deleted\n";
//      std::cout << "new head: " << pos._node->_next->_prev->_data << std::endl;
//      std::cout<< "new head2: " << _head->_data << std::endl;
      //std::cout<< "new size: " << size() << std::endl;
      return true;
   }
   bool erase(const T& x) {
      for(iterator it = begin(); it != end(); ++it){
         if(it._node->_data == x){
            erase(it);
            return true;
         }
      }
      return false;
   }
   // delete all nodes except for the dummy node
   void clear() {
      _isSorted = true;
      while(!empty()){
         erase(begin());
      }
   }

//   bool comp(T a, T b){
//      return a < b;
//   }

   void sort() const {
      if(_isSorted) return;
      for(int i = 0, n = (int)size(); i < n; ++i){
         iterator it = begin();
         for(int j = 0; j < n - 1; ++j, ++it){
            if(it._node->_data > it._node->_next->_data){
               T tmpdata = it._node->_data;
               it._node->_data = it._node->_next->_data;
               it._node->_next->_data = tmpdata;
            }
         }
      }
      _isSorted = true;
   }

private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
