#ifndef _UTILITY_H
#define _UTILITY_H

#include <math.h>
#include <float.h>
#include <map>
#include <vector>
#include <iostream>

using namespace std;

namespace Utility{

class Tools{
public:
	static bool IsDigit(char ch){
		return ch >= 48 && ch <= 57;
	}
};
//-----------------------------------------------------------
class Math{

public:

	static double Abs(double x){
		if (x < 0)
			return -x;
		return x;
	}
	static bool Equal(double x,double y){
		return Abs(x-y)<1e-20;
	}

};
//-----------------------------------------------------------
class Vector2{
private:
	double _x;
	double _y;
public:
	Vector2(){};
	Vector2(double x,double y):_x(x),_y(y){};

	double X(){return _x;}
	double Y(){return _y;}	
	
	const Vector2& operator=(Vector2 rhs){
		_x = rhs.X();
		_y = rhs.Y();
		return *this;
	}
	Vector2 operator+(Vector2 rhs){
		return Vector2(_x + rhs.X(),_y + rhs.Y());
	}
	Vector2 operator/(double rhs){
		return Vector2(_x / rhs,_y / rhs);
	}
};
//-----------------------------------------------------------
class Line2D{
private:
	double _k;
	double _b;
	double _x;
public:
	Line2D(){};
	Line2D(double k, double b){
		_k=k;
		_b=b;
	}
	Line2D(Vector2 p1, Vector2 p2){
		if (Math::Equal(p1.X(),p2.X())){
			_k = DBL_MAX;
			_x = p1.X();
		}
		else{
			_k = (p1.Y() - p2.Y()) / (p1.X()-p2.X());
			_b = p1.Y() - _k * p1.X();
		}
	}
	double K(){return _k;}
	double B(){return _b;}
	double X(){return _x;}
	
	bool Perpendicular(){
		return Math::Equal(_k, DBL_MAX);
	}
	
	static bool Perpendicular(Line2D line){
		return Math::Equal(line.K(), DBL_MAX);
	}
	static double Distance(Line2D line, Vector2 p){
		if (line.Perpendicular())
			return abs(line.X()-p.X());
		return abs(line.K()*p.X() - p.Y() + line.B())/sqrt(1+line.K()*line.K());
	}
};
//-----------------------------------------------------------
class Box2D{
private:
	Vector2 _p1;
	Vector2 _p2;
public:
	Box2D(Vector2 p1, Vector2 p2) :_p1(p1), _p2(p2){}

	double Left(){
		if (_p1.X() < _p2.X())
			return _p1.X();
		return _p2.X();
	}
	double Right(){
		if (_p1.X() > _p2.X())
			return _p1.X();
		return _p2.X();
	}
	double Top(){
		if (_p1.Y() > _p2.Y())
			return _p1.Y();
		return _p2.Y();
	}
	double Bottom(){
		if (_p1.Y() < _p2.Y())
			return _p1.Y();
		return _p2.Y();
	}

	static bool InBox(Box2D box, Vector2 p){
		return p.X() >= box.Left() && p.X() <= box.Right() && p.Y() >= box.Bottom() && p.Y() <= box.Top();
	}
};
//-----------------------------------------------------------
class Vector3{
private:
	double _x;
	double _y;
	double _z;
public:
	Vector3(){};
	Vector3(double x, double y, double z) : _x(x), _y(y), _z(z){};

	double X(){ return _x; }
	double Y(){ return _y; }
	double Z(){ return _z; }
	double Length(){ return sqrt(_x*_x + _y*_y + _z*_z); }

	void SetData(double x, double y, double z){
		_x = x;
		_y = y;
		_z = z;
	}

	const Vector3& operator=(Vector3 rhs){
		_x = rhs.X();
		_y = rhs.Y();
		_z = rhs.Z();
		return *this;
	}
	Vector3 operator+(Vector3 rhs){
		return Vector3(_x + rhs.X(), _y + rhs.Y(), _z + rhs.Z());
	}
	Vector3 operator-(Vector3 rhs){
		return Vector3(_x - rhs.X(), _y - rhs.Y(), _z - rhs.Z());
	}
	Vector3 operator*(double rhs){
		return Vector3(_x * rhs, _y * rhs, _z * rhs);
	}
	Vector3 operator/(double rhs){
		return Vector3(_x / rhs, _y / rhs, _z / rhs);
	}
	bool operator==(Vector3 rhs){
		return (Math::Equal(_x, rhs.X()) && Math::Equal(_y, rhs.Y()) && Math::Equal(_z, rhs.Z()));
	}
};
//-----------------------------------------------------------
class Line{
private:
	Vector3 _p1;
	Vector3 _p2;
public:
	Line(){}
	Line(Vector3 p1, Vector3 p2){
		_p1 = p1;
		_p2 = p2;
	}

	Vector3 P1(){ return _p1; }
	Vector3 P2(){ return _p2; }
	Vector3 Dir(){ return _p2-_p1; }

	static Vector3 Sample(Line line, double t){
		return line.P1() + line.Dir()*t;
	}
	static double Distance(Line line, Vector3 p){
		if (line.P1() == line.P2())
			return -1;

		Vector3 edge1 = p - line.P1();
		Vector3 edge2 = p - line.P2();
		Vector3 base = line.P2() - line.P1();

		Vector3 crossProduct = Vector3(edge1.Y()*edge2.Z() - edge1.Z()*edge2.Y(), edge1.Z()*edge2.X() - edge1.X()*edge2.Z(),
			edge1.X()*edge2.Y() - edge1.Y()*edge2.X());

		return crossProduct.Length() / base.Length();
	}
};


//Linq
//-----------------------------------------------------------
template<typename Value>
class List : vector<Value>{
private:

	int Find(Value value){
		int index = 0;
		vector<Value>::iterator ite;
		for (ite = begin(); ite != end(); ite++){
			if (*ite == value){
				return index;
			}
			index++;
		}
		return -1;
	}

public:
	void Add(Value value){
		push_back(value);
	}
	void Insert(Value value, int index){
		if (index < 0)
			index = 0;
		if (index >= size())
			index = size();
		insert(begin() + index, value);
	}
	bool Contains(Value value){
		return Find(value) >= 0;
	}
	void Remove(Value value){
		int index = Find(value);
		if (index < 0)
			return;
		erase(begin() + index);
	}
	void RemoveAt(int index){
		if (index >= 0 && index < size())
			erase(begin() + index);
	}
	int Count(){
		return size();
	}
	void Clear(){
		clear();
	}

	Value& operator[](int index){
		return Get(index);
	}
	Value& Get(int index){
		if (index < 0 || index >= size())
			throw "Index out of range exception";
		return *(begin() + index);
	}
};
//-----------------------------------------------------------
template <typename Key, typename Value>
class Dictionary : map<Key, Value>{

private:

public:
	Dictionary(){};
	~Dictionary(){};

	void Set(Key key, Value value){
		if (!ContainsKey(key))
			Add(key, value);
		else
			Get(key)£½value;
	}
	void Add(Key key, Value value){
		insert(pair<Key, Value>(key, value));
	}
	bool ContainsKey(Key key){
		return (find(key) != end());
	}
	int Count(){
		return size();
	}
	void Remove(Key key){
		erase(key);
	}
	void Clear(){
		clear();
	}
	Value& Get(Key key){
		if (!ContainsKey(key))
			throw "key not found exception";
		iterator ite = find(key);
		return ite->second;
	}
	Value& operator[](Key key)  {
		return Get(key);
	}

	List<Key> Keys()
	{
		List<Key>keys;
		map<Key, Value>::iterator ite;
		for (ite = begin(); ite != end(); ite++){
			keys.Add(ite->first);
		}
		return keys;
	}

protected:
	static Key GetKEY(std::pair<Key, Value> p)
	{
		return p.first;
	}
};
//-----------------------------------------------------------

}

#endif
