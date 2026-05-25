#include <vector>
#include <unordered_map>
#include <string>
#include <stack>
#include <functional>
#include <cmath>
#include <algorithm>
#include "compute.h"
#include <iostream>

std::unordered_map<std::string, double> mathConsts = {
    {"pi", M_PI},
    {"eps", std::numeric_limits<double>::epsilon()},
    {"tau", M_PI*2},
};


EvalResult computeRPN(std::vector<std::string> rpnExpression, AngleMode mode) {
    std::stack<double> stack;
    std::unordered_map<std::string, std::function<double(double, double)>> binOps = {
        {"+", [](double a, double b) {return a + b;}},
        {"-", [](double a, double b) {return a - b;}},
        {"*", [](double a, double b) {return a * b;}},
        {"/", [](double a, double b) {return a / b;}},
        {"^", [](double a, double b) {return std::pow(a, b);}},
        {"max", [](double a, double b) {return std::max(a, b);}},
    };

    std::unordered_map<std::string, std::function<double(double)>> unaryOps = {
        {"sin", [](double a) {return std::sin(a);}},
        {"cos", [](double a) {return std::cos(a);}},
        {"tan", [](double a) {return std::tan(a);}},
        {"sqrt", [](double a) {return std::sqrt(a);}},
    };

    if (rpnExpression.empty()) {
        return {false, "Empty expression", 0.0};
    }

    for (const std::string& token : rpnExpression) {
        std::cout << "Curr token: " << "[" << token << "]" << "\n";
        try {
            size_t idx;
            double num = std::stod(token, &idx);

            if (idx == token.size()) {
                stack.push(num);
                continue;
            }
        } catch (...) {
        }

        if (mathConsts.count(token)) {
            stack.push(mathConsts[token]);
            continue;
        }

        //Check if token is a binary operator
        if (binOps.count(token)) {
            if (stack.size() < 2) {
                return {false, "Invalid binary expression", 0.0};
            }

            double b = stack.top(); stack.pop();
            double a = stack.top(); stack.pop();
            stack.push(binOps[token](a, b));

        // Check if token is unaru operator
        } else if (unaryOps.count(token)) {
            if (stack.empty()) {
                return {false, "Invalid unary expression", 0.0};
            }
            double a = stack.top(); stack.pop();

            if (mode == DEG) {
                a = a * M_PI / 180.0;
            }

            stack.push(unaryOps[token](a));
        } else {
            return {false, "Unknown operator " + token, 0.0};
        }
    }
    if (stack.size() != 1) {
        return {false, "Invalid expression", 0.0};
    }
    return {true, "", stack.top()};     
}


