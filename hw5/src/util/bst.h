/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
#include <cstddef> // size_t
#include <iostream>

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d, BSTreeNode<T>* p = 0):
      _data(d), _left(0), _right(0), _parent(p), _isLeft(false), _isRight(false){}

   T _data;
   BSTreeNode<T>* _left;
   BSTreeNode<T>* _right;
   BSTreeNode<T>* _parent;
   bool _isLeft;
   bool _isRight;
};


template <class T>
class BSTree
{
public:
   BSTree(){
      //_dummy_node = new BSTreeNode<T>(T());
      _root_node = _dummy_node = new BSTreeNode<T>(T());
   }
   ~BSTree() {
      clear();
      delete _dummy_node;
   } // delete or clear();

   class iterator {
      friend class BSTree;
   
   public:
      iterator(BSTreeNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {
         _next();
         return *this;
         // else if(_node->_parent) _node = _node->_parent;
         //else _node = _node->end();
         return *this;
      }
      iterator operator ++ (int) { iterator clone(_node); _next(); return clone; }
      iterator& operator -- () {
         _prev();
         return *this;
//         else if(_node->_parent) {
//            if(_node == _node->_parent->_right) return _node->_parent;
//            while(_node->_parent && (_node->_parent->_left == _node)){
//               _node = _node->_parent;
//            }
//            if(_node->_parent) return _node->_parent;
//            return _node->end();
//         }
//         else {
//            return _node->end();
//         }
      }
      iterator operator -- (int) { iterator clone(_node); _prev(); return clone; }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return (i._node != this->_node) ? true : false; }
      bool operator == (const iterator& i) const { return (i._node == this->_node) ? true : false; }

   private:
      BSTreeNode<T>* _node;
      void _prev(){
//std::cout << "operator--\n";
         if(_node->_left) {
            while(_node->_right){
               _node = _node->_right;
            }
            return;// *this;
         }
         while(_node->_parent){
//std::cout << "_node->_parent\n";
            if(_node == _node->_parent->_left) {
               _node = _node->_parent;
            }
            else {
//std::cout << "_node->_parent->_right\n";
               _node = _node->_parent;
//std::cout << _node->_data << "\n";
//std::cout << _node->_data <<  " at: " << _node << "\n";
//std::cout << "parent" <<  " at: " << _node->_parent << "\n";
               return;// *this;
            }
         }
//std::cout << "_node = 0\n";
         _node = 0;
      }
      void _next(){
         //std::cout << "inside ++\n";
         if(_node->_right) {
            _node = _node->_right;
            while(_node->_left) _node = _node->_left;
            return;// *this;
         }
         while (_node->_parent){
            if(_node == _node->_parent->_right){
               _node = _node->_parent;
            }
            else{
               _node = _node->_parent;
               return;// *this;
            }
         }
         _node = 0;
      }
   };

   iterator begin() const {
      BSTreeNode<T>* i = _root_node;
      if(!i) return iterator(i);
      while(i->_left) i = i->_left;
      return iterator(i);
   }
   iterator end() const {
      BSTreeNode<T>* i = _root_node;
      while(i->_right) i = i->_right;
      std::cout << "end()\n";
      return iterator(i);
   }

   bool travel(T d, BSTreeNode<T>*& n){
      if(d < n->_data){
         if (n->_left){
            n = n->_left;
            return travel(d, n);
         }
         return false;
      }
      else{
         if (n->_right != _dummy_node){
            n = n->_right;
            return travel(d, n);
         }
         if(n->_data == d)
            return true;
         return false;
      }
   }

   void insert(const T& d){
      if(_root_node == _dummy_node){
         _root_node = new BSTreeNode<T>(d);
         _root_node->_right = _dummy_node;
         _dummy_node->_parent = _root_node;
//std::cout << d<< " at: " << _root_node;
         return;
      }
      BSTreeNode<T>* n = _root_node;
      travel(d, n);
      if(d < n->_data){
         n->_left = new BSTreeNode<T>(d, n);
      }
      else{
         if(!n->_right)
            n->_right = new BSTreeNode<T>(d, n);
         else {
            BSTreeNode<T>* temp = n->_right;
            n->_right = new BSTreeNode<T>(d, n);
//std::cout << d<< " at: " << n->_right;
            n->_right->_right = temp;
            temp->_parent = n->_right;
         }
      }
   }

   bool erase(const T& d) {
      if(_root_node == _dummy_node) return false;
      BSTreeNode<T>* n = _root_node;
      bool exist = travel(d, n);
      if(exist){
         erase(n);
         return true;
      }
      return false;
   }

   bool erase(iterator pos){
      if(pos._node == _dummy_node) return false;
      if(pos._node->_parent){
//         std::cout << "erase()\n";
         if(pos._node == pos._node->_parent->_left) {
            if(pos._node->_left){
               pos._node->_parent->_left = pos._node->_left;
               pos._node->_left->_parent = pos._node->_parent;
               delete pos._node;
            }
            else if(pos._node->_right) {
               pos._node->_parent->_left = pos._node->_right;
               pos._node->_right->_parent = pos._node->_parent;
               delete pos._node;
            }
            else{
               pos._node->_parent->_left = 0;
               delete pos._node;
            }
         }
         else{
            if(pos._node->_left){
               pos._node->_parent->_right = pos._node->_left;
               pos._node->_left->_parent = pos._node->_parent;
               delete pos._node;
            }
            else if(pos._node->_right) {
               pos._node->_parent->_right = pos._node->_right;
               pos._node->_right->_parent = pos._node->_parent;
               delete pos._node;
            }
            else{
               pos._node->_parent->_right = 0;
               delete pos._node;
            }
         }
      }
      // root
      else{
//         std::cout << "erase(): root\n";
         if(pos._node->_left){
            _root_node = pos._node->_left;
            pos._node->_left->_parent = 0;
            delete pos._node;
         }
         else if(pos._node->_right) {
            _root_node = pos._node->_right;
            pos._node->_right->_parent = 0;
            delete pos._node;
         }
         else{
            // should not happen
            _root_node = _dummy_node;
            //delete pos._node;
         }
      }
      return true;
   }

   void clear(){
      while(!empty()) { pop_front(); }
   }

   void pop_front() { erase(begin()); }

   void pop_back() {
      if(!_root_node) return;
      BSTreeNode<T>* last = _root_node;
      while (last->_right) {
         last = last->_right;
      }
      erase(iterator(last));
   }

   bool empty() const { return _root_node == _dummy_node; }

   size_t size() const {
      if(empty()) return 0;
      iterator i = begin();
      size_t n = 0;
      while(i != end()){
         ++i; ++n;
      }
      return n;
   }

   void sort() {}

   void print() const {
      std::cout << "BSTree::print() not implement yet.\n";
   }

private:
   BSTreeNode<T>* _root_node;
   BSTreeNode<T>* _dummy_node;
};

#endif // BST_H
