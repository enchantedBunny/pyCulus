#include "Variable.h"
#include <Python.h>
#include "master.h"
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
	if (op ==  "pow")cPrint2(std::to_string(pw));
	if (type ==  special)return "( " + op + tree[0] + " )";
	return "sum( " + tree[0] + " )";
}
std::string Variable::build(){
	if (type == constant)return std::to_string(value);
	if (type == independent)return "v" + std::to_string(id);
	if (type ==  function)return "( " + left->build() + " " + op + " " + right->build() + " )";
	if (type ==  special)return "( " + op + left->build() + " )";
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
std::string Variable::f(Variable *a, Variable *b, std::string *o, bool getDerivs)
{
	type = function;
	left = a;
	right = b;
	op = *o;
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
		std::string* add = new std::string;
		*add = "+";
		std::string* mult = new std::string;
		*mult = "*";
		
		if (op == "*")
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
		if (op == "+"){
			directDerivs[0]= blob;
			directDerivs[1]= blob;
			cPrint("a+b");
		}
		if (op == "-"){
			directDerivs[0]= blob;
			directDerivs[1]= gblob;
			cPrint("a-b");
		}
		if (op == "/"){
			Variable* flob = new Variable;
			std::string* gop = new std::string;
			*gop = "/";
			flob->f(blob,b, gop, false);	
			directDerivs[0]= flob;
			Variable* fglob = new Variable;
			std::string* hop = new std::string;
			*hop = "*";
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
			//cPrint("find deriv of id"+std::to_string(allOrderedDeps[gg]));
			//cPrint("right has deps: " + std::to_string(right->countOrderedDeps));
			//cPrint("right has a dep: " + std::to_string(right->allOrderedDeps[0]));
			bool inleft = std::find(std::begin(left->allOrderedDeps), std::end(left->allOrderedDeps), allOrderedDeps[gg]) != std::end(left->allOrderedDeps);
			bool inright = std::find(std::begin(right->allOrderedDeps), std::end(right->allOrderedDeps), allOrderedDeps[gg]) != std::end(right->allOrderedDeps);
			//cPrint("bef");
			//cPrint(inleft ? "inleft: true":"inleft: false");
			//cPrint(inright ? "inright: true":"inright: false");
			if (left->id == allOrderedDeps[gg] || left->countOrderedDeps == 1)inleft = true;
			else if (left->type ==independent || left->type ==constant) inleft = false;
			if (right->id == allOrderedDeps[gg] || right->countOrderedDeps == 1)inright = true;
			else if (right->type ==independent || right->type ==constant) inright = false;
			//cPrint(inleft ? "inleft: true":"inleft: false");
			//cPrint(inright ? "inright: true":"inright: false");
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

					//cPrint("it's " + std::to_string(c));
						break;
					}
					c++;
				}
				Variable* wip2 = new Variable;
				wip->f(directDerivs[1], right->derivs[c], mult, false);
				derivs[gg] = wip;
				float * tttt = new float[5];
				tttt[0] = 2;
				tttt[1] = 2;
				tttt[2] = 2;
				tttt[3] = 2;
				tttt[4] = 2;
				cPrint("++++right-> getDerivValue(c is " +std::to_string(right->getDerivValue(c, tttt)));
				cPrint("++++right-> derivs[c] value is " +std::to_string(right->derivs[c]->getValue(tttt)));
				cPrint("++++directDerivs[1] value is " +std::to_string(directDerivs[1]->getValue(tttt)));
				cPrint("++++wip value is " +std::to_string(wip->getValue(tttt)));
				//cPrint("gg is " + std::to_string(gg));
				cPrint(wip->preview());
				//cPrint((*mult=='*')? "yes":"no");
			}	
			else cPrint("probably error :D");
			cPrint2(derivs[gg]->preview());
		}
		
	}
	if (getDerivs){
		return "the 0th deriv of this is" + derivs[0]->preview();
	}
	return "This function has " + std::to_string(countOrderedDeps) + " dependencies";

}
std::string Variable::f(Variable *a, std::string* o, bool getDerivs)
{
	type = special;
	left = a;
	std::string gop = static_cast<std::string>(*o);
	tree[0] = left->build();
	op = gop;
	std::string* e = new std::string;
	*e = "exp";
	std::string* sigmoid = new std::string;
	*sigmoid = "sig";
	std::string* mult = new std::string;
	*mult = "*";
	std::string* minus = new std::string;
	*minus = "-";
	if (op == "exp")
	{
		cPrint("wohoo");
		if (left->type ==function){
			countOrderedDeps = left->countOrderedDeps;
			for (int g = 0; g < countOrderedDeps; g++){
				allOrderedDeps[g] = left->allOrderedDeps[g];
				cPrint(std::to_string(left->allOrderedDeps[g]));
			}
			if (getDerivs){
				Variable* wip2 = new Variable;
				wip2->f(left, e, false);
				for (int g = 0; g < countOrderedDeps; g++){
						Variable* wip = new Variable;
						wip->f(left->derivs[g], wip2, mult, false);
						derivs[g] = wip;
						cPrint2(derivs[g]->preview());
				}
			}
			
			cPrint(std::to_string(countOrderedDeps));

		}
		if (left->type == independent){
			countOrderedDeps = 1;
			allOrderedDeps[0] = left->id;
			if (getDerivs){
				Variable* wip2 = new Variable;
				wip2->f(left, e, false);
				derivs[0] = wip2;
			}
		}
	
	}
	if (op == "sig")
	{
		cPrint("wohoo");
		if (left->type ==function){
			countOrderedDeps = left->countOrderedDeps;
			for (int g = 0; g < countOrderedDeps; g++){
				allOrderedDeps[g] = left->allOrderedDeps[g];
				cPrint(std::to_string(left->allOrderedDeps[g]));
			}
			if (getDerivs){
				Variable* wip2 = new Variable;
				Variable* wip3 = new Variable;
				Variable* wip4 = new Variable;
				Variable* wip5 = new Variable;
				wip2->f(left, sigmoid, false);
				wip3->c(1);
				wip4->f(wip3, wip2, minus,false);
				wip5->f(wip4, wip2, mult,false);
				for (int g = 0; g < countOrderedDeps; g++){
						Variable* wip = new Variable;
						wip->f(left->derivs[g], wip5, mult, false);
						derivs[g] = wip;
						cPrint2(derivs[g]->preview());
				}
			}
			
			cPrint(std::to_string(countOrderedDeps));

		}
		if (left->type == independent){
			countOrderedDeps = 1;
			allOrderedDeps[0] = left->id;
			if (getDerivs){
				Variable* wip2 = new Variable;
				Variable* wip3 = new Variable;
				Variable* wip4 = new Variable;
				Variable* wip5 = new Variable;
				wip2->f(left, sigmoid, false);
				wip3->c(1);
				wip4->f(wip3, wip2, minus,false);
				wip5->f(wip4, wip2, mult,false);
				derivs[0] = wip5;
			}
		}
	
	}
	if (getDerivs){
		return "the 0th deriv of this is" + derivs[0]->preview();
	}
	return "hope" ;

}
std::string Variable::f(Variable *a, std::string *o, int p, bool getDerivs){
	type = special;
	left = a;
	std::string gop = static_cast<std::string>(*o);
	tree[0] = left->build();
	op = gop;
	pw = p;
	std::string* pwt = new std::string;
	*pwt = "pow";
	std::string* mult = new std::string;
	*mult = "*";
	if (op == "pow")
	{
		if (left->type ==function){
			countOrderedDeps = left->countOrderedDeps;
			for (int g = 0; g < countOrderedDeps; g++){
				allOrderedDeps[g] = left->allOrderedDeps[g];
				cPrint(std::to_string(left->allOrderedDeps[g]));
			}
			if (getDerivs){
					Variable* c1 = new Variable;
					c1->c(pw);  //3
					Variable* wip2 = new Variable;
					Variable* wip3 = new Variable;
					if (pw == 2){
						wip3->f(left, c1, mult, false);//2x
					}
					else{
						wip2->f(left, pwt, pw-1, false);//x^2
						wip3->f(wip2, c1, mult, false);//3x^2
					}
					for (int g = 0; g < countOrderedDeps; g++){
						if (getDerivs){
							Variable* wip = new Variable;
							wip->f(left->derivs[g], wip3, mult, false);
							derivs[g] = wip;
							cPrint2(derivs[g]->preview());
						}
					
				}
			}
			
			cPrint(std::to_string(countOrderedDeps));

		}
		if (left->type == independent){
			countOrderedDeps = 1;
			allOrderedDeps[0] = left->id;
			if (getDerivs){
				Variable* c1 = new Variable;
				c1->c(pw);
				Variable* wip2 = new Variable;
				wip2->f(left, pwt, pw-1, false); //x^n-1
				Variable* wip3 = new Variable;
				wip3->f(wip2, c1, mult, false);
				derivs[0] = wip3;
			}
		}
	
	}
	if (getDerivs){
		return "the 0th deriv of this is" + derivs[0]->preview();
	}
	return "hope" ;
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
	if (type == special){
		if (op == "exp"){
			float ev = exp(left->getValue(v));
			cPrint( "I am special" + std::to_string(ev));
			return ev;
		}
		if (op == "sig"){
			float ev = 1/(1+exp(-left->getValue(v)));
			cPrint( "I am special" + std::to_string(ev));
			return ev;
		}
		if (op == "pow"){
			float ev = std::pow(left->getValue(v), pw);
			return ev;
		}
	}
	if(left->id == right->id && op == "*"){
		float lv = left->getValue(v);
		return lv*lv;
	}
	float lout [deps[0]];
	float rout [deps[1]];
	for (unsigned short i = 0; i<countOrderedDeps; i++){
		//int x = std::distance(depsList[0], std::find(depsList[0], depsList[0] + deps[0], allOrderedDeps[i]));
		for (unsigned short hh = 0; hh<deps[0];hh++){
			if (depsList[0][hh] == allOrderedDeps[i]){
				lout[hh] = v[i];
			}
		}
		for (unsigned short hh = 0; hh<deps[1];hh++){
			if (depsList[1][hh] == allOrderedDeps[i]){
				rout[hh] = v[i];
			}
		}
	}
	float lv = left->getValue(lout);
	float rv = right->getValue(rout);
	cPrint( "left value is " + std::to_string(lv));
	cPrint( "right value is " + std::to_string(rv));

	if (op == "+")return  lv + rv;
	if (op == "-")return  lv - rv;
	if (op == "*")return  lv * rv;
	if (op == "/")return  lv / rv;
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
	cPrint2(derivs[g]->preview());
	int help = 0;
	for (int i=0; i<countOrderedDeps; i++){
		bool inDeriv;
		if (derivs[g]->type == function || derivs[g]->type == special)
		inDeriv = std::find(std::begin(derivs[g]->allOrderedDeps), std::end(derivs[g]->allOrderedDeps), allOrderedDeps[i]) != std::end(derivs[g]->allOrderedDeps);
		else if (derivs[g]->type == independent && derivs[g]->id == allOrderedDeps[i])
		inDeriv = true;
		else
		inDeriv = false;
		if (inDeriv){
			pass[help] = v[i];
			help++;
		}
	}
	return derivs[g]->getValue(pass);
}