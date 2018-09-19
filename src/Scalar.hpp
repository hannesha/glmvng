#pragma once

#include <iostream>

struct Scalar{
	enum class Type{INT, LONG_LONG, FLOAT} type;
	union Value{
		int i;
		long long ll;
		float f;
	} value;
	Scalar(const float f): type(Type::FLOAT) { value.f = f; }
	Scalar(const double f): type(Type::FLOAT) { value.f = f; }
	Scalar(const int i): type(Type::INT) { value.i = i; }
	Scalar(const long ll): type(Type::LONG_LONG) { value.ll = ll; }
	Scalar(const long long ll): type(Type::LONG_LONG) { value.ll = ll; }
	
	Scalar(): type(Type::FLOAT) { value.f = 0; }

	std::ostream& operator<<(std::ostream& os){
		switch(type){
			case Scalar::Type::INT:
				os << "int:" << value.i;
				break;
			case Scalar::Type::LONG_LONG:
				os << "long long:" << value.ll;
				break;

			case Scalar::Type::FLOAT:
				os << "float:" << value.f;
				break;
		}
		return os;
	}
};
