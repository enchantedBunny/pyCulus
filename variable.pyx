# distutils: language = c++
# distutils: sources = Variable.cpp
from libc.stdlib cimport malloc, free
from libcpp.string cimport string
from itertools import zip_longest
cimport cython
cdef extern from "Variable.h" namespace "calc":
    cpdef enum vType:
            independent, constant, function, matrix, special
    cdef cppclass Variable:
        int id;
        char op;
        float value;
        vType type;
        string build();
        string preview();
        Variable();
        void setID(int bId);
        float getValue(float *v);
        float getValue(float **v, int rows)
        float* feed(float **v, int rows)
        void c(float value);
        void m(Variable *a);
        void i();
        string f(Variable *a, Variable *b, char *op);
        float* fValues;
        float getDerivValue(int g,float *v);
cdef int b = 0
cdef public void cPrint(string s):
    print(s.decode('utf8'))
def reduce_mean(pyfloats):
    return sum(pyfloats[0:len(pyfloats)])/len(pyfloats)

cdef class var:
    cdef:
        float * cfloats
        float ** cfloatses
        int i
        Variable thisptr      # hold a C++ instance which we're wrapping
        var l
        var r
    def op(self):
        return self.thisptr.op
    def preview(self):
        print(self.thisptr.preview().decode('utf8'))
    def __cinit__(self):
        global b
        b +=1
        self.thisptr = Variable()
        self.thisptr.setID(b);
        print('My id is ',self.thisptr.id)
    def value(self, pyfloats):
        if self.thisptr.type == matrix:
            cfloatses = <float **> malloc(len(pyfloats)*cython.sizeof(float))
            for grop in range(len(pyfloats)):
                cfloats = <float *> malloc(len(pyfloats[grop])*cython.sizeof(float))
                if cfloats is NULL:
                    raise MemoryError()
                for i in range(len(pyfloats[grop])):
                    cfloats[i] = pyfloats[grop][i]
                cfloatses[grop] = cfloats
            return self.thisptr.getValue(cfloatses,len(pyfloats))
        cfloats = <float *> malloc(len(pyfloats)*cython.sizeof(float))
        if cfloats is NULL:
            raise MemoryError()
        for i in range(len(pyfloats)):
            cfloats[i] = pyfloats[i]
        return(self.thisptr.getValue(cfloats))
    def feed(self, floatses):
        cfloatses = <float **> malloc(len(floatses)*cython.sizeof(float))
        for gr in range(len(floatses)):
            cfloats = <float *> malloc(len(floatses[gr])*cython.sizeof(float))
            if cfloats is NULL:
                raise MemoryError()
            for i in range(len(floatses[gr])):
                cfloats[i] = floatses[gr][i]
            cfloatses[gr] = cfloats
        cdef float* glob = self.thisptr.feed(cfloatses, len(floatses))
        out = []
        for i in range(len(floatses)):
            out.append(glob[i])
        return out
    def type(self):
        if self.thisptr.type == constant:
            print('constant')
        if self.thisptr.type == independent:
            print('independent')
        if self.thisptr.type == function:
            print('function')
        if self.thisptr.type == matrix:
            print('matrix')
        if self.thisptr.type == special:
            print('special')
        print(self.thisptr.type)
    def c(self, float x):
        self.thisptr.c(x)
    def i(self):
        self.thisptr.i()
    def f(self, var a, var b, op):
        cdef char s
        s = ord(op[0])
        cdef Variable *l = &a.thisptr
        cdef Variable *r = &b.thisptr
        return self.thisptr.f(l, r, &s).decode('utf8')
    def m(self, var a):
        cdef Variable *l = &a.thisptr
        self.thisptr.m(l)
    def  getDeriv(self,g,pyfloats):
        cfloats = <float *> malloc(len(pyfloats)*cython.sizeof(float))
        if cfloats is NULL:
            raise MemoryError()
        for i in range(len(pyfloats)):
            cfloats[i] = pyfloats[i]
        return(self.thisptr.getDerivValue(g,cfloats))

