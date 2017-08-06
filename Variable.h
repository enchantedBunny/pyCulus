#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <algorithm>
namespace calc {
	enum vType {independent, constant, function};
	class Variable {
		public:
			int id;
			std::string tree[2];
			Variable *left;
			Variable *right;
			unsigned short deps[2];
			unsigned short depsList[2][100];
			unsigned short allOrderedDeps[100];
			unsigned short countOrderedDeps;
			char op;
			std::string build();
			void buildTree();
			std::string preview();
			vType type;
			float value;
			Variable();
			void setID(int bId);
			void i();
			void c(float value);
			std::string f(Variable *a, Variable *b, char *op);
			~Variable();
			float getValue(float *v);
	};
} 
