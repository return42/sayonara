#ifndef PIMPL_H
#define PIMPL_H

#include <memory>

#define PIMPL(Class) private: \
    struct Private; \
    friend struct Private; \
    std::unique_ptr< Class ::Private> _m;

namespace Pimpl
{
    template<typename T, typename... Args>
    std::unique_ptr<T> make(Args&&... args)
    {
	return std::unique_ptr<T>(
		    new T(std::forward<Args>(args)...)
	);
    }
}




#endif // PIMPL_H
