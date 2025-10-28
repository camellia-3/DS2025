#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iomanip>

using namespace std;

// 复数类
class Complex {
private:
    double real;  // 实部
    double imag;  // 虚部

public:
    Complex(double r = 0.0, double i = 0.0) : real(r), imag(i) {}
    
    double getReal() const { return real; }
    double getImag() const { return imag; }
    void setReal(double r) { real = r; }
    void setImag(double i) { imag = i; }
    
    // 计算模
    double getModulus() const {
        return sqrt(real * real + imag * imag);
    }
    
    bool operator==(const Complex& other) const {
        return real == other.real && imag == other.imag;
    }
    
    // 按模比较，模相同按实部比较
    bool operator<(const Complex& other) const {
        double mod1 = this->getModulus();
        double mod2 = other.getModulus();
        if (fabs(mod1 - mod2) < 1e-9) {
            return real < other.real;
        }
        return mod1 < mod2;
    }
    
    friend ostream& operator<<(ostream& os, const Complex& c) {
        os << fixed << setprecision(2) << c.real;
        if (c.imag >= 0)
            os << "+" << fixed << setprecision(2) << c.imag << "i";
        else
            os << fixed << setprecision(2) << c.imag << "i";
        return os;
    }
};

// 复数向量类
class ComplexVector {
private:
    vector<Complex> vec;

public:
    void add(const Complex& c) {
        vec.push_back(c);
    }
    
    int size() const {
        return vec.size();
    }
    
    Complex get(int index) const {
        return vec[index];
    }
    
    void set(int index, const Complex& c) {
        vec[index] = c;
    }
    
    // 打印向量
    void print() const {
        cout << "[";
        for (int i = 0; i < vec.size(); i++) {
            cout << vec[i];
            if (i < vec.size() - 1) cout << ", ";
        }
        cout << "]" << endl;
    }
    
    // 打印向量（带模值）
    void printWithModulus() const {
        cout << "[";
        for (int i = 0; i < vec.size(); i++) {
            cout << vec[i] << "(模:" << fixed << setprecision(2) << vec[i].getModulus() << ")";
            if (i < vec.size() - 1) cout << ", ";
        }
        cout << "]" << endl;
    }
    
    // 置乱操作
    void shuffle() {
        for (int i = vec.size() - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            swap(vec[i], vec[j]);
        }
    }
    
    // 查找复数
    int find(const Complex& target) const {
        for (int i = 0; i < vec.size(); i++) {
            if (vec[i] == target) {
                return i;
            }
        }
        return -1;
    }
    
    // 插入复数
    void insert(int index, const Complex& c) {
        if (index >= 0 && index <= vec.size()) {
            vec.insert(vec.begin() + index, c);
        }
    }
    
    // 删除复数
    void remove(int index) {
        if (index >= 0 && index < vec.size()) {
            vec.erase(vec.begin() + index);
        }
    }
    
    // 唯一化操作
    void unique() {
        for (int i = 0; i < vec.size(); i++) {
            for (int j = i + 1; j < vec.size(); ) {
                if (vec[i] == vec[j]) {
                    vec.erase(vec.begin() + j);
                } else {
                    j++;
                }
            }
        }
    }
    
    // 起泡排序
    void bubbleSort() {
        int n = vec.size();
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (!(vec[j] < vec[j + 1])) {
                    swap(vec[j], vec[j + 1]);
                }
            }
        }
    }
    
    // 归并排序辅助函数
    void mergeSortHelper(int left, int right) {
        if (left >= right) return;
        
        int mid = left + (right - left) / 2;
        mergeSortHelper(left, mid);
        mergeSortHelper(mid + 1, right);
        
        vector<Complex> temp;
        int i = left, j = mid + 1;
        while (i <= mid && j <= right) {
            if (vec[i] < vec[j]) {
                temp.push_back(vec[i++]);
            } else {
                temp.push_back(vec[j++]);
            }
        }
        while (i <= mid) temp.push_back(vec[i++]);
        while (j <= right) temp.push_back(vec[j++]);
        
        for (int k = 0; k < temp.size(); k++) {
            vec[left + k] = temp[k];
        }
    }
    
    // 归并排序
    void mergeSort() {
        mergeSortHelper(0, vec.size() - 1);
    }
    
    // 区间查找
    ComplexVector rangeSearch(double m1, double m2) const {
        ComplexVector result;
        for (int i = 0; i < vec.size(); i++) {
            double mod = vec[i].getModulus();
            if (mod >= m1 && mod < m2) {
                result.add(vec[i]);
            }
        }
        return result;
    }
    
    bool isSorted() const {
        for (int i = 0; i < vec.size() - 1; i++) {
            if (!(vec[i] < vec[i + 1]) && !(vec[i] == vec[i + 1])) {
                return false;
            }
        }
        return true;
    }
    
    // 逆序排列
    void reverse() {
        std::reverse(vec.begin(), vec.end());
    }
};

// 生成随机复数向量
ComplexVector generateRandomComplexVector(int size) {
    ComplexVector cv;
    for (int i = 0; i < size; i++) {
        double real = (rand() % 200 - 100) / 10.0;  // -10.0 到 10.0
        double imag = (rand() % 200 - 100) / 10.0;  // -10.0 到 10.0
        cv.add(Complex(real, imag));
    }
    return cv;
}

// 测试排序效率
void testSortEfficiency(ComplexVector& cv, const string& orderType) {
    cout << "\n=== " << orderType << "序列排序效率测试 ===" << endl;
    
    ComplexVector cv1 = cv;  // 用于起泡排序
    ComplexVector cv2 = cv;  // 用于归并排序
    
    // 起泡排序计时
    clock_t start = clock();
    cv1.bubbleSort();
    clock_t end = clock();
    double bubbleTime = double(end - start) / CLOCKS_PER_SEC * 1000;
    cout << "起泡排序时间: " << fixed << setprecision(3) << bubbleTime << " 毫秒" << endl;
    
    // 归并排序计时
    start = clock();
    cv2.mergeSort();
    end = clock();
    double mergeTime = double(end - start) / CLOCKS_PER_SEC * 1000;
    cout << "归并排序时间: " << fixed << setprecision(3) << mergeTime << " 毫秒" << endl;
    
    if (mergeTime > 0) {
        cout << "效率比较: 归并排序比起泡排序快 " << fixed << setprecision(1) 
             << bubbleTime / mergeTime << " 倍" << endl;
    } else {
        cout << "效率比较: 时间太短无法准确比较" << endl;
    }
}

int main() {
    srand(time(NULL));
    
    cout << "=== 复数类测试程序 ===" << endl;
    
    // 生成随机复数向量
    cout << "\n1. 生成随机复数向量:" << endl;
    ComplexVector cv = generateRandomComplexVector(15);
    cout << "原始向量: ";
    cv.print();
    
    // 测试置乱操作
    cout << "\n2. 测试置乱操作:" << endl;
    cv.shuffle();
    cout << "置乱后: ";
    cv.print();
    
    // 测试查找操作
    cout << "\n3. 测试查找操作:" << endl;
    Complex searchTarget(2.5, 3.5);
    int foundIndex = cv.find(searchTarget);
    if (foundIndex != -1) {
        cout << "找到复数 " << searchTarget << " 在位置 " << foundIndex << endl;
    } else {
        cout << "未找到复数 " << searchTarget << endl;
    }
    
    // 测试插入操作
    cout << "\n4. 测试插入操作:" << endl;
    Complex newComplex(7.5, -2.5);
    cv.insert(3, newComplex);
    cout << "在位置3插入 " << newComplex << " 后: ";
    cv.print();
    
    // 测试删除操作
    cout << "\n5. 测试删除操作:" << endl;
    cv.remove(5);
    cout << "删除位置5后: ";
    cv.print();
    
    // 测试唯一化操作
    cout << "\n6. 测试唯一化操作:" << endl;
    // 添加一些重复项
    cv.insert(0, cv.get(0));  // 复制第一个元素
    cv.insert(0, cv.get(1));  // 复制第二个元素
    cout << "添加重复项后: ";
    cv.print();
    cv.unique();
    cout << "唯一化后: ";
    cv.print();
    
    // 排序效率测试
    cout << "\n7. 排序效率比较测试:" << endl;
    
    // 准备测试数据
    ComplexVector testData = generateRandomComplexVector(500);
    
    // 顺序测试
    testData.mergeSort();
    testSortEfficiency(testData, "顺序");
    
    // 乱序测试
    testData.shuffle();
    testSortEfficiency(testData, "乱序");
    
    // 逆序测试
    testData.mergeSort();
    testData.reverse();
    testSortEfficiency(testData, "逆序");
    
    // 区间查找测试
    cout << "\n8. 区间查找测试:" << endl;
    ComplexVector sortedData = generateRandomComplexVector(20);
    sortedData.mergeSort();
    cout << "有序向量: ";
    sortedData.printWithModulus();
    
    double m1 = 3.0, m2 = 7.0;
    cout << "查找模介于 [" << m1 << ", " << m2 << ") 的复数:" << endl;
    ComplexVector result = sortedData.rangeSearch(m1, m2);
    cout << "查找结果: ";
    result.printWithModulus();
    
    cout << "\n测试完成！" << endl;
    return 0;
}
