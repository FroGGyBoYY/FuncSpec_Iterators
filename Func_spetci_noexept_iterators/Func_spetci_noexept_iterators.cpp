#include<iostream>
#include<vector>
#include<iterator>
#include<list>

//	1 Check of working excaptions in functions
//not in constructor and destructor
//For solving throwing constructors probleb look ta <</dependent_names_problem.cpp>>
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
//	2
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


//	3 
//functions std::advance  -> increment iterator
//std::distance -> return distance between iterators (min_it, max_it) <- 
// <- if this condition is not met it will ba UB

//	3.1
//std::advance incremented random_access iterato on N -> O(1), otherwise N -> O(N)

/*
* this template function is not work 
* because (iter += n) can not being compiled
template<typename Iterator>
void my_advance(Iterator& it, int n) {
	if (std::is_same<typename std::iterator_traits<Iterator>::iterator_category, std::random_access_iterator_tag>::value){
		it += n;
	}else {
		for (int i = 0; i < n; ++i, ++it);
	}
}
thats why we wil use this ->
*/
template<typename Iterator,typename IteratorCategory>
void my_advance_helper(Iterator& iter, int n, IteratorCategory) {
	for (int i = 0; i < n; ++i, ++iter);
}
template<typename Iterator>
void my_advance_helper(Iterator& iter, int n, std::random_access_iterator_tag) {
	iter += n;
}
 
template<typename Iterator>
void my_advance(Iterator& iter, int n) {
	my_advance_helper(iter, n, std::iterator_traits<Iterator>::iterator_category());
}
int main() {
	//1 excaptions try catch throw
	try {
		g();
	}
	catch (...) {//catching with elipsis typeId
		std::cout << "/|\\" << std::endl;
		std::cout << " | exception of" << std::endl;
	}

	std::vector<int>v = { 1,2,3,4,5 };
	for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
		std::cout << *it << ' ';
	}
	std::cout << std::endl;
	for (auto it : v) {
		std::cout << it << ' ';
	}
	std::cout << std::endl;
	std::cout << "\n\n";

	//std::advance
	std::list<int> lst = { 1,2,3,4,5 };

	std::list<int>::iterator it_b = lst.begin();
	std::list<int>::iterator it_e = lst.end();

	std::cout << "Distance between iterators = " << std::distance(it_b, it_e) << std::endl;
	std::cout <<"First elem = " << *it_b << std::endl;

	std::cout << "Move it_b -> on 4 count" << std::endl;
	std::advance(it_b, 4);// trying STL std::advance

	std::cout <<"This element is: " << *it_b << std::endl;
	std::cout << "Distance between iterators = " << std::distance(it_b, it_e) << std::endl;;

	//	3.1
	//trying my_advance
	std::list<int>::iterator _m_it_b = lst.begin();
	std::list<int>::iterator _m_it_e = lst.end();

	std::cout << "First iterator is - " << *_m_it_b << std::endl;
	my_advance(_m_it_b, 3);
	std::cout << "But now he is - " << *_m_it_b << std::endl;
	/*
	*/
	return 0;
}