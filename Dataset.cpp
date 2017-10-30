#include "Dataset.h"
#include <Python.h>
#include "master.h"
#include <string>


void function_w_mat_arg(const Eigen::Map<Eigen::MatrixXd> &mat){
    cPrint3(std::to_string(mat.size()));
    cPrint3(std::to_string(mat.rows()));
    cPrint3(std::to_string(mat.cols()));
    cPrint3(std::to_string(mat(0,0)));
    cPrint3(std::to_string(mat(0,1)));
    cPrint3(std::to_string(mat(1,0)));
    cPrint3(std::to_string(mat(1,1)));
}