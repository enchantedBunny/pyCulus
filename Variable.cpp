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
	if (type ==  special)return "( " +op + " (" + tree[0]  + ") )";
	return "sum( " + tree[0] + " )";
}
std::string Variable::build(){
	if (type == constant)return std::to_string(value);
	if (type == independent)return "v" + std::to_string(id);
	if (type ==  function)return "( " + left->build() + " " + op + " " + right->build() + " )";
	if (type ==  special)return "( " +op + " (" + left->build() ") )";
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
	
	for(j=0;j<n;j++){
         allOrderedDeps[j] = allDepsWithDuplicates[j];
		 out += std::to_string(allOrderedDeps[j]) + " ";
	}
	countOrderedDeps = n;
	if (getDerivs){
		char* add = new char;
		*add = '+';
		char* mult = new char;
		*mult = '*';
		
		if (op == '*')
		{	
			if (right->id == left->id){
				Variable* nlob = new Variable;
				Variable* fblob = new Variable;
				fblob->c(2);
				nlob->f(fblob,a, mult, false);
				directDerivs[0]= nlob;
				cPrint("a*a");
			}
			else{
				directDerivs[0]=b;
				directDerivs[1]=a;
				cPrint("a*b");
			}
		}
		Variable* blob = new Variable;
		Variable* gblob = new Variable;
		blob->c(1);
		gblob->c(-1);
		if (op == '+'){
			directDerivs[0]= blob;
			directDerivs[1]= blob;
			cPrint("a+b");
		}
		if (op == '-'){
			directDerivs[0]= blob;
			directDerivs[1]= gblob;
			cPrint("a-b");
		}
		if (op == '/'){
			Variable* flob = new Variable;
			char* gop = new char;
			*gop = '/';
			flob->f(blob,b, gop, false);	
			directDerivs[0]= flob;
			Variable* fglob = new Variable;
			char* hop = new char;
			*hop = '*';
			fglob->f(gblob,a, hop, false);	 //-a
			Variable* klob = new Variable;
			klob->f(b,b, hop, false);     //b*b
			Variable* tlob = new Variable;
			tlob->f(fglob,klob, gop, false); //-a/b^2
			directDerivs[1]= tlob;
			cPrint("a/b");
		}
		cPrint("I will iterate " + std::to_string(n));
		for (int gg=0; gg < n; gg++){
			cPrint("find deriv of id"+std::to_string(allOrderedDeps[gg]));
			bool inleft = std::find(std::begin(left->allOrderedDeps), std::end(left->allOrderedDeps), allOrderedDeps[gg]) != std::end(left->allOrderedDeps);
			bool inright = std::find(std::begin(right->allOrderedDeps), std::end(right->allOrderedDeps), allOrderedDeps[gg]) != std::end(right->allOrderedDeps);
			if (left->id == allOrderedDeps[gg])inleft = true;
			else if (left->type !=function) inleft = false;
			if (right->id == allOrderedDeps[gg])inright = true;
			else if (right->type !=function) inright = false;
			cPrint(inleft ? "inleft: true":"inleft: false");
			cPrint(inright ? "inright: true":"inright: false");
			Variable* wip = new Variable;
			if (right->id == allOrderedDeps[gg] && left->id == allOrderedDeps[gg] ){  //a a 
				cPrint("a a");
				derivs[gg] = directDerivs[0]; //which equalts directDerivs[1]
			}
			else if (!inright && left->id == allOrderedDeps[gg] ){   //a b
				cPrint("a b");
				derivs[gg] = directDerivs[0];
			}
			else if (!inleft && right->id == allOrderedDeps[gg]){ //b a
				cPrint("b a");
				derivs[gg] = directDerivs[1];
			}
			else if (inright && left->id == allOrderedDeps[gg]){   //a f(a)
				cPrint("a f(a)");
				int c = 0;
				while (c<left->countOrderedDeps){
					if (left->allOrderedDeps[c] == allOrderedDeps[gg]){
						break;
					}
					c++;
				}
				wip->f(directDerivs[0], left->derivs[c], mult, false);
				Variable* wip2 = new Variable;
				wip2->f(directDerivs[1], wip, add, false);
				derivs[gg] = wip2;
			}
			else if (inleft && right->id == allOrderedDeps[gg]){   //f(a) a
				cPrint("f(a) a");
				int c = 0;
				while (c<left->countOrderedDeps){
					if (left->allOrderedDeps[c] == allOrderedDeps[gg]){
						break;
					}
					c++;
				}
				wip->f(directDerivs[0], left->derivs[c], mult, false);
				Variable* wip2 = new Variable;
				wip2->f(directDerivs[1], wip, add, false);
				derivs[gg] = wip2;
			}
			else if (inleft && inright){   //f(a) f(a)
				cPrint("f(a) f(a)");
				int c = 0;
				while (c<left->countOrderedDeps){
					if (left->allOrderedDeps[c] == allOrderedDeps[gg]){
						break;
					}
					c++;
				}
				wip->f(directDerivs[0], left->derivs[c], mult, false);//left
				c = 0;
				while (c<right->countOrderedDeps){
					if (right->allOrderedDeps[c] == allOrderedDeps[gg]){
						break;
					}
					c++;
				}
				Variable* wip2 = new Variable;
				Variable* wip3 = new Variable;
				wip2->f(directDerivs[1], right->derivs[c], mult, false);//right
				wip3->f(wip, wip2, add, false);
				derivs[gg] = wip3;
			}	
			else if (inleft && !inright){   //f(a) b
				cPrint("f(a) b");
				int c = 0;
				while (c<left->countOrderedDeps){
					if (left->allOrderedDeps[c] == allOrderedDeps[gg]){
						break;
					}
					c++;
				}
				wip->f(directDerivs[0], left->derivs[c], mult, false);
				derivs[gg] = wip;
			}	
			else if (!inleft && inright){   //b f(a)
				cPrint("b f(a)");
				int c = 0;
				while (c<right->countOrderedDeps){
					if (right->allOrderedDeps[c] == allOrderedDeps[gg]){
						break;
					}
					c++;
				}
				wip->f(directDerivs[1], right->derivs[c], mult, false);
				derivs[gg] = wip;
			}	
			else cPrint("probably error :D");	
		}
		
	}
	return "This function has " + std::to_string(countOrderedDeps) + " dependencies";

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
	if (type == independent)return 42.199;


	if (op == '+')return  left->getValue(lout) + right->getValue(rout);
	if (op == '-')return  left->getValue(lout) - right->getValue(rout);
	if (op == '*')return  left->getValue(lout) * right->getValue(rout);
	if (op == '/')return  left->getValue(lout) / right->getValue(rout);
	return -42;
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
	float* pass = new float[derivs[g]->countOrderedDeps];
	int help = 0;
	for (int i=0; i<countOrderedDeps; i++){
		bool inDeriv;
		if (derivs[g]->type == function || derivs[g]->type == special)
		inDeriv = std::find(std::begin(derivs[g]->allOrderedDeps), std::end(derivs[g]->allOrderedDeps), allOrderedDeps[i]) != std::end(derivs[g]->allOrderedDeps);
		else if (derivs[g]->type == independent && derivs[g]->id == allOrderedDeps[i])
		inDeriv = true;
		else
		inDeriv = false;
		cPrint(inDeriv ? std::to_string(allOrderedDeps[i])+ "is in": std::to_string(allOrderedDeps[i])+ "is not in");
		if (inDeriv){
			pass[help] = v[i];
			help++;
		}
	}
	return derivs[g]->getValue(pass);
}