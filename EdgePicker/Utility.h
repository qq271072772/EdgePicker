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
	static int Round(double x){
		double decimal = x - (double)(int)x;
		if (decimal < 0.5f)
			return (int)x;
		return ((int)x) + 1;
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
	bool operator==(Vector2 rhs){
		return Math::Equal(_x, rhs.X()) && Math::Equal(_y, rhs.Y());
	}

	static double Distance(Vector2 p1, Vector2 p2){
		double disX = p1.X() - p2.X();
		double disY = p1.Y() - p2.Y();
		return sqrt(disX*disX + disY*disY);
	}
};
//-----------------------------------------------------------
class Line2D{
private:
	double _k;
	double _b;
	double _x;
public:

	enum PointPosition{
		LINE_AT,
		LINE_LEFT,
		LINE_RIGHT,
	};

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
	
	bool Vertical(){
		return Math::Equal(_k, DBL_MAX);
	}
	
	static bool Perpendicular(Line2D line){
		return Math::Equal(line.K(), DBL_MAX);
	}
	static double Distance(Line2D line, Vector2 p){
		if (line.Vertical())
			return abs(line.X()-p.X());
		return abs(line.K()*p.X() - p.Y() + line.B())/sqrt(1+line.K()*line.K());
	}
	static double Sample(Line2D line, double x){
		if (line.Vertical())
			return -1;
		return line.K()*x + line.B();
	}
	static PointPosition PointPos(Line2D line, Vector2 p){
		if (Math::Equal(line.K(), DBL_MAX)){
			if (Math::Equal(p.X(), line.X()))
				return LINE_AT;
			return p.X() < line.X() ? LINE_LEFT : LINE_RIGHT;
		}
		if (Math::Equal(line.K(), 0)){
			if (Math::Equal(p.Y(), line.B()))
				return LINE_AT;
			return p.Y() > line.B() ? LINE_LEFT : LINE_RIGHT;
		}
		double ret = p.Y() - line.K()*p.X() - line.B();
		if (Math::Equal(ret, 0))
			return LINE_AT;
		return line.K() < 0 ? (ret < 0 ? LINE_LEFT : LINE_RIGHT) : (ret>0 ? LINE_LEFT : LINE_RIGHT);
	}
	static bool InterSection(Line2D line1, Line2D line2){
		if (Math::Equal(line1.K(), line2.K()))
			return false;
		return true;
	}
	static Vector2 InterSectionPos(Line2D line1, Line2D line2){
		if (Math::Equal(line1.K(), line2.K()))
			return Vector2(DBL_MAX, DBL_MAX);
		if (line1.Vertical())
			return Vector2(line1.X(), Sample(line2, line1.X()));
		if (line2.Vertical())
			return Vector2(line2.X(), Sample(line1, line2.X()));
		double x = (line2.B() - line1.B()) / (double)(line1.K() - line2.K());
		return Vector2(x, Sample(line1, x));
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
class Ray2D{
public:

	enum RayDir
	{
		LEFT, RIGHT, UP, DOWN,
	};

	Ray2D(){}
	Ray2D(Vector2 s, Vector2 d){
		_start = s;
		_dirP = d;
		_line = Line2D(_start, _dirP);
	}
	~Ray2D(){}

	Vector2 StartP(){
		return _start;
	}
	Vector2 DirP(){
		return _dirP;
	}
	Line2D Line(){
		return _line;
	}

	static bool Intersection(Ray2D ray,Vector2 p1, Vector2 p2){
		Line2D::PointPosition pos1 = Line2D::PointPos(ray.Line(),p1);
		Line2D::PointPosition pos2 = Line2D::PointPos(ray.Line(), p2);

		if (((pos1 == Line2D::PointPosition::LINE_LEFT || pos1 == Line2D::PointPosition::LINE_AT) && pos2 == Line2D::PointPosition::LINE_RIGHT)
			|| ((pos2 == Line2D::PointPosition::LINE_LEFT || pos2 == Line2D::PointPosition::LINE_AT) && pos1 == Line2D::PointPosition::LINE_RIGHT)){

			Line2D edge(p1, p2);
			if (!Line2D::InterSection(ray.Line(), edge))
				return false;
			Vector2 interP = Line2D::InterSectionPos(ray.Line(), edge);
			if (!OnRay(ray,interP))
				return false;
			return true;
		}
		return false;
	}
	static bool OnRay(Ray2D ray,Vector2 p){
		if (Line2D::PointPos(ray.Line(),p) != Line2D::PointPosition::LINE_AT)
			return false;
		if (ray.StartP() == ray.DirP())
			return false;
		if (ray.Line().Vertical())
			return (ray.DirP().Y() > ray.StartP().Y() ? (p.Y() >= ray.StartP().Y()) : (p.Y() <= ray.StartP().Y()));

		return (ray.DirP().X() > ray.StartP().X() ? (p.X() >= ray.StartP().X()) : (p.X() <= ray.StartP().X()));
	}

private:
	Vector2 _start;
	Vector2 _dirP;
	Line2D _line;
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
class Line3D{
private:
	Vector3 _p1;
	Vector3 _p2;
public:
	Line3D(){}
	Line3D(Vector3 p1, Vector3 p2){
		_p1 = p1;
		_p2 = p2;
	}

	Vector3 P1(){ return _p1; }
	Vector3 P2(){ return _p2; }
	Vector3 Dir(){ return _p2-_p1; }

	static Vector3 Sample(Line3D line, double t){
		return line.P1() + line.Dir()*t;
	}
	static double Distance(Line3D line, Vector3 p){
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
