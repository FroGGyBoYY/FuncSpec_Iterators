﻿#include<iostream>
#include<vector>
#include<iterator>
#include<list>
#include<algorithm>

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


//	3.0
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
thats why we will use this ->
*/
/*
	I am trying to solve this problem by using
	template spetsialisation here  |
								  \|/
								  \|/

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
*/

//But this problem can been solved by constexpr behind if

template <typename Iterator>
void my_advance(Iterator& iter, int n) {
	if constexpr(std::is_same<typename std::iterator_traits<Iterator>::iterator_category, typename std::random_access_iterator_tag>::value) {
		iter += n;
	}
	else {
		if (n < 0) { for (int i = 0; i < n; ++i, --iter); }
		else { for (int i = 0; i < n; ++i, ++iter); }
	}
}

template<typename Iterator>
size_t my_distance(Iterator& first_it, const Iterator& second_it) {
	if constexpr (std::is_same<typename std::iterator_traits<Iterator>::iterator_category, typename std::random_access_iterator_tag>::value) {
		return second_it - first_it;
	}
	else {
		size_t result = 0;
		for (; first_it != second_it; ++result, ++first_it);
		return result;
	}
}




//	4 Const iterator
// .cbegin() .cend()
template<typename Iterator>
class _Const_Iterator {
	//look at template Common_Iterator in _vector, look up :)
};
//	5 Reverse iterators
//	If iterator support bidirectional_iterator 
//	that it actually support reverse iterator
template<typename Iterator>
class _Reverse_Iterator {
private:
	Iterator iter;
public:
	_Reverse_Iterator(const Iterator& iter):iter(iter){}
	_Reverse_Iterator<Iterator>& operator++() {
		--iter;
		return *this;
	}
	_Reverse_Iterator<Iterator> operator++(int) {
		_Reverse_Iterator<Iterator> copy = *this;
		--*this;
		return copy;
	}
	_Reverse_Iterator<Iterator>& operator--() {
		++iter;
		return *this;
	}
	_Reverse_Iterator<Iterator> operator--(int) {
		_Reverse_Iterator<Iterator> copy = *this;
		++* this;
		return copy;
	}
	auto operator*() -> decltype(*iter) {
		return *iter;
	}
	bool operator==(const _Reverse_Iterator<Iterator>& it) {
		return (iter == it.iter);
	}
	bool operator!=(const _Reverse_Iterator<Iterator>& it) {
		return !(*this == it);
	}
	Iterator base() const { return iter; }
};

//	 6 Output iterator
//	 back_insert front insert
//	 for copy_if -> containers
//---------------- So here i am trying to create my own iterator
template <typename T>
class _vector {
private:
	size_t size;
	T* arr;
public:
	_vector() :size(0), arr(nullptr) {}
	_vector(size_t size) :size(size), arr(new T[size]) {
		for (int i = 0; i < size; ++i) {
			arr[i] = T();
		}
	}
	_vector(const std::initializer_list<T>& lst) :size(lst.size()), arr(new T[size]){
		std::copy(lst.begin(), lst.end(), arr);
	}

	template <bool IsConst>
	struct Common_Iterator {
	private:
		std::conditional_t<IsConst, const T*, T*> ptr_elem;
	public:
		Common_Iterator() :ptr_elem(nullptr) {}
		Common_Iterator(T* ptr) :ptr_elem(ptr) {}
		Common_Iterator& operator++() {
			++ptr_elem;
			return *this;
		}
		Common_Iterator operator++(int) {
			Common_Iterator copy = *this;
			++* this;
			return copy;
		}
		Common_Iterator& operator--() {
			--ptr_elem;
			return *this;
		}
		Common_Iterator operator--(int) {
			Common_Iterator copy = *this;
			++* this;
			return copy;
		}
		bool operator ==(const Common_Iterator& it) const noexcept {
			return ptr_elem == it.ptr_elem;
		}
		bool operator !=(const Common_Iterator& it)const noexcept {
			return !(*this == it);
		}
		std::conditional_t<IsConst, const T&, T&> operator*() { return *ptr_elem; }
		std::conditional_t<IsConst, const T*, T*> operator->() { return ptr_elem; }
	};

	using Iterator = Common_Iterator<false>;
	using Const_Iterator = Common_Iterator<true>;

	Iterator begin() const noexcept { 
		return arr;
	}
	Iterator end() const noexcept { 
		return (arr + size); }
	Const_Iterator cbegin() const noexcept { return arr;
	}
	Const_Iterator cend() const noexcept {
		return (arr + size);
	}

	using Reverse_Iterator = _Reverse_Iterator<Iterator>;
	using Const_Reverse_Iterator = _Reverse_Iterator<Const_Iterator>;

	Reverse_Iterator rbegin() const noexcept { 
		return Reverse_Iterator((arr + size - 1));
	}
	Reverse_Iterator rend() const noexcept {
		return Reverse_Iterator(std::prev(arr, 1));
	}

	void clear() noexcept {
		size = 0;
		delete[] arr;
		arr = nullptr;
	}
	bool empty() const noexcept{
		return (size ? 0 : 1);
	}
	void push_back(const T& item) {
		++size;
		T* temp = new T[size];
		memcpy(temp, arr, size - 1);
		temp[size - 1] = item;
		delete[]arr;
		arr = temp;
	}
	void pop_back() {
		if (!(*this->empty())) {
			--size;
			T* temp = new T[size];
			memcpy(temp, arr, size);
			delete[]arr;
			arr = temp;
		}
	}
	size_t _size() const noexcept {
		return size;
	}
	T& operator[](const size_t& item) {
		if (item + 1 > size || empty) {
			std::cout << "Check item count" << std::endl;
		}
		else {
			return *(arr + item);
		}
	}
	T& font()const noexcept {
		if (empty()) {
			std::cout << "_vector is empty" << std::endl;
		}
		else {
			return arr[0];
		}
	}
	T& back() const noexcept {
		if (empty()) {
			std::cout << "_vector is empty" << std::endl;
		}
		else {
			return arr[size - 1];
		}
	}
	~_vector() { delete[]arr; }
};




//---------------- End of creating my own iterator

//OUTPUT iterartors
template<typename Container>
class Back_Insert_Iterator {
private:
	Container& container;
public:
	explicit Back_Insert_Iterator(Container& _container) :container(_container) {}
	Back_Insert_Iterator<Container>& operator++() noexcept { 
		return *this; 
	}
	Back_Insert_Iterator<Container> operator++(int) noexcept {
		Back_Insert_Iterator<Container> copy = *this;
		return copy; 
	}
	Back_Insert_Iterator<Container>& operator*() noexcept { 
		return *this;
	}
	Back_Insert_Iterator<Container>& operator=(const typename Container::value_type& item)  {
		container.push_back(item);
		return *this;
	}
	Back_Insert_Iterator<Container>& operator=(const typename Container::value_type&& item) {
		container.push_back(std::move(item));
		return *this;
	}
	Back_Insert_Iterator<Container>& operator=(const Back_Insert_Iterator<Container>& b_iter) {
		container = b_iter.container;
		return *this;
	}
};
template<typename Container>
Back_Insert_Iterator<Container> Back_Inserter(Container& container) {
	return Back_Insert_Iterator<Container>(container);
}



// Unary Predicate for std::copy_if()
//bool IsEven(int x) { return x % 2 == 0; }
struct IsEven {
	bool operator()(int x) { return x % 2 == 0; }
};

//	7 Sream iterators
//	
template<typename T>
class Istream_Iterator {
private:
	std::istream& in;
	T value;
public:
	Istream_Iterator(std::istream& in) :in(in) { in >> value; }
	Istream_Iterator<T>& operator++() {
		in >> value;
		return *this;
	}
	Istream_Iterator<T> operator++(int) {
		in >> value;
		Istream_Iterator<T> copy = *this;
		return copy;
	}
	Istream_Iterator<T>& operator=(const Istream_Iterator<T>& _in) {
		in = _in.in;
		value = _in.value;
		return *this;
	}
	T& operator*() {
		return value;
	}
	

};


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
	std::cout << "\n\n//\tTrying std::advance and std::distance\n";
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
	std::cout << "\n\n//\tTrying my_advance\n";
	std::list<int>::iterator _m_it_b = lst.begin();
	std::list<int>::iterator _m_it_e = lst.end();

	std::cout << "First iterator is - " << *_m_it_b << std::endl;
	my_advance(_m_it_b, 3);
	std::cout << "But now he is - " << *_m_it_b << std::endl;
	std::cout << "\n\tShow my_distance work\t" << std::distance(_m_it_b, _m_it_e) << std::endl;

	//	4  Const iterator
	std::list<int> n_lst = { 1,2,3,4,5,6,7,8,9 };
	
	std::list<int>::const_iterator c_it_b = n_lst.cbegin();// *c_it_b = 1; -> error

	//	5 reverse iterators
	std::cout << "\n\n//\tTrying reverse iterators\n";
	for (std::list<int>::reverse_iterator r_it = n_lst.rbegin(); r_it != n_lst.rend(); ++r_it) {
		std::cout << *r_it << ' ';
	}std::cout << std::endl;
	

	// 3.2 Trying and testing my 
	// container with iterators
	auto lll = { 1,2,3 };
	int* pp = new int[3];
	std::copy(lll.begin(), lll.end(), pp);
	delete[]pp;

	std::cout << "\n\n\t Try show my_vec with Iterators\n" << std::endl;
	_vector<int> my_vec{ 1,2,3,4,5,6,7,8,9,10,11,12 };

	_vector<int>::Iterator it = my_vec.begin();
	_vector<int>::Const_Iterator c_it = my_vec.cbegin();

	*it = 100;
	//*c_it = 200; error -> const iterator

	std::cout <<"\nFirst my_vec element = " << *it << std::endl;
	std::cout << "\nFirst CONST element  = " << *c_it << std::endl;

	std::cout << "\n\t Try show my_vec with Iterators\n" << std::endl;
	for (_vector<int>::Iterator it = my_vec.begin(); it != my_vec.end(); ++it) {
		std::cout << *it << ' ';
	}

	std::cout << "\n\n\t Try show my_vec with Reverse_Iterators\n" << std::endl;
	for (_vector<int>::Reverse_Iterator r_it = my_vec.rbegin(); r_it != my_vec.rend(); ++r_it) {
		std::cout << *r_it << ' ';
	}
	std::cout << std::endl;
	
	// OUTPUT Iterators
	std::list<int> kekw = { 1,2,3,4,5,6,7,8,9,10,11,12 };
	std::vector<int> lmao;


	std::copy(kekw.begin(), kekw.end(), std::back_inserter(lmao));
	for (auto x : lmao) { std::cout << x << ' '; }
	lmao.clear();
	std::cout << std::endl;


	std::copy(kekw.begin(), kekw.end(),Back_Insert_Iterator<std::vector<int>>(lmao));
	for (auto x : lmao) { std::cout << x << ' '; }
	lmao.clear();
	std::cout << std::endl;

	std::copy(kekw.begin(), kekw.end(), Back_Inserter(lmao));
	for (auto x : lmao) { std::cout << x << ' '; }
	lmao.clear();
	std::cout << std::endl;

	std::copy_if(kekw.begin(), kekw.end(), Back_Inserter(lmao), IsEven());
	for (auto x : lmao) { std::cout << x << ' '; }
	lmao.clear();
	std::cout << std::endl;

	//Testing stream iterators
	std::cout << "\tTesting isteram iterator" << std::endl;
	std::istream_iterator<int> _it(std::cin);
	for (int i = 0; i < 4; ++i, ++_it) {
		lmao.push_back(*_it);
		std::cout << lmao[i] << ' ';
	}
	lmao.clear();
	std::cout << std::endl;

	std::cout << "\tTesting isteram iterator" << std::endl;
	Istream_Iterator<int> my_it(std::cin);
	for (int i = 0; i < 4; ++i, ++my_it) {
		lmao.push_back(*my_it);
		std::cout << lmao[i] << ' ';
	}
	lmao.clear();
	std::cout << std::endl;

	return 0;
}