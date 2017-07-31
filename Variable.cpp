#include "Variable.h"
#include <stdio.h>
#include <stdexcept>
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
int Variable::deps(Variable *v){
	int out = 0;
	if (v->type == independent){	
		for(int i = 0; i<64; i++) {
			if (depIds[i] == v->id){
				return 0;
			}
    		else if(depIds[i] == 0)
    		{
        		depIds[i] = v->id;
        		return 1;
    		}
		}
		throw std::invalid_argument( "received negative value" );
		return 0;
	}
	else if (v->type == function){
		for(int i = 0; i<64; i++) {
			if (v->depIds[i] != 0){
				for(int g = 0; g<64; g++) {
					if (depIds[g] == v->depIds[i]){
						break;
					}
    				else if(depIds[g] == 0)
    				{
        				depIds[g] = v->depIds[i];
						out ++;
        				break;
    				}
				}
			}
			else return out;
		}
		return out;
	}
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
	return -199.42;
}
