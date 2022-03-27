# 知识点
二叉树的数据结构：

```cpp
struct TreeNode {
	int val;
	TreeNode *left;
	TreeNode *right;
	TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};
```

二叉树的题目普遍可以用**递归**和**迭代**的方式来解。


# LeetCode
- 二叉树的遍历

- 二叉树的构造

- 一般题目：

题号|题目|难度|完成度|备注
--|--|--|--|--
104|二叉树的最大深度|🟢|✔|迭代（对应 5.4.2）
111|二叉树的最小深度|🟢|✔|（对应 5.4.1）
&nbsp;|二叉树的节点个数||✔|
&nbsp;|二叉树中叶子节点的个数||✔|
110|判断二叉树是否是平衡二叉树|🟢|✔|在 “树类型实现” 一节
958|判断二叉树是否是完全二叉树|🟡|✔|在 “树类型实现” 一节
100|相同的树|🟢|✔|（对应 5.1.8）
101|对称二叉树|🟢|✔|在 “树类型实现” 一节
226|翻转二叉树|🟢|✔|
&nbsp;|两个二叉树的最低公共祖先节点||✔|
&nbsp;|二叉树中插入节点||✔|
112|路径总和|🟢|✔|（对应 5.4.3）
113|路径总和 II|🟡|✔|递归（对应 5.4.4）
108|将有序数组转换为二叉搜索树|🟢|✔|分治法（对应 5.3.5）
109|有序链表转换二叉搜索树|🟡|✔|（对应 5.3.4）

- **二叉搜索树**：

题号|题目|难度|完成度|备注
--|--|--|--|--
98|验证二叉搜索树|🟡|✔|判断二叉树是否存在（对应 5.3.3）
96|不同的二叉搜索树|🟡|✔|动态规划（对应 5.3.1）




🟢 🔴 🟡 ✔ ✖




## 二叉树的最大深度
**描述**：

```
给定一个二叉树，找出其最大深度。

二叉树的深度为根节点到最远叶子节点的最长路径上的节点数。

说明: 叶子节点是指没有子节点的节点。

示例：
给定二叉树 [3,9,20,null,null,15,7]，

    3
   / \
  9  20
    /  \
   15   7
返回它的最大深度 3 。
```

**思路**：

时间复杂度：`O(n)`，空间复杂度：`O(logn)`
```cpp
class Solution {
public:
    int maxDepth(TreeNode* root) {
        if (root == NULL)
        return 0;

		int left = maxDepth(root->left);
		int right = maxDepth(root->right);
		return max(left, right) + 1;
    }
};
```

## 二叉树的最小深度
**描述**：

```
给定一个二叉树，找出其最小深度。

最小深度是从根节点到最近叶子节点的最短路径上的节点数量。

说明: 叶子节点是指没有子节点的节点。

示例:

给定二叉树 [3,9,20,null,null,15,7],

    3
   / \
  9  20
    /  \
   15   7
返回它的最小深度  2.
```

**思路**：

递归版：

时间复杂度：`O(n)`，空间复杂度：`O(logn)`
```cpp
class Solution {
public:
    int minDepth(TreeNode* root) {
        if (root == NULL)
            return 0;
        if (root->left != NULL && root->right != NULL)
            return 1 + std::min(minDepth(root->left), minDepth(root->right));
        else
            return 1 + minDepth(root->right) + minDepth(root->left);
    }
};
```

迭代版：

时间复杂度：`O(n)`，空间复杂度：`O(logn)`

```cpp
class Solution {
public:
    int minDepth(TreeNode *root) {
        if (root == NULL)
            return 0;
        int result = INT_MAX;
        stack<pair<TreeNode *, int>> s;
        s.push(make_pair(root, 1));
        
        while (!s.empty()) {
            auto node = s.top().first;
            auto depth = s.top().second;
            s.pop();
            if (node->left == NULL && node->right == NULL)
                result = min(result, depth);
            
            // 深度控制，剪枝
            if (node->left && result > depth)
                s.push(make_pair(node->left, depth + 1));
            // 深度控制，剪枝
            if (node->right && result > depth) 
                s.push(make_pair(node->right, depth + 1));
        }
        return result;
    }
};
```


## 二叉树中节点的个数

```cpp
int numOfTreeNode(TreeNode *root)
{
	if (root == NULL)
		return 0;

	int left = numOfTreeNode(root->left);
	int right = numOfTreeNode(root->right);
	return left + right + 1;
}
```

## 二叉树中叶子节点的个数

```cpp
int numsOfNodeTreeNode(TreeNode *root)
{
	if (root == NULL)
		return 0;
	if (root->left == NULL && root->right == NULL)
		return 1;

	return numsOfNodeTreeNode(root->left) + numsOfNodeTreeNode(root->right);
}
```

## 二叉树中第k层节点的个数

```cpp
int numsOfkLevelTreeNode(TreeNode *root, int k)
{
	if (root == NULL || k < 1)
		return 0;
	if (k == 1)
		return 1;

	int numsLeft = numsOfkLevelTreeNode(root->left, k - 1);
	int numsRight = numsOfkLevelTreeNode(root->right, k - 1);
	return numsLeft + numsRight;
}
```

## 判断二叉树是否是平衡二叉树


## 判断二叉树是否是完全二叉树



## 相同的树
**描述**：

```
给定两个二叉树，编写一个函数来检验它们是否相同。

如果两个树在结构上相同，并且节点具有相同的值，则认为它们是相同的。

示例 1:
输入:       1         1
          / \       / \
         2   3     2   3

        [1,2,3],   [1,2,3]
输出: true

示例 2:
输入:      1          1
          /           \
         2             2

        [1,2],     [1,null,2]
输出: false

示例 3:
输入:       1         1
          / \       / \
         2   1     1   2

        [1,2,1],   [1,1,2]
输出: false
```

递归版：

时间复杂度：`O(n)`，空间复杂度：`O(logn)`
```cpp
class Solution {
public:
    bool isSameTree(TreeNode* p, TreeNode* q) {
        if (p == NULL && q == NULL)
			return true;
		else if (p == NULL || q == NULL)
			return false;

		if (p->val != q->val)
			return false;

		bool left = isSameTree(p->left, q->left);
		bool right = isSameTree(p->right, q->right);
		return left && right;
    }
};
```

迭代版：

时间复杂度：`O(n)`，空间复杂度：`O(logn)`

```cpp
class Solution {
public:
    bool isSameTree(TreeNode *p, TreeNode *q) {
        stack<TreeNode *> s;
        s.push(p);
        s.push(q);
        while (!s.empty()) {
            p = s.top();
            s.pop();
            q = s.top();
            s.pop();
            if (!p && !q)
                continue;
            if (!p || !q)
                return false;
            if (p->val != q->val)
                return false;
            s.push(p->left);
            s.push(q->left);
            s.push(p->right);
            s.push(q->right);
        }
        return true;
    }
};
```

## 对称二叉树


## 翻转二叉树
**描述**：

```
翻转一棵二叉树。

示例：

输入：

     4
   /   \
  2     7
 / \   / \
1   3 6   9

输出：

     4
   /   \
  7     2
 / \   / \
9   6 3   1
```

```cpp
class Solution {
public:
    TreeNode* invertTree(TreeNode* root) {
		if (root == NULL)
			return NULL;

		TreeNode *left = invertTree(root->left);
		TreeNode *right = invertTree(root->right);
		root->left = right;
		root->right = left;
		return root;
    }
};
```

## 两个二叉树的最低公共祖先节点

```cpp
TreeNode *getLastCommonParent(TreeNode *root, TreeNode *t1, TreeNode *t2)
{
	if (findNode(root->left, t1))
	{
		if (findNode(root->right, t2))
			return root;
		else
			return getLastCommonParent(root->left, t1, t2);
	}
	else
	{
		if (findNode(root->left, t2))
			return root;
		else
			return getLastCommonParent(root->right, t1, t2);
	}
}
// 查找节点node是否在当前二叉树中
bool findNode(TreeNode *root, TreeNode *node)
{
	if (root == NULL || node == NULL)
		return false;
	if (root == node)
		return true;

	bool found = findNode(root->left, node);
	if (!found)
		found = findNode(root->right, node);

	return found;
}
```


## 在二叉树中插入节点

```cpp
TreeNode *insertNode(TreeNode *root, TreeNode *node)
{
	if (root == node)
		return node;

	TreeNode *temp = new TreeNode;
	temp = root;

	TreeNode *last = NULL;
	while (temp != NULL)
	{
		last = temp;
		if (temp->val > node->val)
			temp = temp->left;
		else
			temp = temp->right;
	}
	if (last != NULL)
	{
		if (last->val > node->val)
			last->left = node;
		else
			last->right = node;
	}
	return root;
}
```

## 路径总和
**描述**：

```
给定一个二叉树和一个目标和，判断该树中是否存在根节点到叶子节点的路径，这条路径上所有节点值相加等于目标和。

说明: 叶子节点是指没有子节点的节点。

示例: 
给定如下二叉树，以及目标和 sum = 22，

              5
             / \
            4   8
           /   / \
          11  13  4
         /  \      \
        7    2      1
返回 true, 因为存在目标和为 22 的根节点到叶子节点的路径 5->4->11->2。
```

思路：

递归：

时间复杂度：`O(n)`，空间复杂度：`O(logn)`
```cpp
class Solution {
public:
    bool hasPathSum(TreeNode* root, int sum) {
        if(!root)
            return false;
        if(root->val == sum && !root->left && !root->right)
            return true;
        return hasPathSum(root->left, sum - root->val) || hasPathSum(root->right, sum - root->val);
    }
};
```

广度优先搜索 BFS
```cpp
class Solution {
public:
    bool hasPathSum(TreeNode* root, int sum) {
        if(!root)
            return false;
        queue<TreeNode *> q_node;
        queue<int> q_val;
        q_node.push(root);
        q_val.push(root->val);
        while(!q_node.empty())
        {
            TreeNode* p = q_node.front();
            int t = q_val.front();
            q_node.pop();
            q_val.pop();
            if(p->left == nullptr && p->right == nullptr)
            {
                if(t == sum)
                    return true;
                continue;
            }
            if(p->left)
            {
                q_node.push(p->left);
                q_val.push(p->left->val + t);
            }
            if(p->right)
            {
                q_node.push(p->right);
                q_val.push(p->right->val + t);
            }
        }
        return false;
    }
};
```

## 路径总和II
**描述**：

```
给定一个二叉树和一个目标和，找到所有从根节点到叶子节点路径总和等于给定目标和的路径。

说明: 叶子节点是指没有子节点的节点。

示例:
给定如下二叉树，以及目标和 sum = 22，

              5
             / \
            4   8
           /   / \
          11  13  4
         /  \    / \
        7    2  5   1
返回:
[
   [5,4,11,2],
   [5,8,4,5]
]
```

**思路**：

```cpp
class Solution {
private:
    vector<vector<int>> ans;

    void dfs(TreeNode* root, vector<int>& t, int sum)
    {
        if(root == NULL)
            return;

        t.push_back(root->val);

        // 是叶子节点且从根节点到叶子节点路径总和=sum -> 符合题目的路径
        if(root->val == sum && root->left == NULL && root->right == NULL)
            ans.push_back(t);
        
        if(root->left)
            dfs(root->left, t, sum - root->val);
        if(root->right)
            dfs(root->right, t, sum - root->val);
        // 弹出最后一个元素
        t.pop_back();
    }

public:
    vector<vector<int>> pathSum(TreeNode* root, int sum) {
        vector<int> t;
        dfs(root, t, sum);
        return ans;
    }
};
```

## 将有序数组转换为二叉搜索树
**描述**：

```
将一个按照升序排列的有序数组，转换为一棵高度平衡二叉搜索树。

本题中，一个高度平衡二叉树是指一个二叉树每个节点 的左右两个子树的高度差的绝对值不超过 1。

示例:

给定有序数组: [-10,-3,0,5,9],

一个可能的答案是：[0,-3,9,-10,null,5]，它可以表示下面这个高度平衡二叉搜索树：

      0
     / \
   -3   9
   /   /
 -10  5
```

**思路**：

分治法

时间复杂度：`O(n)`，空间复杂度：`O(logn)`

```cpp
class Solution {
private:
    template <typename RandomAccessIterator>
    TreeNode *sortedArrayToBST(RandomAccessIterator first,
                               RandomAccessIterator last) {
        const auto length = distance(first, last);
        // 终止条件
        if (length <= 0)
            return NULL;
        // 三方合并
        auto mid = first + length / 2;
        TreeNode *root = new TreeNode(*mid);
        root->left = sortedArrayToBST(first, mid);
        root->right = sortedArrayToBST(mid + 1, last);
        return root;
    }
public:
    TreeNode *sortedArrayToBST(vector<int> &num) {
        return sortedArrayToBST(num.begin(), num.end());
    }
};
```

## 有序链表转换二叉搜索树
**描述**：

```
给定一个单链表，其中的元素按升序排序，将其转换为高度平衡的二叉搜索树。

本题中，一个高度平衡二叉树是指一个二叉树每个节点 的左右两个子树的高度差的绝对值不超过 1。

示例:

给定的有序链表： [-10, -3, 0, 5, 9],

一个可能的答案是：[0, -3, 9, -10, null, 5], 它可以表示下面这个高度平衡二叉搜索树：

      0
     / \
   -3   9
   /   /
 -10  5
```

**思路**：

这题与 “将有序数组转换为二叉搜索树” 类似，但是单链表不能随机访问，而自顶向下的二分法必须需要 RandomAccessIterator，因此前面的方法不适合本题。存在一种自底向上的方法。

分治法：**自顶向下**

时间复杂度：`O(nlogn)`，空间复杂度：`O(logn)`（自己不是很确定）

```cpp
class Solution {
private:
    int listLength(ListNode *node) {
        int n = 0;
        while (node) {
            ++n;
            node = node->next;
        }
        return n;
    }

    ListNode *nth_node(ListNode *node, int n) {
        while (--n)
            node = node->next;
        return node;
    }

    TreeNode *sortedListToBST(ListNode *head, int len) {
        if (len == 0)
            return nullptr;
        if (len == 1)
            return new TreeNode(head->val);
        TreeNode *root = new TreeNode(nth_node(head, len / 2 + 1)->val);
        root->left = sortedListToBST(head, len / 2);
        root->right = sortedListToBST(nth_node(head, len / 2 + 2), (len - 1) / 2);
        return root;
    }

public:
    TreeNode *sortedListToBST(ListNode *head) {
        return sortedListToBST(head, listLength(head));
    }
};
```

分治法：**自底向上**

时间复杂度：`O(n)`，空间复杂度：`O(logn)`（自己不是很确定）

```cpp
class Solution {
private:
    TreeNode *sortedListToBST(ListNode *&list, int start, int end) {
        if (start > end)
            return nullptr;
        int mid = start + (end - start) / 2;
        TreeNode *leftChild = sortedListToBST(list, start, mid - 1);
        TreeNode *parent = new TreeNode(list->val);
        parent->left = leftChild;
        list = list->next;
        parent->right = sortedListToBST(list, mid + 1, end);
        return parent;
    }

public:
    TreeNode *sortedListToBST(ListNode *head) {
        int len = 0;
        ListNode *p = head;
        while (p) {
            len++;
            p = p->next;
        }
        return sortedListToBST(head, 0, len - 1);
    }
};
```

# 二叉搜索树
## 验证二叉搜索树
**描述**：

```
给定一个二叉树，判断其是否是一个有效的二叉搜索树。

假设一个二叉搜索树具有如下特征：

节点的左子树只包含小于当前节点的数。
节点的右子树只包含大于当前节点的数。
所有左子树和右子树自身必须也是二叉搜索树。

示例 1:
输入:
    2
   / \
  1   3
输出: true

示例 2:
输入:
    5
   / \
  1   4
     / \
    3   6
输出: false

解释: 输入为: [5,1,4,null,null,3,6]。
     根节点的值为 5 ，但是其右子节点值为 4 。
```

**思路**：

一棵 BST 定义为：
- 节点的左子树中的值要严格小于该节点的值。
- 节点的右子树中的值要严格大于该节点的值。
- 左右子树也必须是二叉查找树。
- 一个节点的树也是二叉查找树。

BST 的每个节点应该要小于右边子树的所有节点。不仅仅是和左右子节点比较，而是要和左子树和右子树的**所有**节点比较。

因此在框架外，可以使用辅助函数，增加函数参数列表，**在参数中携带额外信息**，如下：这样 root 可以对整棵左子树和右子树进行约束

使用 `long` 是防止越界溢出。[链接](https://leetcode-cn.com/problems/validate-binary-search-tree/solution/cjian-ji-dai-ma-ji-hu-shuang-bai-by-orangeman/)

```cpp
class Solution {
private:
    bool dfs(TreeNode* root, long min, long max) {
        if(!root)
		 	return true;
        if(root->val < min || root->val > max) 
			return false;
        return dfs(root->left, min, root->val - 1l) && dfs(root->right, root->val + 1l, max);
    }
public:
    bool isValidBST(TreeNode* root) {
       return dfs(root, INT_MIN, INT_MAX); 
    }
};
```







## 不同的二叉搜索树
**描述**：

```
给定一个整数 n，求以 1 ... n 为节点组成的二叉搜索树有多少种？

示例:

输入: 3
输出: 5
解释:
给定 n = 3, 一共有 5 种不同结构的二叉搜索树:

   1         3     3      2      1
    \       /     /      / \      \
     3     2     1      1   3      2
    /     /       \                 \
   2     1         2                 3
```

**思路**：[动态规划](https://leetcode-cn.com/problems/unique-binary-search-trees/solution/xi-xiang-yi-xia-huan-shi-hen-jian-dan-de-by-jerry-/)

对于每一个根i他都是由左边子树（1, 2, ..., i - 1)和右边子树 (i + 1, i + 2, ..., n) 组成的。因此他的个数肯定是两个子树情况的积。而且，这种根一共有 n 个，再将这些加起来就可以了。

```cpp
class Solution {
public:
    int numTrees(int n) {
        vector<int> dp(n + 1, 0);
        dp[0] = 1;
        dp[1] = 1;
        
        for(int i = 2; i <= n; i++)
            for(int j = 1; j <= i; j++){
                dp[i] += dp[j - 1] * dp[i - j];
            }
        return dp[n];
    }
};
```














# 未整合
## 二叉树的搜索区间

给定两个值 k1 和 k2（k1 < k2）和一个二叉查找树的根节点。找到树中所有值在 k1 到 k2 范围内的节点。即打印所有x (k1 <= x <= k2) 其中 x 是二叉查找树的中的节点值。返回所有升序的节点值。

```java
ArrayList<Integer> result;
ArrayList<Integer> searchRange(TreeNode root, int k1, int k2)
{
    result = new ArrayList<Integer>();
    searchHelper(root, k1, k2);
    return result;
}
void searchHelper(TreeNode root, int k1, int k2)
{
    if (root == null)
    {
        return;
    }
    if (root.val > k1)
    {
        searchHelper(root.left, k1, k2);
    }
    if (root.val >= k1 && root.val <= k2)
    {
        result.add(root.val);
    }
    if (root.val < k2)
    {
        searchHelper(root.right, k1, k2);
    }
}
```

## 二叉树内两个节点的最长距离

二叉树中两个节点的最长距离可能有三种情况：
1. 左子树的最大深度+右子树的最大深度为二叉树的最长距离
2. 左子树中的最长距离即为二叉树的最长距离
3. 右子树种的最长距离即为二叉树的最长距离
因此，递归求解即可

```java
private static class Result
{
    int maxDistance;
    int maxDepth;
    public Result() {
    }
    public Result(int maxDistance, int maxDepth) {
        this.maxDistance = maxDistance;
        this.maxDepth = maxDepth;
    }
}

int getMaxDistance(TreeNode root)
{
    return getMaxDistanceResult(root).maxDistance;
}

Result getMaxDistanceResult(TreeNode root)
{
    if (root == null)
    {
        Result empty = new Result(0, -1);
        return empty;
    }
    Result lmd = getMaxDistanceResult(root.left);
    Result rmd = getMaxDistanceResult(root.right);
    Result result = new Result();
    result.maxDepth = Math.max(lmd.maxDepth, rmd.maxDepth) + 1;
    result.maxDistance = Math.max(lmd.maxDepth + rmd.maxDepth, Math.max(lmd.maxDistance, rmd.maxDistance));
    return result;
}
```


# 参考资料
- [面试前准备：二叉树高频面试题和答案](https://mp.weixin.qq.com/s?__biz=MzUyNjQxNjYyMg==&mid=2247486365&idx=3&sn=0c9dd00f69159cfe2ad07899eaadb16f&chksm=fa0e641ccd79ed0a7b327fe338ff589499c0115048af185cc5d327ab6f4f8d85aec26f2de331&scene=21#wechat_redirect)



