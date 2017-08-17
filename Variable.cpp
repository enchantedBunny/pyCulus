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
	if (type ==  function)return "( " + tree[0]  + " " + op + " " + tree[1] + " )";
	return "sum( " + tree[0] + " )";
}
std::string Variable::build(){
	if (type == constant)return std::to_string(value);
	if (type == independent)return "v" + std::to_string(id);
	if (type ==  function)return "( " + left->build() + " " + op + " " + right->build() + " )";
	return "sum( " + left->build() + " )";
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
std::string Variable::f(Variable *a, Variable *b, char *o, bool getDerivs)
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
	
	i = 0;
	int ri = 0;
	for(j=0;j<n;j++){
         allOrderedDeps[j] = allDepsWithDuplicates[j];
		 out += std::to_string(allOrderedDeps[j]) + " ";
	}
	countOrderedDeps = n;
	if (getDerivs){
		
		while (i < n){
			if (allOrderedDeps[i] == left->id)break;
			i++;
		}
		while (ri < n){
			if (allOrderedDeps[ri] == right->id)break;
			ri++;
		}
		if (op == '*')
		{	
			derivs[i]=b;
			derivs[ri]=a;
		}
		Variable* blob = new Variable;
		Variable* gblob = new Variable;
		blob->c(1);
		gblob->c(-1);
		if (op == '+'){
			derivs[i]= blob;
			derivs[ri]= blob;
		}
		if (op == '-'){
			derivs[i]= blob;
			derivs[ri]= gblob;
		}
		if (op == '/'){
			Variable* flob = new Variable;
			char* gop = new char;
			*gop = '/';
			flob->f(blob,b, gop, false);	
			derivs[i]= flob;
			Variable* fglob = new Variable;
			char* hop = new char;
			*hop = '*';
			fglob->f(gblob,a, hop);	 //-a
			Variable* klob = new Variable;
			klob->f(b,b, hop);     //b*b
			Variable* tlob = new Variable;
			tlob->f(fglob,klob, gop, false); //-a/b^2
			derivs[ri]= tlob;
		}
	}
	return out;

}
void Variable::m(Variable *a)
{
	type = matrix;
	left = a;
	tree[0] = left->build();
}

Variable::~Variable()
{
}
float Variable::getValue(float **v, int rows){
	float* vals = left->feed(v, rows);
	float out=0;
	for (int i = 0; i<rows;i++){
		out += vals[i];
	}
	return out/rows;
}
float Variable::getValue(float *v)
{
	std::string myType;
	if (type == constant)myType="const";
	if (type == independent)myType="indep";
	if (type == function)myType="func";
	cPrint("I am a " + myType + " and my v[0] is " + std::to_string(v[0]));
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
	if (op == '+')return  left->getValue(lout) + right->getValue(rout);
	if (op == '-')return  left->getValue(lout) - right->getValue(rout);
	if (op == '*')return  left->getValue(lout) * right->getValue(rout);
	if (op == '/')return  left->getValue(lout) / right->getValue(rout);
	return op;
}
float* Variable::feed(float **v, int rows){
	cPrint("feeding " + std::to_string(rows));
	float out[rows]; 
	for (int i = 0; i< rows;i++){
		out[i] = getValue(v[i]);
	}
	fValues = out;
	return fValues;
}
float Variable::getDerivValue(int g,float *v){
	//cPrint(std::to_string(derivs[g]->right->id));
	cPrint(std::to_string(v[0]));
	return derivs[g]->getValue(v);
}