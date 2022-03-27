# 总结
树是一种重要的非线性数据结构，广泛地应用于计算机技术的各个领域。采用树可以实现一些高效地查找算法，例如数据库系统中用到的红黑树等。

树本身的定义是递归的，因此很多涉及到树的算法通常都可以用递归的方式来实现。然而递归算法在数据量较大的时候效率很低，所以通常会将递归改写成**迭代**算法。

涉及到树的题目主要包括**树的遍历**，**平衡二叉树**，**查找二叉树**等。

LeetCode 上二叉树的节点定义如下：

```cpp
struct TreeNode
{
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) { }
};
```

- 101. 对称二叉树

    递归：对左右子树做深度优先遍历

    迭代：层次遍历，检查每层元素是否镜像对称



# LeetCode

## Construct Binary Tree from Inorder and Postorder Traversal
**描述**：

```
Given inorder and postorder traversal of a tree, construct the binary tree.

Note: You may assume that duplicates do not exist in the tree.
```

**分析**：

二叉树的几种遍历方式：

```
            1
    --------|-------
    2               3
----|----       ----|----
4       5       6       7
```
前序遍历：1245367（root -> left -> right）

中序遍历：4251637（left -> root -> right）

后序遍历：4526731（left -> right -> root）

知道一个二叉树的中序遍历以及后序遍历的结果，如何构建这颗二叉树：
1. 对于**后序遍历**来说，**最后一个元素一定是根节点**，也就是 1
2. 然后在**中序遍历**的结果里面找到 1 所在的位置，那么它的左半部分就是其左子树，有半部分就是其右子树
3. 将中序遍历左半部分 425 取出，同时发现后序遍历的结果也在相应的位置上面，只是顺序稍微不一样，也就是 452。我们可以发现，后序遍历中的 2 就是该子树的根节点
4. 对于右子树，我们取出 637，同时发现后序遍历中对应的数据偏移了一格，并且顺序也不一样，为 673。而 3 就是这颗右子树的根节点

重复上述过程，通过**后序遍历**找到**根节点**，然后在**中序遍历**数据中根据根节点拆分成两个部分，同时将对应的后序遍历的数据也拆分成两个部分，重复递归，就可以得到整个二叉树了

1. 递归：时间复杂度：O(n)，空间复杂度：O(logn)

```cpp
class Solution
{
public:
    TreeNode *buildTree(vector<int> &inorder, vector<int> &postorder)
    {
        return buildTree(begin(inorder), end(inorder),
                        begin(postorder), end(postorder));
    }

    template <typename BidiIt>
    TreeNode *buildTree(BidiIt in_first, BidiIt in_last,
                        BidiIt post_first, BidiIt post_last)
    {
        if (in_first == in_last)
            return nullptr;
        if (post_first == post_last)
            return nullptr;
        const auto val = *prev(post_last);
        TreeNode *root = new TreeNode(val);
        auto in_root_pos = find(in_first, in_last, val);
        auto left_size = distance(in_first, in_root_pos);
        auto post_left_last = next(post_first, left_size);
        root->left = buildTree(in_first, in_root_pos, post_first, post_left_last);
        root->right = buildTree(next(in_root_pos), in_last, post_left_last,
                                prev(post_last));
        return root;
    }
};
```

2. 另一种解法：

```cpp
class Solution
{
public:
    // 使用 hash map 以便保证快速的在中序遍历结果里面找到根节点
    unordered_map<int, int> m;
    TreeNode *buildTree(vector<int> &inorder, vector<int>
                                                &postorder)
    {
        if (postorder.empty())
        {
            return NULL;
        }
        for (int i = 0; i < inorder.size(); i++)
        {
            m[inorder[i]] = i;
        }
        return build(inorder, 0, inorder.size() - 1,
                    postorder, 0, postorder.size() - 1);
    }
    TreeNode *build(vector<int> &inorder, int s0, int e0,
                    vector<int> &postorder, int s1, int e1)
    {
        if (s0 > e0 || s1 > e1)
        {
            return NULL;
        }
        TreeNode *root = new TreeNode(postorder[e1]);
        int mid = m[postorder[e1]];
        int num = mid - s0;
        root->left = build(inorder, s0, mid - 1,
                        postorder, s1, s1 + num - 1);
        root->right = build(inorder, mid + 1, e0,
                            postorder, s1 + num, e1 - 1);
        return root;
    }
};
```


## Validate Binary Search Tree
> LeetCode 98 验证二叉搜索树 Validate Binary Search Tree

**描述**：

给定一个二叉树，判断其是否是一个有效的二叉搜索树。

假设一个二叉搜索树具有如下特征：

节点的左子树只包含小于当前节点的数。节点的右子树只包含大于当前节点的数。所有左子树和右子树自身必须也是二叉搜索树。

**输入输出示例**：

输入:[2,1,3]

输出: true 示例 2:

输入:[5,1,4,null,null,3,6]。

输出: false

解释: 输入为: [5,1,4,null,null,3,6]。根节点的值为 5 ，但是其右子节点值为 4 。

**易错方法**：（简单但错误的方法）对每一个顶点，判断其左孩子是不是比它小，左孩子是否比它大。


```cpp
bool isValidBST(struct TreeNode* root){
    if( root == NULL ) 
        return true;
    if( root->left != NULL && root->left->val > root->val ) 
        return false;
    if( root->right != NULL && root->right->val < root->val ) 
        return false;
    if( !isValidBST(root->left) || !isValidBST(root->right) ) 
        return false;
    
    return true;
}
```
易错点在于：一个节点的左子树上的所有结点，而不仅仅是左孩子。例如：

```
        6
      /   \
     4     8
    / \   / \
   3  7  1  10
```

上图中的根结点是 6 ，而其左子树中的结点 7 大于 6 ，显然不符合二叉排序树的定义；右子树当中的结点 1 小于根结点 6，同样是不合理的；但是你发现上面程序会返回 true，因为上面的程序仅检查了一个结点的左孩子和右孩子，我们自然想到检查一个的结点左子树当中的最大顶点是否比结点小，右子树当中值最小的顶点是否比结点大不就好了，比如根结点 6 的左子树当中最大的顶点为 7 大于 6 ，所以不是一颗二叉排序树，返回false;

**解法 1**：（正确但并不高效）对于每一个结点，检查结点左子树中值最大的结点的值是否小于结点，右子树中值最小的结点是否大于结点。

```cpp
int minValueNode(struct TreeNode* node)
{
    struct TreeNode* current = node;

    /*从删除结点的右孩子开始一直向左走，找到最小值*/
    while (current && current->left != NULL)
        current = current->left;

    return current->val;
}

/* 返回删除结点左子树当中的值最大的结点指针 */
int maxValueNode(struct TreeNode* node)
{
    struct TreeNode* current = node;

    /*从删除结点的右孩子开始一直向左走，找到最小值*/
    while (current && current->right != NULL)
        current = current->right;

    return current->val;
}

bool isValidBST(struct TreeNode* root){
    if(root == NULL) return true;
    if(root->left != NULL && maxValueNode(root->left) >= root->val) return false;
    if(root->right != NULL && minValueNode(root->right) <= root->val) return false;

    if(!isValidBST(root->left) || !isValidBST(root->right)) return false;

    return true;
}
```

方法 2（正确且高效）：

方法 1 低效在哪儿呢？对于每一个结点都查找其左子树中值最大的结点，查找其右子树中值最小的结点，这样不可避免地会重复查找一棵树的子树，效率自然低下。更好的解决方案就是只对树中的每个结点检查一次。诀窍就是写一个辅助函数 `isBSTUtil(struct TreeNode* root, long long min ,long long max)` 来遍历树，不断更新每一个遍历结点的最大与最小值的取值，初始时将最小值设置为 `LONG_MIN` ，将最大值设置为 `LONG_MAX` ；当递归调用左子树是，最大值则被更新为结点的值为 `max = root->val` ( **左子树结点的最大值一定小于root->val** )，即调用 `isBSTUtil(root->left，min, root-val)`  ;当递归调用右子树时，右子树中所有结点的下界也比 `root` 的值大，故更新 `min = root-val` （**右子树结点的最小值一定大于root->val**），即调用 `isBSTUtil(root->right, root->val, max)`。

```cpp
int isBSTUtil(struct TreeNode* node, long long min, long long max)  
{  
    /* 是一颗空树 */
    if (node==NULL)  
        return 1; 
        
    /* 结点的值小于等于最小值，或者大于等于最大值都是不合理的，返回false */  
    if (node->val <= min || node->val >= max)  
        return 0;  
  
    /*否则递归地判断结点的左子树和右子树*/
    return 
        isBSTUtil(node->left, min, node->val) && 
        isBSTUtil(node->right, node->val, max); 
}  

bool isValidBST(struct TreeNode* root){
    return isBSTUtil(root,LONG_MIN, LONG_MAX);
}
```

方法 3：（简化方法 2）：我们可以使用空指针代替 LONG_MIN 和 LONG_MAX 的值。(道理和方法 2 一样，一个结点的左子树中结点的最大值不会超过结点的值，一个结点的右子树中结点的最小值也会大于结点的值)

```cpp
class Solution {
public:
    bool isValidBST(TreeNode* root,TreeNode* l=NULL, TreeNode* r=NULL) {
        if(root == NULL) return true;
        if(l != NULL && root->val <= l->val) return false;
        if(r != NULL && root->val >= r->val) return false;
        return isValidBST(root->left, l, root) and
            isValidBST(root->right, root, r);
    }
};
```

方法 4：（巧妙简单高效）前面我们讨论过二叉排序树的中序遍历结果是一个有序数组，且有序数组的前一个元素一定小于后一个元素。

在进行中序遍历时，我们可以通过使用在递归调用函数中传入一个用于保存当前访问结点前一个结点值的整型变量 pre ，如果当前访问结点的值小于或者等于他的前驱结点的值，则该树不是二叉排序树。

时间复杂度为 O(n) ，空间复杂度 O(1)
```cpp
class Solution {
bool isBSTUtil(struct TreeNode* root, long long& prev) 
{ 
    if (root) { 
        if (!isBSTUtil(root->left, prev)) 
            return false; 
  
        // 当前结点小于等于它的直接前驱顶点，返回false 
        if (root->val <= prev) 
            return false; 
  
        //初始化pre 为当前结点
        prev = root->val; 
  
        return isBSTUtil(root->right, prev); 
    } 
  
    return true; 
} 
public:
    bool isValidBST(TreeNode* root) {
        long long prev = LONG_MIN; 
        return isBSTUtil(root, prev); 
    }
};
```

## 平衡二叉树
> 110. 平衡二叉树 Balanced Binary Tree

**题目描述**：

```
给定一个二叉树，判断它是否是高度平衡的二叉树。
本题中，一棵高度平衡二叉树定义为：
一个二叉树每个节点 的左右两个子树的高度差的绝对值不超过1。
```

**输入输出示例**：

```
示例一：
    给定二叉树 [3,9,20,null,null,15,7]
    返回 true
示例二：
    给定二叉树 [1,2,2,3,3,null,null,4,4]
    返回 false
```

**题目解析**：

考虑一颗二叉树是否高度平衡，我们需要检查下面的这些条件：

一颗空树必然是高度平衡的。一颗非空的树 T 是高度平衡的，当且仅当满足下面 3 个条件（递归定义）：
1. 树 T 的左子树是平衡的
2. 树 T 的右子树是平衡的
3. 左右子树的高度之差不超过 1

**备注**：此处的二叉树并没注明是二叉搜索树，而平衡二叉树是在二叉搜索树的基础上的，这点需要明白

**方法 1**：检查一颗二叉树是不是高度平衡，则对二叉树的结点检查其左右子树的高度之差是否超过 1，超过 1 则返回 false，否则返回 true

```cpp
int abs(int x)
{
    if (x < 0)
    {
        return -x;
    }
    return x;
}
int max(int x, int y)
{
    return (x >= y) ? x : y;
}
//计算node的高度
int height(struct TreeNode *node)
{
    if (node == NULL)
    {
        return 0;
    }
    return 1 + max(height(node->left), height(node->right));
}
//判断二叉树是否平衡
bool isBalanced(struct TreeNode *root)
{
    int lh; //左子树高度
    int rh; //右子树高度

    //树为空返回true;
    if (root == NULL)
        return 1;
    //获得左子树深度
    lh = height(root->left);
    //获得右子树深度
    rh = height(root->right);
    //判断左右子树高度之差是否小于1，并且结点的左右子树平衡，返回true;
    if (abs(lh - rh) <= 1 && isBalanced(root->left) && isBalanced(root->right))
    {
        return 1;
    }
    return 0;
}
```

方法 2：（对方法 1 优化）但是上面的方法存在性能上的问题，当输入是一颗斜树的时候，其时间复杂度将变成 O(n<sup>2</sup>) 。问题在于我们判断二叉树是否平衡的函数 `isBalanced()` 当中嵌套了一个计算树的高度的函数 `height()`，这样以来，当树为一颗斜树的时候，时间复杂度就会达到 O(n<sup>2</sup>)  。解决的办法就是将这两个函数合并，取消单独调用的 `height()` 函数，而是在递归进行判断的时候计算树的高度：

```cpp
int abs(int x)
{
    if (x < 0)
    {
        return -x;
    }
    return x;
}

bool isBalancedUtil(struct TreeNode *root, int *height)
{
    int lh; //保存左子树的高度
    int rh; //右子树的高度

    int l = 0; //左子树是否平衡标志
    int r = 0; //右子树是否平衡标志

    if (root == NULL)
    {
        *height = 0;
        return 1;
    }
    //递归判断左右子树是否平衡
    l = isBalancedUtil(root->left, &lh);
    r = isBalancedUtil(root->right, &rh);

    //计算树的高度，左右子树高度较大者加1
    *height = ((lh >= rh) ? lh : rh) + 1;

    //如果左右子树高度之差大于等于2，返回false;
    if (abs(lh - rh) >= 2)
    {
        return 0;
    }
    //否则返回左右子树平衡标志的与
    return l && r;
}
bool isBalanced(struct TreeNode *root)
{
    int height = 0;
    return isBalancedUtil(root, &height);
}
```


## 5.1.1 Binary Tree Preorder Traversal
**描述**：

```
Given a binary tree, return the preorder traversal of its nodes’ values.
For example: Given binary tree {1,#,2,3},
    1
    \
     2
    /
   3
return [1,2,3].
Note: Recursive solution is trivial, could you do it iteratively?
```


```
给定一个二叉树，返回其节点值的前序遍历。
注意：递归解决方案是琐碎的，你能迭代吗?
```

**分析**：

用栈或者 Morris 遍历


解法 1：使用栈，时间复杂度：O(n)，空间复杂度：O(n)
```cpp
class Solution
{
public:
    vector<int> preorderTraversal(TreeNode *root)
    {
        vector<int> result;
        stack<const TreeNode *> s;
        if (root != nullptr)
            s.push(root);
        while (!s.empty())
        {
            const TreeNode *p = s.top();
            s.pop();
            result.push_back(p->val);
            if (p->right != nullptr)
                s.push(p->right);
            if (p->left != nullptr)
                s.push(p->left);
        }
        return result;
    }
};
```

解法 2：Morris 先序遍历，时间复杂度：O(n)，空间复杂度：O(1)

```cpp
class Solution
{
public:
    vector<int> preorderTraversal(TreeNode *root)
    {
        vector<int> result;
        TreeNode *cur = root, *prev = nullptr;
        while (cur != nullptr)
        {
            if (cur->left == nullptr)
            {
                result.push_back(cur->val);
                prev = cur; /* cur 刚刚被访问过 */
                cur = cur->right;
            }
            else
            {
                /* 查找前驱 */
                TreeNode *node = cur->left;
                while (node->right != nullptr && node->right != cur)
                    node = node->right;
                if (node->right == nullptr)
                {                               /* 还没线索化，则建立线索 */
                    result.push_back(cur->val); /* 仅这一行的位置与中序不同 */
                    node->right = cur;
                    prev = cur; /* cur 刚刚被访问过 */
                    cur = cur->left;
                }
                else
                {   /* 已经线索化，则删除线索 */
                    node->right = nullptr;
                    /* prev = cur; 不能有这句，cur 已经被访问 */
                    cur = cur->right;
                }
            }
        }
        return result;
    }
};
```

## 5.1.2 Binary Tree Inorder Traversal
**描述**：

```
Given a binary tree, return the inorder traversal of its nodes’ values.
For example: Given binary tree {1,#,2,3},
    1
    \
     2
    /
   3
return [1,3,2].
Note: Recursive solution is trivial, could you do it iteratively?
```


```
给定一个二叉树，返回其节点值的中序遍历。
注意：递归解决方案是琐碎的，你能迭代吗?
```

**分析**：

用栈或者 Morris 遍历


解法 1：使用栈，时间复杂度：O(n)，空间复杂度：O(n)
```cpp
class Solution
{
public:
    vector<int> inorderTraversal(TreeNode *root)
    {
        vector<int> result;
        stack<const TreeNode *> s;
        const TreeNode *p = root;
        while (!s.empty() || p != nullptr)
        {
            if (p != nullptr)
            {
                s.push(p);
                p = p->left;
            }
            else
            {
                p = s.top();
                s.pop();
                result.push_back(p->val);
                p = p->right;
            }
        }
        return result;
    }
};
```

解法 2：Morris 中序遍历，时间复杂度：O(n)，空间复杂度：O(1)

```cpp
class Solution
{
public:
    vector<int> inorderTraversal(TreeNode *root)
    {
        vector<int> result;
        TreeNode *cur = root, *prev = nullptr;
        while (cur != nullptr)
        {
            if (cur->left == nullptr)
            {
                result.push_back(cur->val);
                prev = cur;
                cur = cur->right;
            }
            else
            {
                /* 查找前驱 */
                TreeNode *node = cur->left;
                while (node->right != nullptr && node->right != cur)
                    node = node->right;
                if (node->right == nullptr)
                { /* 还没线索化，则建立线索 */
                    node->right = cur;
                    /* prev = cur; 不能有这句，cur 还没有被访问 */
                    cur = cur->left;
                }
                else
                { /* 已经线索化，则访问节点，并删除线索 */
                    result.push_back(cur->val);
                    node->right = nullptr;
                    prev = cur;
                    cur = cur->right;
                }
            }
        }
        return result;
    }
};
```

**相关题目**：
- Binary Tree Preorder Traversal，见 §5.1.1
- Binary Tree Postorder Traversal，见 §5.1.3
- Recover Binary Search Tree，见 §5.1.7

## 5.1.3 Binary Tree Postorder Traversal

**描述**：

```
Given a binary tree, return the postorder traversal of its nodes’ values.
For example: Given binary tree {1,#,2,3},
    1
    \
     2
    /
   3
return  [3,2,1].
Note: Recursive solution is trivial, could you do it iteratively?
```


```
给定一个二叉树，返回其节点值的后序遍历。
注意：递归解决方案是琐碎的，你能迭代吗?
```

**分析**：

用栈或者 Morris 遍历


解法 1：使用栈，时间复杂度：O(n)，空间复杂度：O(n)

```cpp
class Solution
{
public:
    vector<int> postorderTraversal(TreeNode *root)
    {
        vector<int> result;
        stack<const TreeNode *> s;
        /* p，正在访问的结点，q，刚刚访问过的结点 */
        const TreeNode *p = root, *q = nullptr;
        do
        {
            while (p != nullptr)
            { /* 往左下走 */
                s.push(p);
                p = p->left;
            }
            q = nullptr;
            while (!s.empty())
            {
                p = s.top();
                s.pop();
                /* 右孩子不存在或已被访问，访问之 */
                if (p->right == q)
                {
                    result.push_back(p->val);
                    q = p; /* 保存刚访问过的结点 */
                }
                else
                {
                    /* 当前结点不能访问，需第二次进栈 */
                    s.push(p);
                    /* 先处理右子树 */
                    p = p->right;
                    break;
                }
            }
        } while (!s.empty());
        return result;
    }
};
```

解法 2：Morris 后序遍历，时间复杂度：O(n)，空间复杂度：O(1)

```cpp
class Solution
{
public:
    vector<int> postorderTraversal(TreeNode *root)
    {
        vector<int> result;
        TreeNode dummy(-1);
        TreeNode *cur, *prev = nullptr;
        std::function<void(const TreeNode *)> visit =
            [&result](const TreeNode *node) {
                result.push_back(node->val);
            };
        dummy.left = root;
        cur = &dummy;
        while (cur != nullptr)
        {
            if (cur->left == nullptr)
            {
                prev = cur; /* 必须要有 */
                cur = cur->right;
            }
            else
            {
                TreeNode *node = cur->left;
                while (node->right != nullptr && node->right != cur)
                    node = node->right;
                if (node->right == nullptr)
                { /* 还没线索化，则建立线索 */
                    node->right = cur;
                    prev = cur; /* 必须要有 */
                    cur = cur->left;
                }
                else
                { /* 已经线索化，则访问结点，并删除线索 */
                    visit_reverse(cur->left, prev, visit);
                    prev->right = nullptr;
                    prev = cur; /* 必须要有 */
                    cur = cur->right;
                }
            }
        }
        return result;
    }

private:
    // 逆转路径
    static void reverse(TreeNode *from, TreeNode *to)
    {
        TreeNode *x = from, *y = from->right, *z;
        if (from == to)
            return;
        while (x != to)
        {
            z = y->right;
            y->right = x;
            x = y;
            y = z;
        }
    }
    // 访问逆转后的路径上的所有结点
    static void visit_reverse(TreeNode *from, TreeNode *to,
                              std::function<void(const TreeNode *)> &visit)
    {
        TreeNode *p = to;
        reverse(from, to);
        while (true)
        {
            visit(p);
            if (p == from)
                break;
            p = p->right;
        }
        reverse(to, from);
    }
};
```


**相关题目**：
- Binary Tree Preorder Traversal，见 §5.1.1
- Binary Tree Inorder Traversal，见 §5.1.2
- Recover Binary Search Tree，见 §5.1.7

