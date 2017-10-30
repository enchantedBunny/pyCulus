from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
import eigency
setup(ext_modules=[Extension("pyCalc", ["master.pyx", "Variable.cpp", "Dataset.cpp"], language="c++",)],
      cmdclass = {'build_ext': build_ext},
      include_dirs = [".", "pyCulus"] + eigency.get_includes()
      )
