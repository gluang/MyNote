# 总结
我们在选择排序算法的时候，通常会根据以下几个维度来考虑：
1. 时间复杂度
2. 空间复杂度（对内存空间的消耗）
3. 算法的稳定性（如果待排序的序列中存在值相等的元素，经过排序之后，相等元素之间原有的先后顺序不变）

**算法稳定性**：假设在数列中存在 a[i]=a[j] ，若在排序之前，a[i] 在 a[j] 前面；并且排序之后，a[i] 仍然在 a[j] 前面。则这个排序算法是稳定的

1. 插入排序：插入排序 -> 希尔排序
2. 选择排序：选择排序 -> 堆排序
3. 交换排序：冒泡排序 -> 快速排序
4. 归并排序
5. 基数排序、计数排序 -> 桶排序

**内部排序** 是数据记录在内存中进行排序（占用常数内存，不占用额外内存）

**外部排序** 是因排序的数据很大，一次不能容纳全部的排序记录，在排序过程中需要访问外存（占用额外内存）

排序方法|**平均**时间|最**坏**时间|最**好**时间| 空间 |稳定性|排序方式
--|--|--|--|--|--|--
**冒泡排序** | **O(n<sup>2</sup>)** | O(n<sup>2</sup>) | O(n)| O(1) | ✅ | 内部
**选择排序** | **O(n<sup>2</sup>)** | O(n<sup>2</sup>) | O(n<sup>2</sup>) | O(1) | ❌ | 内部
**插入排序** | **O(n<sup>2</sup>)** | O(n<sup>2</sup>) | O(n) | O(1) | ✅ | 内部
**希尔排序** | `O(n * logn)` | O(n<sup>2</sup>) | O(n) | O(1) | ❌ | 内部
**归并排序** | `O(n * logn)` |	O(n * logn) | O(n * logn) | O(n) |✅|❌外部
**快速排序** | `O(n * logn)` | O(n<sup>2</sup>) | O(logn) | O(logn) | ❌ |内部
**堆排序** | `O(n * logn)` | O(n * logn) | O(n * logn) | O(1) | ❌ | 内部
**计数排序** | O(n + k) | O(n + k) | O(n + k) | O(k) | ✅ | ❌外部
**基数排序** | O(n * k) | O(n * k) | O(n * k) | O(n + k) | ✅ | ❌外部
**桶排序** | O(n + k) | O(n<sup>2</sup>) | O(n + k) | O(n + k) | ✅ | ❌外部


规律：
1. O(n<sup>2</sup>)、O(n * logn)、O(n + k)
2. 不稳定排序：选择、希尔、快排、堆
3. 外部排序：归并、基数、计数、桶（这几个都需要 new，其他排序方式则不用）
4. 空间复杂度发生变化：快排、基数、计数、桶


# 参考资料
规定：
```
arr
len
start
end
```

测试代码：
```cpp
int main()
{
    int a[] = {53, 3, 542, 748, 14, 214, 154, 63, 616};
    int n = sizeof(a) / sizeof(int);

    std::cout << "原先顺序： ";
    for (int i = 0; i < n; i++)
        std::cout << a[i] << " ";
    std::cout << std::endl;


    int correct[] = {3, 14, 53, 63, 154, 214, 542, 616, 748};
    std::cout << "正确顺序： ";
    for (int i = 0; i < n; i++)
        std::cout << correct[i] << " ";
    std::cout << std::endl;

    quick_sort(a, n);

    std::cout << "排序顺序： ";
    for (int i = 0; i < n; i++)
        std::cout << a[i] << " ";
    std::cout << std::endl;

    return 0;
}
```

- 冒泡排序：[原理](http://wangkuiwu.github.io/2013/05/01/bubble-sort/)、[C++ 实现](https://github.com/wangkuiwu/datastructs_and_algorithm/blob/master/source/algrightm/sort/bubble/cplus/BubbleSort.cpp)、[Go 实现](https://geekr.dev/posts/go-sorting-algorithm-bubble)
- 选择排序：[原理](http://wangkuiwu.github.io/2013/05/05/select-sort/)、[C++ 实现](https://github.com/wangkuiwu/datastructs_and_algorithm/blob/master/source/algrightm/sort/selection_sort/cplus/SelectSort.cpp)、[Go 实现](https://geekr.dev/posts/go-sorting-algorithm-selection)
- 插入排序：[原理](http://wangkuiwu.github.io/2013/05/03/insert-sort/)、[C++ 实现](https://github.com/wangkuiwu/datastructs_and_algorithm/blob/master/source/algrightm/sort/insert_sort/cplus/InsertSort.cpp)、[Go 实现](https://geekr.dev/posts/go-sorting-algorithm-insertion)
- 快速排序：[原理](http://wangkuiwu.github.io/2013/05/02/quick-sort/)、[C++ 实现](https://github.com/wangkuiwu/datastructs_and_algorithm/blob/master/source/algrightm/sort/quick_sort/cplus/QuickSort.cpp)、[Go 实现](https://geekr.dev/posts/go-sorting-algorithm-quick)
- 堆排序：[原理](http://wangkuiwu.github.io/2013/05/06/heap-sort/)、[C++ 实现](https://github.com/wangkuiwu/datastructs_and_algorithm/blob/master/source/algrightm/sort/heap_sort/cplus/HeapSort.cpp)
- 希尔排序：[原理](http://wangkuiwu.github.io/2013/05/04/shell-sort/)、[C++ 实现](https://github.com/wangkuiwu/datastructs_and_algorithm/blob/master/source/algrightm/sort/shell_sort/cplus/ShellSort.cpp)
- 归并排序：[原理](http://wangkuiwu.github.io/2013/05/08/merge-sort/)、[C++ 实现](https://github.com/wangkuiwu/datastructs_and_algorithm/blob/master/source/algrightm/sort/merge_sort/cplus/MergeSort.cpp)、[Go 实现](https://geekr.dev/posts/go-sorting-algorithm-merge)
- 基数排序：[原理](http://wangkuiwu.github.io/2013/05/10/radix-sort/)、[C++ 实现](https://github.com/wangkuiwu/datastructs_and_algorithm/blob/master/source/algrightm/sort/radix_sort/cplus/RadixSort.cpp)
- 桶排序：[原理](http://wangkuiwu.github.io/2013/05/09/bucket-sort/)、[C++ 实现](https://github.com/wangkuiwu/datastructs_and_algorithm/blob/master/source/algrightm/sort/bucket_sort/cplus/BucketSort.cpp)
- [十大经典排序算法(动态演示+代码)](https://mp.weixin.qq.com/s/fJEiPQ_iLvz0ecS9Cb2DJg)
- [基数排序、计数排序和桶排序的区别](https://blog.csdn.net/qq_19446965/article/details/81517552)
- [十大经典排序算法动画与解析](https://www.cxyxiaowu.com/2026.html)
