#pragma once

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <iostream>

namespace sjtu {
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector {
public:
	class iterator;
	friend class iterator;
private:
	T *data;
	int currentLength;
	int maxsize;

	void doubleSpace()
	{
		if (maxsize <= 16)
        {
            //std::cout << "space is doubled" << std::endl;
			maxsize = 16;
        }
		T * cmp = (T*)malloc(sizeof(T)* (this->maxsize * 2+1));
		for (int i = 0; i < currentLength; ++i)
		{
			new (cmp + i) T(*(data + i));
		}
		maxsize = maxsize * 2;
		for (int i = 0; i < currentLength; ++i)
		{
			(data + i)->~T();
		}
		free(data);
		data = cmp;
	}

public:
	/**
	 * TODO
	 * a type for actions of the elements of a vector, and you should write
	 *   a class named const_iterator with same interfaces.
	 */
	/**
	 * you can see RandomAccessIterator at CppReference for help.
	 */
	class const_iterator;
	class iterator {
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
		T *Toelement;
		vector *Tovector;
	public:
		iterator() = default;

		iterator(const iterator &) = default;

		iterator &operator=(const iterator &) = default;

		iterator(vector *a , T * b)
		{
			Tovector = a;
			Toelement = b;
		}
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		iterator operator+(const int &n) const {
			//TODO
			iterator cmp(Tovector, Toelement + n);
			return cmp;
		}
		iterator operator-(const int &n) const {
			//TODO
			iterator cmp(Tovector, Toelement - n);
			return cmp;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			//TODO
			if (rhs.Tovector == Tovector)
			{
				int cmp = Toelement - rhs.Toelement;
				return cmp;
			}
			else throw invalid_iterator();
		}
		iterator operator+=(const int &n) {
			//TODO
			Toelement += n;
			return iterator(Tovector, Toelement);
		}
		iterator operator-=(const int &n) {
			//TODO
			Toelement -= n;
			return iterator(Tovector, Toelement);
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int)
		{
			T* cmp;
			cmp = Toelement;
			Toelement += 1;
			return iterator(Tovector, cmp);
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++()
		{
			Toelement += 1;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int)
		{
			T* cmp;
			cmp = Toelement;
			Toelement -= 1;
			return iterator(Tovector, cmp);
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--()
		{
			Toelement -= 1;
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const
		{
			return *Toelement;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const
		{
			if (rhs.Toelement == Toelement)
			{
				return true;
			}
			else return false;
		}
		bool operator==(const const_iterator &rhs) const
		{
			if (rhs == Toelement)
			{
				return true;
			}
			return false;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const
		{
			if (rhs.Toelement != Toelement)
			{
				return true;
			}
			return false;
		}
		bool operator!=(const const_iterator &rhs) const
		{
			if (rhs != Toelement)
			{
				return true;
			}
			return false;
		}
	};
	/**
	 * TODO
	 * has same function as iterator, just for a const object.
	 */
	class const_iterator
	{
	private:
		/**
		* TODO add data members
		*   just add whatever you want.
		*/
		T *Toelement;
		const vector *Tovector;
	public:
		const_iterator() = default;

		const_iterator(const iterator & a)
		{
			Toelement = a.Toelement;
			Tovector = a.Tovector;
		}

		const_iterator &operator=(const iterator & a)
		{
			Toelement = a.Toelement;
			Tovector = a.Tovector;
			return this;
		}

		/*const_iterator(vector *a, T * b)
		{
			Tovector = a;
			Toelement = b;
		}*/

		const_iterator(const vector *a, T * b) :Tovector(a), Toelement(b) {}
	public:
		/**
		* return a new iterator which pointer n-next elements
		*   even if there are not enough elements, just return the answer.
		* as well as operator-
		*/
		const_iterator operator+(const int &n) const {
			//TODO
			const_iterator cmp(Tovector, Toelement + n);
			return cmp;
		}
		const_iterator operator-(const int &n) const {
			//TODO
			const_iterator cmp(Tovector, Toelement - n);
			return cmp;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const const_iterator &rhs) const {
			//TODO
			if (rhs.Tovector == Tovector)
			{
				int cmp = rhs.Toelement - Toelement;
				return cmp;
			}
			else throw invalid_iterator();
		}
		const_iterator operator+=(const int &n) {
			//TODO
			Toelement += n;
			return const_iterator(Tovector, Toelement);
		}
		const_iterator operator-=(const int &n) {
			//TODO
			Toelement -= n;
			return const_iterator(Tovector, Toelement);
		}
		/**
		* TODO iter++
		*/
		const_iterator operator++(int)
		{
			T* cmp;
			cmp = Toelement;
			Toelement += 1;
			return const_iterator(Tovector, cmp);
		}
		/**
		* TODO ++iter
		*/
		const_iterator& operator++()
		{
			Toelement += 1;
			return *this;
		}
		/**
		* TODO iter--
		*/
		const_iterator operator--(int)
		{
			T* cmp;
			cmp = Toelement;
			Toelement -= 1;
			return const_iterator(Tovector, cmp);
		}
		/**
		* TODO --iter
		*/
		const_iterator& operator--()
		{
			Toelement -= 1;
			return *this;
		}
		/**
		* TODO *it
		*/
		const T& operator*() const
		{
			return *Toelement;
		}
		/**
		* a operator to check whether two iterators are same (pointing to the same memory).
		*/
		bool operator==(const iterator &rhs) const
		{
			if (rhs == Toelement)
			{
				return true;
			}
			else return false;
		}
		bool operator==(const const_iterator &rhs) const
		{
			if (rhs.Toelement == Toelement)
			{
				return true;
			}
			return false;
		}
		/**
		* some other operator for iterator.
		*/
		bool operator!=(const iterator &rhs) const
		{
			if (rhs.Toelement != Toelement)
			{
				return true;
			}
			return false;
		}
		bool operator!=(const const_iterator &rhs) const
		{
			if (rhs.Toelement != Toelement)
			{
				return true;
			}
			return false;
		}
	};
	/**
	 * TODO Constructs
	 * Atleast three: default constructor, copy constructor and a constructor for std::vector
	 */
	vector()
	{
        currentLength = 0;
        maxsize = 0;
        data = nullptr;
	}
	vector(const vector &other)
	{
		data = (T*)malloc(sizeof(T)* other.maxsize);
		//cout << "malloc" <<endl;
		for (int i = 0; i < other.currentLength; ++i)
		{
			new (data + i) T(*(other.data + i));
		}
		//cout << "new" <<endl;
		currentLength = other.currentLength;
		maxsize = other.maxsize;
	}
	/**
	 * TODO Destructor
	 */
	void clear()
	{
		for (int i = 0; i < currentLength; ++i)
		{
			(data + i)->~T();
		}
		free(data);
	}

	~vector()
	{
		for (int i = 0; i < currentLength; ++i)
		{
			(data + i)->~T();
		}
		free(data);
	}
	/**
	 * TODO Assignment operator
	 */
	vector &operator=(const vector &other)
	{
		clear();
		//cout << "clear" <<endl;
		data = (T*)malloc(sizeof(T)* other.maxsize);
		//cout << "malloc" << endl;
		for (int i = 0; i < other.currentLength; ++i)
		{
			new (data + i) T(*(other.data + i));
		}
		//cout << "new" << endl;
		currentLength = other.currentLength;
		maxsize = other.maxsize;
		return *this;
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T & at(const size_t &pos)
	{
		if (pos > currentLength)
		{
			throw index_out_of_bound();
		}
		else
			return *(data + pos);
	}
	const T & at(const size_t &pos) const
	{
		if (pos > currentLength)
		{
			throw index_out_of_bound();
		}
		else
			return *(data + pos);
	}
	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T & operator[](const size_t &pos)
	{
		if (pos > currentLength)
		{
			throw index_out_of_bound();
		}
		else
			return *(data + pos);
	}
	const T & operator[](const size_t &pos) const
	{
		if (pos > currentLength)
		{
			throw index_out_of_bound();
		}
		else
			return *(data + pos);
	}
	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T & front() const
	{
		if (currentLength == 0)
		{
			throw container_is_empty();
		}
		else
			return *data;
	}
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T & back() const
	{
		if (currentLength == 0)
		{
			throw container_is_empty();
		}
		else
			return *(data + currentLength -1);
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin()
	{
		iterator cmp(this, data);
		return cmp;
	}
	const_iterator cbegin() const
	{
		const_iterator cmp(this, data);
		return cmp;
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end()
	{
		iterator cmp(this, data + currentLength);
		return cmp;
	}
	const_iterator cend() const
	{
		const_iterator cmp(this, data + currentLength);
		return cmp;
	}
	/**
	 * checks whether the container is empty
	 */
	bool empty() const
	{
		return currentLength == 0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const
	{
		return currentLength;
	}
	/**
	 * returns the number of elements that can be held in currently allocated storage.
	 */
	size_t capacity() const
	{
		return maxsize;
	}
	/**
	 * clears the contents
	 */
	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value)
	{
		//std::cout << "currentLength = " << currentLength << std:: endl;
		//std :: cout << "maxsize = " << maxsize << std :: endl;
		if (currentLength >= maxsize)
		{
			int cmp = pos - begin();
			doubleSpace();
			pos = begin() + cmp;
		}
		for(iterator i = end() ; i != pos ; --i)
		{
			//std::cout << i - begin() << std::endl;
			*i = *(i - 1);
		}
		*pos = value;
		currentLength += 1;
		return pos;
	}
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value is true
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t &ind, const T &value)
	{
		if (currentLength >= maxsize)
		{
			//std::cout << "doubled" << std::endl;
			doubleSpace();
		}
		for (int i = currentLength ; i > ind ; --i)
		{
			*(data + i) = *(data + i - 1);
		}
		currentLength += 1;
		*(data + ind) = value;
		return iterator(this, data + ind);
	}
	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos)
	{
		for (iterator i = pos; i != end(); ++i)
		{
			*i = *(i + 1);
		}
		currentLength -= 1;
		if (pos == end() - 1)
		{
			return end();
		}
		else
			return pos;
	}
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
	iterator erase(const size_t &ind)
	{
		for (int i = ind; i <= currentLength; ++i)
		{
			*(data + i) = *(data + i + 1);
		}
		currentLength -= 1;
		iterator cmp(this, data + ind);
		return cmp;
	}
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value)
	{
		//std :: cout << "current = "<<currentLength << " maxsize = " <<  maxsize << std::endl;
		if (currentLength >= maxsize)
		{
			//std::cout << "space is full" << std::endl;
			doubleSpace();
		}
		new (data + currentLength) T(value);
		currentLength += 1;

	}
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back()
	{
		if (currentLength == 0)
		{
			throw container_is_empty();
		}
		(data+currentLength - 1) -> ~T();
		currentLength -= 1;
	}
};


}