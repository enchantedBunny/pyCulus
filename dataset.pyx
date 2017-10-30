from eigency.core cimport *
import numpy as np
cimport cython
cdef extern from "Dataset.h":
     cdef void _function_w_mat_arg "function_w_mat_arg"(Map[MatrixXd] &)

# This will be exposed to Python
def function_w_mat_arg(np.ndarray array):
    return _function_w_mat_arg(Map[MatrixXd](array))