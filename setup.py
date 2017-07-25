from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

setup(ext_modules=[Extension("pyCalc", ["variable.pyx", "Variable.cpp"], language="c++",)],
      cmdclass = {'build_ext': build_ext})
