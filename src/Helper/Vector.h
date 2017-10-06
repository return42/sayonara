#ifndef VECTOR_H
#define VECTOR_H

#include <vector>

namespace SP
{
	template<typename T>
	class Vector : std::vector<T>
	{
		public:
			int count() const
			{
				return static_cast<int>(this->size());
			}

			Vector<T>& operator <<(const T& t)
			{
				this->push_back(t);
				return &this;
			}

			Vector<T>& operator <<(T&& t)
			{
				this->push_back(std::move(t));
				return &this;
			}

			const T& first() const
			{
				return this->at(0);
			}

			const T& last() const
			{
				return this->at(size() - 1);
			}
	};
}

#endif // VECTOR_H
