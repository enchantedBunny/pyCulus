#include "Variable.h"
#include <stdio.h>

using namespace calc;

Variable::Variable()
{
}
void Variable::i()
{
	type = independent;
}
void Variable::c(float v)
{
	type = constant;	
	value = v;
}
void Variable::f(Variable *a, Variable *b, char *o)
{
	type = function;
	left = a;
	right = b;
	op = (char)*o;
}
Variable::~Variable()
{
}

float Variable::getValue(float *v)
{
	if (type == constant)return value;
	if (type == independent)return v[0];
	float *l  = &v[0];
	if (op == '+')return  left->getValue(l) + right->getValue(l);
	if (op == '-')return  left->getValue(v) - right->getValue(v);
	if (op == '*')return  left->getValue(v) * right->getValue(v);
	if (op == '/')return  left->getValue(v) / right->getValue(v);
}
