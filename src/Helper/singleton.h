#ifndef SINGLETON_H
#define SINGLETON_H

#define SINGLETON_QOBJECT(class_name) protected: \
						class_name (QObject* object=0); \
						public: \
						static class_name *getInstance() { static class_name instance; return &instance; } \
						virtual ~class_name ();


#define SINGLETON(class_name) protected: \
						class_name (); \
						public: \
						static class_name *getInstance() { static class_name instance; return &instance; } \
						virtual ~class_name ();

#endif
