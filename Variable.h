namespace calc {
	class Variable {
		public:
			float value;
			Variable();
			Variable(float value);
			Variable(float h, Variable a, Variable b, char op);
			~Variable();
			float getValue();
	};
} 
