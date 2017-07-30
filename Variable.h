namespace calc {
	enum vType {independent, constant, function};
	class Variable {
		public:
			static int biggestId;
			int id;
			int ndeps[2];
			Variable *left;
			Variable *right;
			char op;
			vType type;
			float value;
			Variable();
			void setID(int bId);
			void i();
			void c(float value);
			float f(Variable *a, Variable *b, char *op);
			~Variable();
			float getValue(float *v);
	};
} 
