# LeetCode

题号|题目|难度|完成度|备注
--|--|--|--|--
26|[删除排序数组中的重复项](#删除排序数组中的重复项)|🟢|✔|快慢指针（对应 2.1.1）


🟢 🔴 🟡 ✔ ✖

80

## 删除排序数组中的重复项
**描述**：

```
给定一个排序数组，你需要在 原地 删除重复出现的元素，使得每个元素只出现一次，返回移除后数组的新长度。

不要使用额外的数组空间，你必须在 原地 修改输入数组 并在使用 O(1) 额外空间的条件下完成。

示例 1:
    给定数组 nums = [1,1,2], 
    函数应该返回新的长度 2, 并且原数组 nums 的前两个元素被修改为 1, 2。 
    你不需要考虑数组中超出新长度后面的元素。

示例 2:
    给定 nums = [0,0,1,1,1,2,2,3,3,4],
    函数应该返回新的长度 5, 并且原数组 nums 的前五个元素被修改为 0, 1, 2, 3, 4。
    你不需要考虑数组中超出新长度后面的元素。
```

对于数组来说，在尾部插入、删除元素是比较高效的，时间复杂度是 `O(1)`，但是如果在中间或者开头插入、删除元素，就会涉及数据的搬移，时间复杂度为 `O(n)`，效率较低。

对于数组相关的算法问题，有一个通用的技巧：**要尽量避免在中间删除元素，那我就先想办法把这个元素换到最后去**。这样的话，最终待删除的元素都拖在数组尾部，一个一个 pop 掉就行了，每次操作的时间复杂度也就降低到 O(1) 了。

因此衍生出：双指针技巧（快慢指针）：我们让慢指针 slow 走左后面，快指针 fast 走在前面探路，找到一个不重复的元素就告诉 slow 并让 slow 前进一步。这样当 fast 指针遍历完整个数组 nums 后，nums[0..slow] 就是不重复元素，之后的所有元素都是重复元素。

时间复杂度：`O(n)`、空间复杂度：`O(1)`

```cpp
int removeDuplicates(vector<int>& nums) {
    if (nums.empty())
        return 0;

    int slow = 0, fast = 1;
    while (fast < nums.size()) {
        if (nums[fast] != nums[slow]) {
            slow ++;
            // 维护 nums[0, ..., slow] 无重复
            nums[slow] = nums[fast];
        }
        fast++;
    }
    // 长度为 索引+1
    return slow + 1;
}
```

![](images/有序数组去重-2.gif)


**思路 2**：使用 STL

时间复杂度：`O(n)`、空间复杂度：`O(1)`

```cpp
class Solution {
public:
	int removeDuplicates(vector<int>& nums) {
		return distance(nums.begin(), unique(nums.begin(), nums.end()));
	}
};
```


# 待整理
## Remove Duplicates from Sorted Array
- 描述：

    Given a sorted array, remove the duplicates in place such that each element appear only once and return the new length.

    Do not allocate extra space for another array, you must do this in place with constant memory.

    For example, Given input array A = [1,1,2],

    Your function should return length = 2, and A is now [1,2].

- 双指针：时间复杂度 O(n)、空间复杂度 O(1)

    ```cpp
    int removeDuplicates(vector<int> &nums)
    {
        if (nums.empty())
            return 0;

        int index = 0;
        for (size_t i = 1; i < nums.size(); i++)
        {
            if (nums[index] != nums[i])
            {
                nums[++index] = nums[i];
            }
        }
        return index + 1;
    }
    ```

- STL：时间复杂度 O(n)、空间复杂度 O(1)

    ```cpp
    int removeDuplicates(vector<int> &nums)
    {
        return distance(nums.begin(), unique(nums.begin(), nums.end()));
    }
    ```

- STL：时间复杂度 O(n)、空间复杂度 O(1)

    ```cpp
    // TODO: 不是很理解这个过程
    template <typename InIt, typename OutIt>
    OutIt removeDuplicates(InIt first, InIt Last, OutIt output)
    {
        while (first != last)
        {
            *output++ = *first;
            first = upper_bound(first, Last, *first);
        }
        return output;
    }

    int removeDuplicates(vector<int> &nums)
    {
        return distance(nums.begin(), removeDuplicates(nums.begin(), nums.end(), nums.begin()));
    }
    ```

## Remove Duplicates from Sorted Array II
- 描述：

    Follow up for ”Remove Duplicates”: What if duplicates are allowed at most twice?

    For example, Given sorted array A = [1,1,1,2,2,3],

    Your function should return length = 5, and A is now [1,1,2,2,3]

- 分析：

    由于已经是排序的数组，加一个变量记录一下元素出现的次数即可。如果是还没排序的数组，则需要引入一个 hashmap 来记录出现的次数

-  双指针：时间复杂度 O(n)、空间复杂度 O(1)

    ```cpp
    int removeDuplicates(vector<int> &nums)
    {
        if (nums.size() <= 2)
        {
            return nums.size();
        }

        // 将 index 改为 3，即可允许重复最多 3 次
        int index = 2;
        for (size_t i = 2; i < nums.size(); i++)
        {
            if (nums[i] != nums[index - 2])
            {
                nums[index++] = nums[i];
            }
            return index;
        }

        return index;
    }
    ```

- 简洁版本：时间复杂度 O(n)、空间复杂度 O(1)

    ```cpp
    int removeDuplicates(vector<int> &nums)
    {
        const size_t n = nums.size();
        int index = 0;
        for (size_t i = 0; i < n; i++)
        {
            if (i > 0 && i < n - 1 && nums[i] == nums[i - 1] && nums[i] == nums[i + 1])
                continue;
            nums[index++] = nums[i];
        }

        return index;
    }
    ```










## Remove Element
- 描述：

    Given an array and a value, remove all instances of that value in place and return the new length.

    The order of elements can be changed. It doesn’t matter what you leave beyond the new length.

- 代码：

    1. 双指针：时间复杂度 O(n) 空间复杂度 O(1)

    ```cpp
    int removeElement(vector<int> &nums, int target)
    {
        int index = 0;
        for (size_t i = 0; i < nums.size(); i++)
        {
            if (nums[i] != target)
            {
                nums[index++] = nums[i];
            }
        }
        return index;
    }
    ```

    2. STL：时间复杂度 O(n) 空间复杂度 O(1)

    ```cpp
    int removeElement(vector<int> &nums, int target)
    {
        return distance(nums.begin(), remove(nums.begin(), nums.end(), target));
    }
    ```



## Plus One
- 描述：

    Given a number represented as an array of digits, plus one to the number.

- 分析：

    高精度加法

- 代码：
