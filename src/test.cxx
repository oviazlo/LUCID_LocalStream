#include <serviceFunctions.h>

using namespace std;

template<class Type1, class Type2>
struct Pair
{
  /// data-members
  Type1 first;
  Type2 second;
  
  /// constructors
  Pair(const Type1& t1 = Type1(), const Type2& t2 = Type2()):
  first(t1), second(t2)
  {}
  
  Pair(const Pair<Type1,Type2>& OtherPair):
  first(OtherPair.first),
  second(OtherPair.second)
  {}

  /// functions
  void swap(const Pair<Type1,Type2>& OtherPair)
  {
    first = OtherPair.first;
    second = OtherPair.second;
  }
  
  void printPair(const string prefix = "pair")
  {
    cout << prefix << ": <" << first << "," << second << ">" << endl; 
  }
  
  /// Operator overload
  bool operator == (const Pair<Type1,Type2>& OtherPair)
  const
  {
    return first == OtherPair.first &&
	   second == OtherPair.second;
  }
  
  Pair<Type1,Type2> operator + (const Pair<Type1,Type2>& OtherPair)
  const
  {
    return Pair(first + OtherPair.first,second + OtherPair.second);
  }
  
  Pair<Type1,Type2> operator - (const Pair<Type1,Type2>& OtherPair)
  const
  {
    return Pair(first - OtherPair.first,second - OtherPair.second);
  }
  
  Pair<Type1,Type2> operator * (const Pair<Type1,Type2>& OtherPair)
  const
  {
    return Pair(first * OtherPair.first,second * OtherPair.second);
  }
  
  Pair<Type1,Type2> operator / (const Pair<Type1,Type2>& OtherPair)
  const
  {
    return Pair(first / OtherPair.first,second / OtherPair.second);
  }
  
  Pair<Type1,Type2>& operator = (const Pair<Type1,Type2>& OtherPair)
  {
    swap(OtherPair);
    return *this;
  }
  
  Pair<Type1,Type2>& operator += (const Pair<Type1,Type2>& OtherPair)
  {
    first = first + OtherPair.first;
    second = second + OtherPair.second;
    return *this;
  }
  
};


template<class Type = int, Type ARRAY_SIZE = 10>
class Array
{
  
  Type TheArray[ARRAY_SIZE];
  
public:
  template<typename T>
  void Copy(T target_array[ARRAY_SIZE])
  {
    for (int nIndex = 0; nIndex<ARRAY_SIZE; ++nIndex){
      target_array[nIndex] = static_cast<T>(TheArray[nIndex]);
    }
  }
  
};

template<class T>
class Convert
{

  T data;
public:
  Convert(const T& tData = T()): data(tData)
  {}
  
  operator T() const
  {
    return data; 
  }
  
};



int main(){
/*
Pair<int,double> one(12,2.2);
one.printPair("1");
Pair<int,double> two(2,34.5);
two.printPair("2");

Pair<int,double> three = one * two * one;
three.printPair("1*2*1");

three += three;
three.printPair("3+=3");

Array<> myArr;
double doubArr[10];

myArr.Copy(doubArr);*/

Convert<int> IntData(40);
float FloatData;
double DoubleData;

FloatData = IntData;
DoubleData = IntData;



return 0;

}
