#ifndef PIMPL_H
#define PIMPL_H

#include <memory>

#define PIMPL(Class) private: \
    struct Private; \
    friend class Private; \
    std::unique_ptr< Class ::Private> _m;

namespace Pimpl
{
    template<typename T>
    std::unique_ptr<T> make()
    {
	return std::unique_ptr<T>(new T());
    }
}




#endif // PIMPL_H
