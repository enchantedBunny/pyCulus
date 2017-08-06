#include "Variable.h"
#include <Python.h>
#include "variable.h"
using namespace calc;
// 	cPrint("preview:"); will print preview :)
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
std::string Variable::preview(){
	if (type == constant)return "c";
	if (type == independent)return "v" + std::to_string(id);
	return "( " + tree[0]  + " " + op + " " + tree[1] + " )";
}
std::string Variable::build(){
	if (type == constant)return std::to_string(value);
	if (type == independent)return "v" + std::to_string(id);
	return "( " + left->build() + " " + op + " " + right->build() + " )";
}
void Variable::buildTree(){
	tree[0] = left->build();
	tree[1] = right->build();	
}
unsigned short getFirst(std::string &s){
	size_t l = s.find('v');
	s[l]='g';
	std::string out = "";
	if (s.length()>l+1)if ('0' <= s[l+1] && s[l+1] <= '9'){
		out += s[l+1];
		if (s.length()>l+2)if ('0' <= s[l+2] && s[l+2] <= '9'){
			out += s[l+2];
			if (s.length()>l+3)if ('0' <= s[l+3] && s[l+3] <= '9')
				out += s[l+3];
		}
	}
	return atoi(out.c_str());
}
std::string Variable::f(Variable *a, Variable *b, char *o)
{
	type = function;
	left = a;
	right = b;
	op = (char)*o;
	buildTree();
	for (int i = 0;i<2; i++){
		deps[i] = (unsigned short)std::count(tree[i].begin(), tree[i].end(), 'v');
		
		
	}
	int i2 = 0;
	while (tree[0].find('v') != std::string::npos){depsList[0][i2] = getFirst(tree[0]);i2++;}
	i2 = 0;
	while (tree[1].find('v') != std::string::npos){depsList[1][i2] = getFirst(tree[1]);i2++;}
	int n = deps[0] + deps[1];
	unsigned short * allDepsWithDuplicates = new unsigned short[n];
	std::copy(depsList[0], depsList[0] + deps[0], allDepsWithDuplicates);
	std::copy(depsList[1], depsList[1] + deps[1], allDepsWithDuplicates + deps[0]);	
	unsigned short i, j, k;
	for(i=0;i<n;i++)
        for(j=i+1;j<n;)
        {
            if(allDepsWithDuplicates[i]==allDepsWithDuplicates[j])
            {
                for(k=j;k<n-1;++k)
                    allDepsWithDuplicates[k]=allDepsWithDuplicates[k+1];
                --n;
            }
            else
                ++j;
        }
    std::sort(allDepsWithDuplicates, allDepsWithDuplicates+n);
	std::string out = "";
    for(i=0;i<n;i++){
         allOrderedDeps[i] = allDepsWithDuplicates[i];
		 out += std::to_string(allOrderedDeps[i]) + " ";
	}
	countOrderedDeps = n;

	return out;

}

Variable::~Variable()
{
}

float Variable::getValue(float *v)
{
   //auto start = std::chrono::high_resolution_clock::now();
   	if (type == constant)return value;
	if (type == independent)return v[0];
	float lout [deps[0]];
	float rout [deps[1]];
	for (unsigned short i = 0; i<countOrderedDeps; i++){
		int x = std::distance(depsList[0], std::find(depsList[0], depsList[0] + deps[0], allOrderedDeps[i]));
		if (depsList[0][x] == allOrderedDeps[i]){
			lout[x] = v[i];
		}
		x = std::distance(depsList[1], std::find(depsList[1], depsList[1] + deps[1], allOrderedDeps[i]));
		if (depsList[1][x] == allOrderedDeps[i]){
			rout[x] = v[i];
		}

	}
	//auto finish = std::chrono::high_resolution_clock::now();
	//cPrint(std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count()));
	if (op == '+')return  left->getValue(lout) + right->getValue(rout);
	if (op == '-')return  left->getValue(lout) - right->getValue(rout);
	if (op == '*')return  left->getValue(lout) * right->getValue(rout);
	if (op == '/')return  left->getValue(lout) / right->getValue(rout);
	return op;
}
