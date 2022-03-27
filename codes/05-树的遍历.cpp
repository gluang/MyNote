#include <queue>
#include <stack>
#include <vector>

struct TreeNode {
    int       val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int value)
        : val(value)
        , left(nullptr)
        , right(nullptr) {
    }
};

// 前序遍历：递归
class preOrder1 {
public:
    std::vector<int> res;
    std::vector<int> preorderTraversal(TreeNode *root) {
        if (root == nullptr)
            return res;
        res.push_back(root->val);
        if (root->left != nullptr)
            preorderTraversal(root->left);
        if (root->right != nullptr)
            preorderTraversal(root->right);

        return res;
    }
};
// 前序遍历：迭代
class preOrder2 {
public:
    std::vector<int> preorderTraversal(TreeNode *root) {
        std::vector<int> res;
        if (root == nullptr)
            return res;

        std::stack<TreeNode *> s;
        TreeNode              *cur = root;
        while (!s.empty() || cur) {
            if (cur != nullptr) {
                res.push_back(cur->val);
                if (cur->right != nullptr) {
                    s.push(cur->right);
                }
                cur = cur->left;
            } else {
                // 直到cur为nullptr后，左子树到底了，开始处理栈中数据
                cur = s.top();
                s.pop();
            }
        }

        return res;
    }
};

// 中序遍历：递归
class inOrder1 {
public:
    std::vector<int> res;
    std::vector<int> inorderTraversal(TreeNode *root) {
        if (root == nullptr)
            return res;

        if (root->left != nullptr)
            inorderTraversal(root->left);
        res.push_back(root->val);
        if (root->right != nullptr)
            inorderTraversal(root->right);

        return res;
    }
};
// 中序遍历：迭代
class inOrder2 {
public:
    std::vector<int> inorderTraversal(TreeNode *root) {
        std::vector<int> res;
        if (root == nullptr)
            return res;

        std::stack<TreeNode *> s;
        TreeNode              *cur = root;
        while (!s.empty() || cur) {
            if (cur != nullptr) {
                s.push(cur->left);
                cur = cur->left;
            } else {

                cur = s.top();
                s.pop();
                res.push_back(cur->val);
                cur = cur = cur->right;
            }
        }
        return res;
    }
};

// 后序遍历：递归
class postOrder1 {
public:
    std::vector<int> res;
    std::vector<int> postorderTraversal(TreeNode *root) {
        if (root == nullptr)
            return res;

        if (root->left != nullptr)
            postorderTraversal(root->left);
        if (root->right != nullptr) {
            postorderTraversal(root->right);
        }
        res.push_back(root->val);

        return res;
    }
};
// 后序遍历：迭代1
class postOrder2 {
public:
    std::vector<int> postorderTraversal(TreeNode *root) {
        std::vector<int> res;
        if (root == nullptr)
            return res;

        std::stack<TreeNode *> s;
        s.push(root);

        while (!s.empty()) {
            TreeNode *temp = s.top();
            if (temp->left != nullptr) {
                s.push(temp->left);
                temp->left = nullptr;
            } else if (temp->right != nullptr) {
                s.push(temp->right);
                temp->right = nullptr;
            } else {
                res.push_back(temp->val);
                s.pop();
            }
        }
        return res;
    }
};
// 后序遍历：迭代2
class postOrder3 {
public:
    std::vector<int> postorderTraversal(TreeNode *root) {
        std::vector<int> res;
        if (root == nullptr)
            return res;

        std::stack<TreeNode *> s;
        s.push(root);

        while (!s.empty()) {
            TreeNode *cur = s.top();
            s.pop();
            res.push_back(cur->val);
            if (cur->left != nullptr)
                s.push(cur->left);
            if (cur->right != nullptr)
                s.push(cur->right);
        }
        // 反向输出
        return std::vector<int>(res.rbegin(), res.rend());
    }
};

// 层序遍历：
class levelOrder {
public:
    std::vector<int> levelorderTraversal(TreeNode *root) {
        std::vector<int> res;
        if (root == nullptr)
            return res;

        std::queue<TreeNode *> q;
        q.push(root);

        while (!q.empty()) {
            TreeNode *cur = q.front();
            res.push_back(cur->val);
            q.pop();
            if (cur->left != nullptr)
                q.push(cur->left);
            if (cur->right != nullptr)
                q.push(cur->right);
        }

        return res;
    }
};

int main() {
    
}