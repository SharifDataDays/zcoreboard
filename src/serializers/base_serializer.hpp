#ifndef BASE_SERIALIZER_H
#define BASE_SERIALIZER_H

#include <jsoncpp/json/json.h>

class BaseSerializer {
public:
	virtual Json::Value parse() = 0;
};

#endif
