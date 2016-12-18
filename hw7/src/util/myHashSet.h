/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
//#define _DEBUG_INFO

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

#ifdef _DEBUG_INFO
#include <iostream>
#endif // _DEBUG_INFO
using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ()" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(const HashSet<Data>* h, size_t b, size_t e) :
         _hashSet((HashSet<Data>*)h)/* magic? */, _bkt_idx(b), _ele_idx(e) {};
      Data& operator * () { return (*_hashSet)[_bkt_idx][_ele_idx]; }
      iterator& operator ++ () {
         if (_bkt_idx >= _hashSet->_numBuckets) { _bkt_idx = _ele_idx = -1; }
         else if ((*_hashSet)[_bkt_idx].size() - _ele_idx > 1) {
            ++_ele_idx;
         }
         else {
            ++_bkt_idx;
            _ele_idx = 0;
            for (; _bkt_idx <= _hashSet->_numBuckets; ++_bkt_idx){
               if (_bkt_idx == _hashSet->_numBuckets) {
                  _bkt_idx = _ele_idx = -1;
                  break;
               }
               if (!(*_hashSet)[_bkt_idx].empty())
                  break;
            }
         }
         return *this;
      }
      iterator operator = (const iterator& i) {
         _hashSet = i._hashSet; _bkt_idx = i._bkt_idx; _ele_idx = i._ele_idx;
         return *this;
      }
      bool operator != (const iterator& i) {
         if (i._hashSet != _hashSet) return true;
         if (i._bkt_idx != _bkt_idx) return true;
         if (i._ele_idx != _ele_idx) return true;
         return false;
      }

   private:
      HashSet<Data>* _hashSet;
      size_t _bkt_idx; // bucket index
      size_t _ele_idx; // element index
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      iterator ret(this, 0, 0);
      if (!_buckets[0].empty())
         return ret;
      ++ret;
      return ret;
   }
   // Pass the end
   iterator end() const { return iterator(this, -1, -1); }
   // return true if no valid data
   bool empty() const {
      if (size())
         return false;
      return true;
   }
   // number of valid data
   size_t size() const {
      size_t s = 0;
      for (size_t i = 0; i < _numBuckets; ++i) {
         s += _buckets[i].size();
      }
      return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const {
      size_t idx = d() % _numBuckets;
      for (size_t i = 0; i < _buckets[idx]; ++i) {
         if (_buckets[idx][i] == d)
            return true;
      }
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      size_t idx = d() % _numBuckets;
      for (size_t i = 0; i < _buckets[idx].size(); ++i) {
         if (d == _buckets[idx][i]) {
            d = _buckets[idx][i];
            return true;
         }
      }
      return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) {
      if (query(d)) return true;
      size_t idx = d() % _numBuckets;
      _buckets[idx].push_back(d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      size_t idx = d() % _numBuckets;
      for (size_t i = 0; i < _buckets[idx].size(); ++i) {
         if (d == _buckets[idx][i]) {
            return false;
         }
      }
      _buckets[idx].push_back(d);
#ifdef _DEBUG_INFO
cout << "\033[0;31m";
cout << __FILE__ << "    ";
cout << _buckets[idx][_buckets[idx].size()-1]
   << " (" << idx << ", " << _buckets[idx].size()-1 <<")" << endl;
cout << "\033[0m";
#endif //_DEBUG_INFO
      return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) {
      size_t idx = d() % _numBuckets;
      for (size_t i = 0; i < _buckets[idx].size(); ++i) {
         if (d == _buckets[idx][i]) {
            _buckets[idx].erase(_buckets[idx].begin() + i);
            return true;
         }
      }
      return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
