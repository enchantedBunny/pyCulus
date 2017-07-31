#include <string>
#include <stdio.h>

#include <stdexcept>
namespace calc {
	enum vType {independent, constant, function};
	class Variable {
		public:
			int id;
			int ndeps[2];
			int depIds[64];
			Variable *left;
			Variable *right;
			char op;
			std::string build();
			vType type;
			float value;
			int deps(Variable *v);
			Variable();
			void setID(int bId);
			void i();
			void c(float value);
			float f(Variable *a, Variable *b, char *op);
			~Variable();
			float getValue(float *v);
	};
} 
