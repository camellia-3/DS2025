#include <iostream>
#include <string>
#include <cstring>
#include <queue>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;

typedef int Rank;

// 位图类 Bitmap
class Bitmap {
private:
    unsigned char* M;
    Rank N, _sz;

protected:
    void init(Rank n) {
        M = new unsigned char[N = (n + 7) / 8];
        memset(M, 0, N);
        _sz = 0;
    }

public:
    Bitmap(Rank n = 8) { init(n); }
    
    ~Bitmap() { delete[] M; M = NULL; _sz = 0; }
    
    Rank size() { return _sz; }
    
    void set(Rank k) {
        expand(k);
        if (!test(k)) _sz++;
        M[k >> 3] |= (0x80 >> (k & 0x07));
    }
    
    void clear(Rank k) {
        expand(k);
        if (test(k)) _sz--;
        M[k >> 3] &= ~(0x80 >> (k & 0x07));
    }
    
    bool test(Rank k) {
        expand(k);
        return M[k >> 3] & (0x80 >> (k & 0x07));
    }
    
    char* bits2string(Rank n) {
        expand(n - 1);
        char* s = new char[n + 1];
        s[n] = '\0';
        for (Rank i = 0; i < n; i++)
            s[i] = test(i) ? '1' : '0';
        return s;
    }
    
    void expand(Rank k) {
        if (k < 8 * N) return;
        Rank oldN = N;
        unsigned char* oldM = M;
        init(2 * k);
        memcpy(M, oldM, oldN);
        delete[] oldM;
    }
};

// Huffman编码类型
typedef Bitmap HuffCode;

// 二叉树节点类
template<typename T>
struct BinNode {
    T data;
    int weight;
    BinNode<T>* parent;
    BinNode<T>* lc;
    BinNode<T>* rc;
    
    BinNode() : parent(NULL), lc(NULL), rc(NULL), weight(0) {}
    BinNode(T e, int w, BinNode<T>* p = NULL, BinNode<T>* l = NULL, BinNode<T>* r = NULL)
        : data(e), weight(w), parent(p), lc(l), rc(r) {}
    
    bool operator<(const BinNode<T>& other) const {
        return weight > other.weight;
    }
    
    bool isLeaf() { return !lc && !rc; }
};

// 二叉树类
template<typename T>
class BinTree {
protected:
    BinNode<T>* _root;
    int _size;
    
    void clear(BinNode<T>* x) {
        if (!x) return;
        clear(x->lc);
        clear(x->rc);
        delete x;
    }
    
public:
    BinTree() : _root(NULL), _size(0) {}
    ~BinTree() { clear(_root); }
    
    BinNode<T>* root() { return _root; }
    int size() { return _size; }
    
    BinNode<T>* insertAsRoot(T const& e, int w) {
        _size = 1;
        return _root = new BinNode<T>(e, w);
    }
    
    BinNode<T>* insertAsLC(BinNode<T>* x, T const& e, int w) {
        _size++;
        x->lc = new BinNode<T>(e, w, x);
        return x->lc;
    }
    
    BinNode<T>* insertAsRC(BinNode<T>* x, T const& e, int w) {
        _size++;
        x->rc = new BinNode<T>(e, w, x);
        return x->rc;
    }
};

// Huffman树类
class HuffTree {
private:
    BinTree<char>* tree;
    map<char, string> codeTable;
    
    struct NodeCompare {
        bool operator()(BinNode<char>* a, BinNode<char>* b) {
            return a->weight > b->weight;
        }
    };
    
    void generateCodes(BinNode<char>* node, string code) {
        if (!node) return;
        
        if (node->isLeaf()) {
            codeTable[node->data] = code;
            return;
        }
        
        generateCodes(node->lc, code + "0");
        generateCodes(node->rc, code + "1");
    }
    
public:
    HuffTree() : tree(NULL) {}
    ~HuffTree() { delete tree; }
    
    void buildFromText(const string& text) {
        // 统计字符频率（只考虑26个字母，不分大小写）
        map<char, int> freq;
        for (char c : text) {
            if (isalpha(c)) {
                char lowerC = tolower(c);
                freq[lowerC]++;
            }
        }
        
        // 创建优先队列（最小堆）
        priority_queue<BinNode<char>*, vector<BinNode<char>*>, NodeCompare> pq;
        
        // 为每个字符创建叶子节点
        for (auto& pair : freq) {
            BinNode<char>* node = new BinNode<char>(pair.first, pair.second);
            pq.push(node);
        }
        
        // 构建Huffman树
        while (pq.size() > 1) {
            // 取出权重最小的两个节点
            BinNode<char>* left = pq.top(); pq.pop();
            BinNode<char>* right = pq.top(); pq.pop();
            
            // 创建新节点作为父节点
            BinNode<char>* parent = new BinNode<char>('\0', left->weight + right->weight);
            parent->lc = left;
            parent->rc = right;
            left->parent = parent;
            right->parent = parent;
            
            pq.push(parent);
        }
        
        if (pq.empty()) {
            cerr << "Error: No characters found in text!" << endl;
            return;
        }
        
        // 创建二叉树并设置根节点
        tree = new BinTree<char>();
        tree->insertAsRoot('\0', pq.top()->weight);
        tree->root()->lc = pq.top()->lc;
        tree->root()->rc = pq.top()->rc;
        if (pq.top()->lc) pq.top()->lc->parent = tree->root();
        if (pq.top()->rc) pq.top()->rc->parent = tree->root();
        
        // 删除临时节点
        BinNode<char>* temp = pq.top();
        pq.pop();
        delete temp;
        
        // 生成编码表
        generateCodes(tree->root(), "");
    }
    
    string encode(const string& word) {
        string encoded;
        for (char c : word) {
            char lowerC = tolower(c);
            if (codeTable.find(lowerC) != codeTable.end()) {
                encoded += codeTable[lowerC];
            }
        }
        return encoded;
    }
    
    void displayCodeTable() {
        cout << "Huffman Code Table:" << endl;
        for (auto& pair : codeTable) {
            cout << "'" << pair.first << "': " << pair.second << " (frequency: ";
            // 显示频率信息（需要重新统计或存储）
            cout << ")" << endl;
        }
    }
    
    void displayCodeTableWithFreq(const string& text) {
        cout << "Huffman Code Table with Frequencies:" << endl;
        map<char, int> freq;
        for (char c : text) {
            if (isalpha(c)) {
                char lowerC = tolower(c);
                freq[lowerC]++;
            }
        }
        
        for (auto& pair : codeTable) {
            cout << "'" << pair.first << "': " << pair.second;
            if (freq.find(pair.first) != freq.end()) {
                cout << " (frequency: " << freq[pair.first] << ")";
            }
            cout << endl;
        }
    }
    
    HuffCode encodeToBitmap(const string& word) {
        string binaryStr = encode(word);
        HuffCode bitmap(binaryStr.length());
        
        for (size_t i = 0; i < binaryStr.length(); i++) {
            if (binaryStr[i] == '1') {
                bitmap.set(i);
            }
        }
        
        return bitmap;
    }
};

// 读取《I Have a Dream》演讲文本
string readDreamSpeech() {
    string filePath = "C:\\Users\\WT\\OneDrive\\桌面\\I Have A Dream.txt";
    
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Could not open file: " << filePath << endl;
        // 返回默认文本作为后备
        return "i have a dream that one day this nation will rise up and live out the true meaning of its creed we hold these truths to be self evident that all men are created equal";
    }
    
    stringstream buffer;
    buffer << file.rdbuf();
    string content = buffer.str();
    file.close();
    
    // 清理文本，只保留字母和空格
    string cleanedContent;
    for (char c : content) {
        if (isalpha(c) || c == ' ') {
            cleanedContent += tolower(c);
        }
    }
    
    cout << "Successfully read speech file, length: " << cleanedContent.length() << " characters" << endl;
    return cleanedContent;
}

int main() {
    // 读取演讲文本
    string speech = readDreamSpeech();
    cout << "Speech text sample: " << speech.substr(0, 100) << "..." << endl << endl;
    
    // 构建Huffman树
    HuffTree huffTree;
    huffTree.buildFromText(speech);
    
    // 显示编码表
    huffTree.displayCodeTableWithFreq(speech);
    cout << endl;
    
    // 编码测试单词
    vector<string> testWords = {"dream", "freedom", "equality", "hope", "america"};
    
    for (const string& word : testWords) {
        string encoded = huffTree.encode(word);
        HuffCode bitmap = huffTree.encodeToBitmap(word);
        
        cout << "Word: '" << word << "'" << endl;
        cout << "Encoded: " << encoded << endl;
        
        char* bitmapStr = bitmap.bits2string(encoded.length());
        cout << "Bitmap: " << bitmapStr << endl;
        delete[] bitmapStr;
        
        cout << "Length: " << encoded.length() << " bits" << endl;
        cout << endl;
    }
    
    return 0;

}
