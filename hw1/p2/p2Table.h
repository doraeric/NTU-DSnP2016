#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>

using namespace std;

class Row
{
public:
   const int operator[] (size_t i) const; // TODO
   int& operator[] (size_t i); // TODO
   bool *visible;

   Row(bool *p1, int *p2): visible(p1), _data(p2){}

   void print();

private:
   int  *_data;
};

class Table
{
public:
   size_t colnum;
   size_t rownum();
   const Row& operator[] (size_t i) const;
   Row& operator[] (size_t i);
   bool read(const string&);
   void print();
   void add(Row r);
   Table(): colnum(0){}

private:
   vector<Row>  _rows;
};

#endif // P2_TABLE_H
