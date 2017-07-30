# distutils: language = c++
# distutils: sources = Variable.cpp
from libc.stdlib cimport malloc, free
cimport cython
cdef extern from "Variable.h" namespace "calc":
    cpdef enum vType:
            independent, constant, function
    cdef cppclass Variable:
        char op;
        float value;
        vType type;
        Variable();
        float getValue(float *v);
        void c(float value);
        void i();
        void f(Variable *a, Variable *b, char *op);


cdef class var:
    cdef:
        float * cfloats
        int i
        Variable thisptr      # hold a C++ instance which we're wrapping
        var l
        var r
    def op(self):
        return self.thisptr.op
    def __cinit__(self):
        self.thisptr = Variable()
    def value(self, pyfloats):
        cfloats = <float *> malloc(len(pyfloats)*cython.sizeof(float))
        print('get it')
        if cfloats is NULL:
            raise MemoryError()
        for i in range(len(pyfloats)):
            cfloats[i] = pyfloats[i]
        print(self.thisptr.getValue(cfloats))
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
        self.thisptr.f(l, r, &s)

