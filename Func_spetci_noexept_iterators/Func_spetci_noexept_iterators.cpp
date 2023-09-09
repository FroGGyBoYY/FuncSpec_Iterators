#include<iostream>
#include<vector>
#include<list>

class S {
public:
	int f(int x, int y) {
		if (y == 0) throw 1;
		return x / y;
	}
};

class F {
public:
	int f(int x, int y)noexcept {
		return x * y;
	}
};
template<typename T>
int f(const T& x) noexcept(noexcept(x.f(1, 0))) {
	return x.f(1, 0);
}
//exceptions in desrtuctors
class Danger {
private:
	int x = 0;
public:
	Danger(const int& x) :x(x) {}
	int get_X() noexcept { return x; }
	~Danger() noexcept(false) {
		if (x == 0)throw 1;
	}
};

void g();
void f() {
	Danger d(0);
	std::cout << "caught f( " << d.get_X() << " )" << std::endl;
	g();
}
void g() {
	Danger d(0);
	std::cout << "caught g( " << d.get_X() << " )" << std::endl;
}
int main() {
	try {
		g();
	}
	catch (...) {
		std::cout << "/|\\" << std::endl;
		std::cout << " | exception of" << std::endl;
	}
	std::vector<int>v = { 1,2,3,4,5 };

	for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
		std::cout << *it << ' ';
	}std::cout << std::endl;
	for (auto it : v) {
		std::cout << it << ' ';
	}std::cout << std::endl;

	std::list<int>lst = { 3,2,1 };
	for (std::list<int>::iterator it = lst.begin(); it != lst.end(); ++it) {
		std::cout << *it << ' ';
	}std::cout << std::endl;
	
	return 0;
}