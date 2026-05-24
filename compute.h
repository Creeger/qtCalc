#ifndef COMPUTE_H
#define COMPUTE_H

#include <string>
#include <vector>

struct EvalResult {
    bool valid;
    std::string error;
    double result;
};

enum AngleMode {
    RAD,
    DEG
};


EvalResult computeRPN(std::vector<std::string> rpnExpression, AngleMode mode);
#endif // COMPUTE_H
