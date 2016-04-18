#ifndef SET_H
#define SET_H

#include <set>

#include <QList>

namespace SP {
	template<typename T>
	class Set :
			public std::set<T>
	{

	public:

		Set() :
			std::set<T>()
		{

		}


		Set(const T& one_element) :
			Set()
		{
			this->insert(one_element);
		}


		QList<T> toList() const
		{
			QList<T> ret;
			for(auto it=this->begin(); it!=this->end(); it++){
				ret << *it;
			}
			return ret;
		}


		bool isEmpty() const
		{
			return (this->size() == 0);
		}


		T first() const
		{
			return *(this->begin());
		}


		bool contains(const T& value) const
		{
			auto it = this->find(value);
			return (it != this->end());
		}

		void remove(const T& value)
		{
			auto it = this->find(value);
			if(it != this->end()){
				this->erase(it);
			}
		}
	};
}

#endif // SET_H
