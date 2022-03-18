#include <iostream>

// 冒泡排序-1：两两比较
void bubble_sort_1(int *arr, int len) {
    for (int i = len - 1; i > 0; --i) {
        for (int j = 0; j < i; ++j) {
            if (arr[ j ] > arr[ j + 1 ])
                std::swap(arr[ j ], arr[ j + 1 ]);
        }
    }
}
// 冒泡排序-2：增加哨兵
void bubble_sort_2(int *arr, int len) {
    bool flag = false;
    for (int i = len - 1; i > 0; --i) {
        for (int j = 0; j < i; ++j) {
            if (arr[ j ] > arr[ j + 1 ]) {

                std::swap(arr[ j ], arr[ j + 1 ]);
                flag = true;
            }
        }
        if (!flag) {
            break;
        }
    }
}

// 选择排序：寻找最值
void select_sort(int *arr, int len) {
    for (int i = 0; i < len; ++i) {
        int min = i;
        for (int j = i + 1; j < len; ++j) {
            if (arr[ j ] < arr[ min ]) {
                min = j;
            }
        }
        if (min != i) {
            std::swap(arr[ i ], arr[ min ]);
        }
    }
}

// 插入排序：类似整理扑克牌序
void insert_sort(int *arr, int len) {
    for (int i = 1; i < len; ++i) {
        int key = arr[ i ];
        int j   = i - 1;
        while ((j >= 0) && (key < arr[ j ])) {
            arr[ j + 1 ] = arr[ j ];
            j--;
        }
        arr[ j + 1 ] = key;
    }
}

// 希尔排序：对半分组，再使用插入排序
void shell_sort(int *arr, int len) {
    for (int gap = len / 2; gap > 0; gap = gap / 2) {
        for (int i = gap; i < len; ++i) {
            int j = i - gap;
            if (arr[ i ] < arr[ j ]) {
                // 插入排序
                int temp = arr[ i ];
                for (; j >= 0 && arr[ j ] > temp; j -= gap) {
                    arr[ j + gap ] = arr[ j ];
                }
                arr[ j + gap ] = temp;
            }
        }
    }
}

// 归并排序：分解再合并
// 递归版本
void merge(int *arr, int start, int mid, int end) {
    int *temp  = new int[ end - start + 1 ];
    int  i     = start;
    int  j     = mid + 1;
    int  index = 0;

    while (i <= mid && j <= end) {
        if (arr[ i ] <= arr[ j ])
            temp[ index++ ] = arr[ i++ ];
        else
            temp[ index++ ] = arr[ j++ ];
    }
    while (i <= mid) {
        temp[ index++ ] = arr[ i++ ];
    }
    while (j <= mid) {
        temp[ index++ ] = arr[ j++ ];
    }
    for (int t = 0; t < index; ++t) {
        arr[ start + t ] = temp[ t ];
    }
    delete[] temp;
}
void merge_sort(int *arr, int start, int end) {
    if (arr == nullptr || start >= end)
        return;
    int mid = (start + end) / 2;
    merge_sort(arr, start, mid);
    merge_sort(arr, mid, end);
    merge(arr, start, mid, end);
}
// 迭代版本：
void merge_sort_recursive(int *arr, int len) {
    int *temp = new int[ len ];
    for (int seg = 1; seg < len; seg += seg) {
        for (int start = 0; start < len; start += seg + seg) {
            int mid    = std::min(start + seg, len);
            int high   = std::min(start + seg + seg, len);
            int index  = start;
            int start1 = start, end1 = mid;
            int start2 = mid, end2 = high;
            while (start1 < end1 && start2 < end2) {
                temp[ index++ ] = arr[ start1 ] < arr[ start2 ] ? arr[ start1++ ] : arr[ start2 ];
            }
            while (start1 < end1) {
                temp[ index++ ] = arr[ start1 ];
            }
            while (start2 < end2) {
                temp[ index++ ] = arr[ start2 ];
            }

            for (int i = 0; i < len; ++i) {
                arr[ i ] = temp[ i ];
            }
            delete[] temp;
        }
    }
}

// 快速排序：平衡二叉树
void quick_sort_recursive(int *arr, int start, int end) {
    if (start >= end)
        return;
    int left  = start;
    int right = end;
    int key   = arr[ left ];
    while (left < right) {
        while (left < right && arr[ right ] > key)
            right--;
        while (left < right && arr[ left ] < key)
            left++;
        if (left < right) {
            std::swap(arr[ left ], arr[ right ]);
        }
    }
    arr[ left ] = key;
    quick_sort_recursive(arr, start, left - 1);
    quick_sort_recursive(arr, left + 1, end);
}
void quick_sort(int *arr, int len) {
    quick_sort_recursive(arr, 0, len - 1);
}

// 堆排序
void max_heap(int *arr, int start, int end) {
    int cur  = start;
    int left = 2 * cur + 1;
    int temp = arr[ cur ];
    for (; left <= end; cur = left, left = 2 * left + 1) {
        if (left < end && arr[ left ] < arr[ left + 1 ])
            left++;
        if (temp < arr[ left ]) {
            arr[ cur ]  = arr[ left ];
            arr[ left ] = temp;
        } else {
            break;
        }
    }
}
void heap_sort(int *arr, int len) {
    for (int i = len / 2 - 1; i >= 0; i--) {
        max_heap(arr, i, len - 1);
    }
    for (int i = len - 1; i > 0; i--) {
        std::swap(arr[ 0 ], arr[ i ]);
        max_heap(arr, 0, i - 1);
    }
}

// 基数排序：从低位到高位
void buckets_sort(int *arr, int len, int exp) {
    int *temp          = new int[ len ];
    int  buckets[ 10 ] = {0};
    for (int i = 0; i < len; ++i) {
        buckets[ (arr[ i ] / exp) % 10 ]++;
    }
    for (int i = 1; i < 10; ++i) {
        buckets[ i ] += buckets[ i - 1 ];
    }
    for (int i = len - 1; i >= 0; i--) {
        temp[ buckets[ (arr[ i ] / exp % 10) ] - 1 ] = arr[ i ];
        buckets[ (arr[ i ] / exp % 10) ]--;
    }
    for (int i = 0; i < len; ++i) {
        arr[ i ] = temp[ i ];
    }
    delete[] temp;
}
void radix_sort(int *arr, int len) {
    int max = arr[ 0 ];
    for (int i = 1; i < len; ++i) {
        if (arr[ i ] > max) {
            max = arr[ i ];
        }
    }
    for (int exp = 1; max / exp > 0; exp *= 10) {
        buckets_sort(arr, len, exp);
    }
}

// 计数排序
void count_sort(int *arr, int len) {
    int min = arr[ 0 ], max = arr[ 0 ];
    for (int i = 1; i < len; ++i) {
        if (arr[ i ] > max) {
            max = arr[ i ];
        } else if (arr[ i ] < min) {
            min = arr[ i ];
        }
    }
    int  vaild = max - min + 1;
    int *count = new int[ vaild ];
    for (int i = 0; i < len; ++i) {
        count[ arr[ i ] - min ]++;
    }
    for (int i = 1; i < vaild; ++i) {
        count[ i ] += count[ i - 1 ];
    }

    int *rs = new int[ len ];
    for (int i = len - 1; i >= 0; --i) {
        rs[ --count[ arr[ i ] - min ] ] = arr[ i ];
    }
    for (int i = 0; i < len; ++i) {
        arr[ i ] = rs[ i ];
    }
    delete[] rs;
    delete[] count;
}