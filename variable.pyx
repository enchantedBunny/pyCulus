# distutils: language = c++
# distutils: sources = Variable.cpp
from libc.stdlib cimport malloc, free
from libcpp.string cimport string
from itertools import zip_longest
cimport cython
cdef extern from "Variable.h" namespace "calc":
    cpdef enum vType:
            independent, constant, function
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
        void c(float value);
        void i();
        string f(Variable *a, Variable *b, char *op);
cdef int b = 0
cdef public void cPrint(string s):
    print(s.decode('utf8'))
def reduce_mean(pyfloats):
    return sum(pyfloats[0:len(pyfloats)])/len(pyfloats)
    
cdef class var:
    cdef:
        float * cfloats
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
        print(self.thisptr.id)
    def value(self, pyfloats):
        cfloats = <float *> malloc(len(pyfloats)*cython.sizeof(float))
        if cfloats is NULL:
            raise MemoryError()
        for i in range(len(pyfloats)):
            cfloats[i] = pyfloats[i]
        return(self.thisptr.getValue(cfloats))
    def feed(self, floats):
        out = []
        for f in floats:
            cfloats = <float *> malloc(len(f)*cython.sizeof(float))
            if cfloats is NULL:
                raise MemoryError()
            for i in range(len(f)):
                cfloats[i] = f[i]
            out.append(self.thisptr.getValue(cfloats))
        return(out)
    def type(self):
        if self.thisptr.type == constant:
            print('constant')
        if self.thisptr.type == independent:
            print('independent')
        if self.thisptr.type == function:
            print('function')
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
        print(self.thisptr.f(l, r, &s).decode('utf8'))

