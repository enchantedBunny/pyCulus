#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <algorithm>
#include <math.h>
namespace calc {
	enum vType {independent, constant, function, matrix, special};
	class Deriv;
	class Variable {
		public:
			int id;
			int pw;
			std::string tree[2];
			Variable *left;
			Variable *right;
			unsigned short deps[2];
			unsigned short depsList[2][100];
			unsigned short allOrderedDeps[100];
			unsigned short countOrderedDeps;
			std::string op;
			std::string build();
			void buildTree();
			std::string preview();
			vType type;
			float value;
			Variable();
			void setID(int bId);
			void i();
			void m(Variable *a);
			void c(float value);
			std::string f(Variable *a, Variable *b, std::string *op, bool getDerivs = true);
			std::string f(Variable *a, std::string *op, bool getDerivs = true);
			std::string f(Variable *a, std::string *op, int p, bool getDerivs = true);
			~Variable();
			float getValue(float *v);
			float getValue(float **v, int rows);
			float* feed(float **v, int rows);
			float* fValues;
			Variable* derivs[100];
			Variable* directDerivs[2];
			float getDerivValue(int g,float *v);
	};
	class Deriv {
		public:
			char op;
			Variable *left;
			Variable *right;
	};
} 
