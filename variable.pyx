# distutils: language = c++
# distutils: sources = Rectangle.cpp
cdef extern from "Variable.h" namespace "calc":
    cdef cppclass Variable:
        float value;
        Variable() except +;
        Variable(float value);
        Variable(float h, Variable a, Variable b, char op);
        float getValue();

cdef class _Variable:
    cdef Variable thisptr      # hold a C++ instance which we're wrapping
    def __cinit__(self,float x,   _Variable a = None, _Variable b = None, char g = None):
        if x == None:
            if a == None:
                self.thisptr = Variable()
            else:
                self.thisptr = Variable(5,a.thisptr,b.thisptr,g)
        if x != None:
            self.thisptr = Variable(x)
    def getValue(self):
        print('get it')
        self.thisptr.getValue()

