namespace calc {
	enum vType {independent, constant, function};
	class Variable {
		public:
			Variable *left;
			Variable *right;
			char op;
			vType type;
			float value;
			Variable();
			void i();
			void c(float value);
			void f(Variable *a, Variable *b, char *op);
			~Variable();
			float getValue(float *v);
	};
} 
