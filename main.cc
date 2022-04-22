#include <iostream>
#include <vector>
#include <unordered_set>
#include <memory>
#include <cmath>
using namespace std;

bool is_near(double x, double y) {
	constexpr double epsilon = 0.01;
	if (isinf(x) && isinf(y))
		return true;
	return abs(x-y) < epsilon;
}

struct Shape {
    virtual ~Shape() = default;
    virtual string getType() const = 0;
};

struct Point: public Shape {
    double x = 0, y = 0;
	Point(double new_x = 0, double new_y = 0) : x(new_x), y(new_y) {}
	/*Point(const Point &p) {
		x = p.x;
		y = p.y;
	}*/
	double distance(const Point& p) {
		return hypot(x - p.x, y - p.y);
	}
	double slope(const Point& p) {
		return (y - p.y) / (x - p.x);
	}
	bool operator==(const Point& p) const{
		return (this->x == p.x && this->y == p.y);
	}
	struct hash {
		auto operator()(const Point& p) const
		{return std::hash<double>()(p.x * 1000 + p.y);}
	};
	virtual string getType() const override {
		return "Point";
	}
	friend ostream& operator<<(ostream& out, const Point& p) {
		out << '(' << p.x << ',' << p.y << ')' << endl;
		return out;
	}
};

bool is_near(Point x, Point y) {
	constexpr double epsilon = 0.01;
	return (abs(x.x-y.x) < epsilon && abs(x.y-y.y) < epsilon);
}

struct Line: public Shape {
	Point a;
	Point b;
	double length = 0, slope = 0;
	Line(Point p1={0,0}, Point p2={0,0}) {
		a = p1;
		b = p2;
		length = getLength();
		slope = getSlope();
	}
	double getLength() {
		return a.distance(b);
	}
	double getSlope() {
		return a.slope(b);
	}
    virtual string getType() const override {
		return "Line";
	}
	friend ostream& operator<<(ostream& out, const Line& l) {
		out << l.a << l.b;
		return out;
	}
};

struct Angle {
	Line a, b;
	double deg;
	double operator()() const {
		return deg;
	}
	Angle(Line l = {{0,0},{0,0}}, Line k = {{0,0},{0,0}}) : a(l), b(k) {}
};

struct Triangle: public Shape {
	Point a;
	Point b;
	Point c;
	Line ab;
	Line bc;
	Line ca;
	Angle bac;
	Angle cba;
	Angle acb;
	double area;
	Triangle(Point p1={0,0}, Point p2={0,0}, Point p3={0,0}) {
		a = p1;
		b = p2;
		c = p3;
	}
    virtual string getType() const override {
		return "Triangle";
	}
	friend ostream& operator<<(ostream& out, const Triangle& t) {
		out << t.a << t.b << t.c;
		return out;
	}
};

struct Rectangle: public Shape {
	Point a;
	Point b;
	Point c;
	Point d;
	Line ab;
	Line bc;
	Line cd;
	Line da;
	double area;
	Rectangle(Point p1={0,0}, Point p2={0,0}, Point p3={0,0}, Point p4={0,0}) {
		a = p1;
		b = p2;
		c = p3;
		d = p4;
	}
    virtual string getType() const override {
		return "Rectangle";
	}
	friend ostream& operator<<(ostream& out, const Rectangle& r) {
		out << r.a << r.b << r.c << r.d;
		return out;
	}
};

struct Other: public Shape {
    vector<Point> vec;
	bool is_closed = false;
	int length = 0;
	Other(vector<Point> &vec, bool is_closed = false, int length = 0) {
		this->vec = vec;
		this->is_closed = is_closed;
		this->length = length;
	}
	virtual string getType() const override {return "Other";}
	friend ostream& operator<<(ostream& out, const Other& o) {
		for (auto p : o.vec)
			cout << p ;
		return out;
	}
};

struct Empty: public Shape {
	Empty() {}
    virtual string getType() const override {return "Empty";}
	friend ostream& operator<<(ostream& out, const Empty& e) {
		return out;
	}
};

shared_ptr<Shape> GetType(vector<Point> &vec) {
	shared_ptr<Shape> shape;

	if (vec.empty()) {
		shape = make_shared<Empty>();
	}
	else if (vec.size() == 1) {
		shape = make_shared<Point>(vec.at(0));
	}
	else {
		bool other = false;
		bool back = false;
		vector<Point> vertices;
		vertices.push_back(vec.at(0));
		double slope = vec.at(0).slope(vec.at(1));
		Point a = vec.at(0);
		int b = 1;
		for (size_t i = 2; i < vec.size(); i++) {
			if (vertices.size() > 4) {
				other = true;
				break;
			}
			auto loc = find(vertices.begin(), vertices.end(), [] (const Point& p) {return is_near(p, vec.at(b));});
			if (!back) {
				if (is_near(vec.at(b).slope(vec.at(i)), slope)) {
					if (is_near(vec.at(i).distance(a), vec.at(b).distance(a)))
						b = i;
					else if (vec.at(i).distance(a) > vec.at(b).distance(a)) {
						if (vec.at(i).distance(a)
							> vec.at(i).distance(vec.at(b))) {
							b = i;
						}
						else {
							other = true;
							break
						}
					}
					else {
						if (is_near(vec.at(b).distance(vec.at(i)),
									vec.at(b).distance(a))) {;}
						else if (vec.at(b).distance(vec.at(i))
									> vec.at(b).distance(a)) {
							other = true;
							break;
						}
						back = true;
						if (loc == vertices.end()) {
							vertices.push_back(vec.at(b));
						}
						else {
							a = *(loc);
							b = i;
						}
					}
				}
				else {
					if (vec.at(b) != vec.at(i-1)) {
						other = true;
						break;
					}
					if (loc == vertices.end()) {
						vertices.push_back(vec.at(b));
						a = vec.at(b);
					}
					else 
						a = *(loc);
					b = i;
					slope = a.slope(vec.at(b));
				}
			}
			else {
				if (is_near(vec.at(i-1).slope(vec.at(i)), slope)) {
					if (vec.at(b).distance(vec.at(i)) > vec.at(b).distance(a)) {
						other = true;
						break;
					}
					if (vec.at(i-1).distance(vec.at(i)) > vec.at(b).distance(a)) {
						vertices.erase(loc);
						vertices.push_back(vec.at(i));
						b = i;
					}
					back = false;
					slope = vec.at(b).slope(vec.at(i));
				}
				else {
					if (vec.at(i) != a) {
						other = true;
						break;
					}
					else {
						for (size_t ai = 0; ai < vec.size(); ai++) {
							if (vec.at(ai) == a) {
								b = ai;
								break;
							}
						}
						if (loc != vec.begin()) {
							a = *(loc - 1);
						}
					}
				}
			}
		}
		if (find(vertices.begin(), vertices.end(), vec.at(b)) == vertices.end() && vertices.size() < 3)
//find(vertices.begin(), vertices.end(), [] (const Point& p) {return is_near(p, vec.at(b));});
			vertices.push_back(vec.at(b));
	//	for (auto p : vertices)
	//		cout << p.x << " " << p.y << endl;
		if (is_near(vec.at(0), vec.at(vec.size()-1)) && vertices.size() > 2) {
			if (vertices.size() == 3)
				shape = make_shared<Triangle>(vertices.at(0),
						vertices.at(1), vertices.at(2));
			if (vertices.size() == 4) {
				double o1 = (vertices.at(1).x-vertices.at(0).x)
					* (vertices.at(2).x-vertices.at(1).x)
					+ (vertices.at(1).y-vertices.at(0).y)
					* (vertices.at(2).y-vertices.at(1).y);
				double o2 = (vertices.at(3).x-vertices.at(2).x)
					* (vertices.at(0).x-vertices.at(3).x)
					+ (vertices.at(3).y-vertices.at(2).y)
					* (vertices.at(0).y-vertices.at(3).y);
	//			cout << o1 << ' ' << o2 << endl;
				if (o1 == 0 && o2 == 0) {
					shape = make_shared<Rectangle>(vertices.at(0), vertices.at(1), vertices.at(2), vertices.at(3));
	//				cout << "Here" <<endl;
				}
				else
					other = true;
			}
			else
				other = true;
		}
		else if (vertices.size() == 2) {
			shape = make_shared<Line>(vertices.at(0), vertices.at(1));
		}
		else
			other = true;
		if (other)
			shape = make_shared<Other>(vec, true);
	}
	return shape;
}

int main() {
	vector<Point> vec;
	double x, y;
	cout << "Please enter a point (x y):" << endl;
	while (cin >> x >> y) {
		cout << "Please enter a point (x y):" << endl;
		if (!cin){
			cout << "BAD INPUT!" << endl;
			exit(1);
		}
		Point p = {x,y};
		vec.push_back(p);
	}
	shared_ptr<Shape> shape = GetType(vec);
	if (auto s = dynamic_pointer_cast<Point>(shape))
		cout << *s;
	if (auto s = dynamic_pointer_cast<Line>(shape))
		cout << *s;
	if (auto s = dynamic_pointer_cast<Triangle>(shape)) {
		cout << *s;
		cout << vec.at(vec.size()-1);
	}
	if (auto s = dynamic_pointer_cast<Rectangle>(shape)) {
		cout << *s;
		cout << vec.at(vec.size()-1);
	}
	if (auto s = dynamic_pointer_cast<Other>(shape))
		cout << *s;
	if (shape)
		cout << shape->getType() << endl;
}
