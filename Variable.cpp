#include "Variable.h"
#include <stdio.h>
using namespace calc;
Variable::Variable()
{
}
void Variable::setID(int bId)
{
	id = bId;
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
int deps(Variable *v){
	if (v->type == independent)
		return 1;
	else if (v->ndeps[0] != NULL)
		return v->ndeps[0] + v->ndeps[1];
	return 0;
};
float Variable::f(Variable *a, Variable *b, char *o)
{
	type = function;
	left = a;
	right = b;
	op = (char)*o;
	
	ndeps[0] = deps(left);
	ndeps[1] = deps(right);
	return ndeps[0] + ndeps[1];

}
Variable::~Variable()
{
}

float Variable::getValue(float *v)
{
	if (type == constant)return value;
	if (type == independent)return v[0];
	float *l  = &v[0];
	float *r  = &v[1];
	if (op == '+')return  left->getValue(l) + right->getValue(r);
	if (op == '-')return  left->getValue(l) - right->getValue(r);
	if (op == '*')return  left->getValue(l) * right->getValue(r);
	if (op == '/')return  left->getValue(l) / right->getValue(r);
}
