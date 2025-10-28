#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <ctime>
using namespace std;

int largestRectangleArea(vector<int>& heights) {
    int n = heights.size();
    vector<int> left(n), right(n, n);  // left记录每个柱子左侧第一个更矮的位置，right记录右侧第一个更矮的位置
    stack<int> stk;  // 单调栈，存储柱子的索引，保证栈内索引对应的高度单调递增
    
    // 计算left数组
    for (int i = 0; i < n; ++i) {
        while (!stk.empty() && heights[stk.top()] >= heights[i]) {
            stk.pop();
        }
        left[i] = stk.empty() ? -1 : stk.top();
        stk.push(i);
    }
    
    // 清空栈，计算right数组
    while (!stk.empty()) stk.pop();
    for (int i = n - 1; i >= 0; --i) {
        while (!stk.empty() && heights[stk.top()] >= heights[i]) {
            stk.pop();
        }
        right[i] = stk.empty() ? n : stk.top();
        stk.push(i);
    }
    
    // 计算最大面积
    int maxArea = 0;
    for (int i = 0; i < n; ++i) {
        int width = right[i] - left[i] - 1;
        maxArea = max(maxArea, heights[i] * width);
    }
    return maxArea;
}
int main() {
    srand(time(0));  // 初始化随机数种子
    for (int t = 0; t < 10; ++t) {
        int n = rand() % 105 + 1;  // 生成1~105之间的长度
        vector<int> heights(n);
        for (int i = 0; i < n; ++i) {
            heights[i] = rand() % 105;  // 生成0~104之间的高度
        }
        
        // 输出当前测试用例
        cout << "测试用例 " << t + 1 << "：heights = [";
        for (int i = 0; i < n; ++i) {
            cout << heights[i];
            if (i < n - 1) cout << ", ";
        }
        cout << "]" << endl;
        
        // 计算并输出最大面积
        int area = largestRectangleArea(heights);
        cout << "最大面积：" << area << endl << endl;
    }
    return 0;
}
