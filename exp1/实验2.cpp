#include <iostream>
#include <stack>
#include <string>
#include <cmath>
#include <unordered_map>
#include <cctype>
#include <stdexcept>

using namespace std;

// 栈模板类实现
template <typename T>
class Stack {
private:
    stack<T> stk;
public:
    void push(T elem) { stk.push(elem); }
    T pop() { 
        if (isEmpty()) throw runtime_error("栈为空，无法弹出元素");
        T top = stk.top();
        stk.pop();
        return top;
    }
    T top() { 
        if (isEmpty()) throw runtime_error("栈为空，无法获取栈顶元素");
        return stk.top();
    }
    bool isEmpty() { return stk.empty(); }
    int size() { return stk.size(); }
};

// 运算符优先级映射表
unordered_map<char, int> opPriority = {
    {'+', 1}, {'-', 1},
    {'*', 2}, {'/', 2},
    {'^', 3},  // 幂运算
    {'s', 4}, {'c', 4}, {'t', 4}, {'l', 4}  // sin, cos, tan, log
};

// 检查字符是否为运算符
bool isOperator(char c) {
    return opPriority.count(c) > 0;
}

// 执行双目运算
double calculate(double num1, double num2, char op) {
    switch (op) {
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        case '*': return num1 * num2;
        case '/': 
            if (num2 == 0) throw runtime_error("除零错误");
            return num1 / num2;
        case '^': return pow(num1, num2);
        default: throw runtime_error("未知运算符");
    }
}

// 执行单目运算（三角函数等）
double calculateUnary(double num, char op) {
    switch (op) {
        case 's': return sin(num);  // 弧度制
        case 'c': return cos(num);
        case 't': return tan(num);
        case 'l': 
            if (num <= 0) throw runtime_error("对数定义域错误（必须大于0）");
            return log(num);  // 自然对数
        default: throw runtime_error("未知单目运算符");
    }
}

// 表达式计算主函数
double evaluateExpression(const string& expr) {
    Stack<double> numStack;  // 数字栈
    Stack<char> opStack;     // 运算符栈
    int n = expr.size();
    int i = 0;

    while (i < n) {
        // 跳过空格
        if (isspace(expr[i])) {
            i++;
            continue;
        }

        // 处理数字（包括小数、自然常数e）
        if (isdigit(expr[i]) || expr[i] == '.' || 
            ((expr[i] == '-' || expr[i] == '+') && 
             (i + 1 < n && (isdigit(expr[i+1]) || expr[i+1] == '.' || expr[i+1] == 'e' || expr[i+1] == 'E')))) {
            
            double num = 0;
            bool isNegative = false;
            
            // 处理符号
            if (expr[i] == '-') {
                isNegative = true;
                i++;
            } else if (expr[i] == '+') {
                i++;
            }
            
            // 处理自然常数e
            if (i < n && (expr[i] == 'e' || expr[i] == 'E')) {
                num = M_E;
                i++;
            } else {
                // 整数部分
                while (i < n && isdigit(expr[i])) {
                    num = num * 10 + (expr[i] - '0');
                    i++;
                }
                // 小数部分
                if (i < n && expr[i] == '.') {
                    i++;
                    double fraction = 0.1;
                    while (i < n && isdigit(expr[i])) {
                        num += (expr[i] - '0') * fraction;
                        fraction *= 0.1;
                        i++;
                    }
                }
                // 指数部分（科学计数法）
                if (i < n && (expr[i] == 'e' || expr[i] == 'E')) {
                    i++;
                    bool expNegative = false;
                    if (expr[i] == '-') {
                        expNegative = true;
                        i++;
                    } else if (expr[i] == '+') {
                        i++;
                    }
                    double exp = 0;
                    while (i < n && isdigit(expr[i])) {
                        exp = exp * 10 + (expr[i] - '0');
                        i++;
                    }
                    if (expNegative) exp = -exp;
                    num *= pow(10, exp);
                }
            }
            
            if (isNegative) num = -num;
            numStack.push(num);
        }
        // 处理自然常数e（单独出现的情况）
        else if ((expr[i] == 'e' || expr[i] == 'E') && 
                 (i == 0 || !isdigit(expr[i-1]) && expr[i-1] != '.')) {
            numStack.push(M_E);
            i++;
        }
        // 处理左括号
        else if (expr[i] == '(') {
            opStack.push(expr[i]);
            i++;
        }
        // 处理右括号
        else if (expr[i] == ')') {
            while (!opStack.isEmpty() && opStack.top() != '(') {
                char op = opStack.pop();
                // 处理单目运算符
                if (op == 's' || op == 'c' || op == 't' || op == 'l') {
                    if (numStack.isEmpty()) throw runtime_error("表达式无效（单目运算符缺少操作数）");
                    double num = numStack.pop();
                    numStack.push(calculateUnary(num, op));
                }
                // 处理双目运算符
                else {
                    if (numStack.size() < 2) throw runtime_error("表达式无效（双目运算符缺少操作数）");
                    double num2 = numStack.pop();
                    double num1 = numStack.pop();
                    numStack.push(calculate(num1, num2, op));
                }
            }
            if (opStack.isEmpty()) throw runtime_error("括号不匹配（缺少左括号）");
            opStack.pop();  // 弹出左括号
            i++;
        }
        // 处理运算符（包括三角函数和对数）
        else if ((i + 2 < n && expr.substr(i, 3) == "sin") ||
                 (i + 2 < n && expr.substr(i, 3) == "cos") ||
                 (i + 2 < n && expr.substr(i, 3) == "tan") ||
                 (i + 2 < n && expr.substr(i, 3) == "log") ||
                 isOperator(expr[i])) {
            
            // 处理三角函数和对数
            if (i + 2 < n && expr.substr(i, 3) == "sin") {
                opStack.push('s');
                i += 3;
            } else if (i + 2 < n && expr.substr(i, 3) == "cos") {
                opStack.push('c');
                i += 3;
            } else if (i + 2 < n && expr.substr(i, 3) == "tan") {
                opStack.push('t');
                i += 3;
            } else if (i + 2 < n && expr.substr(i, 3) == "log") {
                opStack.push('l');
                i += 3;
            } else {  // 处理双目运算符
                char op = expr[i];
                // 处理负号（作为单目运算符）
                if (op == '-' && (i == 0 || expr[i-1] == '(' || isOperator(expr[i-1]))) {
                    // 检查下一个字符是否是数字或e
                    if (i + 1 < n && (isdigit(expr[i+1]) || expr[i+1] == '.' || expr[i+1] == 'e' || expr[i+1] == 'E')) {
                        // 让下一个循环处理这个带负号的数字
                        i++;
                        continue;
                    } else {
                        // 其他情况，如 -sin(x) 等，我们暂时不支持
                        throw runtime_error("不支持的单目负号用法");
                    }
                }
                // 弹出优先级更高或相等的运算符先计算
                while (!opStack.isEmpty() && opStack.top() != '(' && 
                       opPriority[opStack.top()] >= opPriority[op]) {
                    char topOp = opStack.pop();
                    if (topOp == 's' || topOp == 'c' || topOp == 't' || topOp == 'l') {
                        if (numStack.isEmpty()) throw runtime_error("表达式无效（单目运算符缺少操作数）");
                        double num = numStack.pop();
                        numStack.push(calculateUnary(num, topOp));
                    } else {
                        if (numStack.size() < 2) throw runtime_error("表达式无效（双目运算符缺少操作数）");
                        double num2 = numStack.pop();
                        double num1 = numStack.pop();
                        numStack.push(calculate(num1, num2, topOp));
                    }
                }
                opStack.push(op);
                i++;
            }
        }
        else {
            throw runtime_error("无效字符: " + string(1, expr[i]));
        }
    }

    // 处理剩余运算符
    while (!opStack.isEmpty()) {
        char op = opStack.pop();
        if (op == '(') throw runtime_error("括号不匹配（缺少右括号）");
        
        if (op == 's' || op == 'c' || op == 't' || op == 'l') {
            if (numStack.isEmpty()) throw runtime_error("表达式无效（单目运算符缺少操作数）");
            double num = numStack.pop();
            numStack.push(calculateUnary(num, op));
        } else {
            if (numStack.size() < 2) throw runtime_error("表达式无效（双目运算符缺少操作数）");
            double num2 = numStack.pop();
            double num1 = numStack.pop();
            numStack.push(calculate(num1, num2, op));
        }
    }

    if (numStack.size() != 1) throw runtime_error("表达式无效（操作数数量错误）");
    return numStack.pop();
}

int main() {
    // 测试案例
    string testCases[] = {
        "3 + 4 * 2 / (1 - 5)",          // 基础运算: 1
        "2 ^ 3 + 5",                    // 幂运算: 13
        "sin(0)",                       // 正弦函数: 0
        "cos(0) + tan(0)",              // 余弦+正切: 1
        "log(1) + 5",                   // 对数运算: 5
        "((2 + 3) * 4) / 5",            // 括号运算: 4
        "3.5 + 2.5 * 2",                // 小数运算: 8.5
        "sin(3.1415926 / 2) + log(e)"   // 复合运算: 约2.0
    };

    for (const string& expr : testCases) {
        cout << "表达式: " << expr << endl;
        try {
            double result = evaluateExpression(expr);
            cout << "结果: " << result << endl << endl;
        } catch (const exception& e) {
            cout << "错误: " << e.what() << endl << endl;
        }
    }

    // 交互式计算
    string expr;
    cout << "请输入表达式(输入q退出): ";
    while (getline(cin, expr) && expr != "q") {
        try {
            double result = evaluateExpression(expr);
            cout << "结果: " << result << endl;
        } catch (const exception& e) {
            cout << "错误: " << e.what() << endl;
        }
        cout << "请输入表达式(输入q退出): ";
    }

    return 0;
}