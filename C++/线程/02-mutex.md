<h1 align="center">目录</h1>

* [1. 互斥量](#1-互斥量)
    * [1.1. 类型](#11-类型)
    * [1.2. shared_mutex](#12-shared_mutex)
* [2. 锁](#2-锁)
    * [2.1. std::lock_guard](#21-stdlock_guard)
    * [2.2. std::unique_lock](#22-stdunique_lock)
    * [2.3. std::shared_lock](#23-stdshared_lock)
    * [2.4. std::scoped_lock](#24-stdscoped_lock)
* [3. 进行延迟初始化——std::call_once](#3-进行延迟初始化stdcall_once)
* [4. 实例](#4-实例)
    * [4.1. 数据竞争和互斥对象](#41-数据竞争和互斥对象)
    * [4.2. 实现互斥锁](#42-实现互斥锁)
    * [4.3. 同步并行中使用std::cout](#43-同步并行中使用stdcout)

---

# 1. 互斥量

## 1.1. 类型
对比 STL 中的一些不同的互斥量：

类型名 |描述
--|--
`mutex`|具有 `lock` 和 `unlock` 成员函数的标准互斥量。并提供非阻塞函数 `try_lock` 成员函数
`timed_mutex`|与互斥量相同，并提供 `try_lock_for` 和 `try_lock_until` 成员函数，其能在某个时间段内对程序进行阻塞
`recursive_mutex`|与互斥量相同，不过当一个线程对实例进行上锁，其可以对同一个互斥量对象多次调用 `lock` 而不产生阻塞。持有线程可以多次调用 `unlock` ，不过需要和 lock 调用的次数匹配时，线程才不再拥有这个互斥量
`recursive_timed_mutex` |提供与 `timed_mutex` 和 `ecursive_mutex` 的特性
`shared_mutex`|这个互斥量在这方面比较特殊，它可以被锁定为独占模式或共享模式。独占模式时，其与标准互斥量的行为一样。共享模式时，其他线程也可能在共享模式下对其进行上锁。其会在最后一个处于共享模式下的锁拥有者进行解锁时，整个互斥量才会解锁。其行为有些类似于 `shared_ptr` ，只不过互斥量不对内存进行管理，而是对锁的所有权进行管理
`shared_timed_mutex`|同时具有 `shared_mutex` 和 `timed_mutex` 两种互斥量独占模式和共享模式的特性


## 1.2. shared_mutex

> `std::shared_mutex` 是 C++17 的内容

* `std::shared_mutex` 底层实现主要原理是操作系统提供的读写锁，也就是说，**在存在多个线程对共享资源读的情况少许线程对共享资源写的情况下**，`std::shared_mutex` 比 `std::mutex` 效率更高

* `std::shared_mutex` 提供了 
    
    * `lock()` 和 `unlock()` 方法获取**写锁**和解除**写锁**
    * 提供了 `lock_shared()` 和 `unlock_shared()` 方法获取**读锁**和解除**读锁**
    * 写锁模式我们称为**排他锁**（Exclusive Locking），读锁模式我们称为**共享锁**（Shared Locking）

* C++ 新标准中引入与 `std::shared_mutex` 配合使用的 `std::unique_lock`、`std::shared_lock` 两个对象用于出了锁进入作用域自动加锁、出了作用域自动解除锁
    
    * `std::unique_lock` 用于加解 `std::shared_mutex `的**写锁**
    * `std::shared_lock` 用于加解 `std::shared_mutex` 的**读锁**

如果条件允许，建议读者认真甄别实际场景，可以使用 `std::shared_mutex` 去替代部分 `std::mutex`，以提高程序执行效率。

下面是对共享资源存在多个读线程和一个写线程，分别使用 `std::mutex` 和 `std::shared_mutex` 做的一个性能测试

```cpp
//读线程数量
#define READER_THREAD_COUNT 8
//最大循环次数
#define LOOP_COUNT 5000000

#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>

class shared_mutex_counter {
public:
    shared_mutex_counter()  = default;
    ~shared_mutex_counter() = default;

    //使用std::shared_mutex，同一时刻多个读线程可以同时访问value_值
    unsigned int get() const {
        //注意：这里使用std::shared_lock
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return value_;
    }

    //使用std::shared_mutex，同一个时刻仅有一个写线程可以修改value_值
    void increment() {
        //注意：这里使用std::unique_lock
        std::unique_lock<std::shared_mutex> lock(mutex_);
        value_++;
    }

    //使用std::shared_mutex，同一个时刻仅有一个写线程可以重置value_值
    void reset() {
        //注意：这里使用std::unique_lock
        std::unique_lock<std::shared_mutex> lock(mutex_);
        value_ = 0;
    }

private:
    mutable std::shared_mutex mutex_;
    // value_是多个线程的共享资源
    unsigned int value_ = 0;
};

class mutex_counter {
public:
    mutex_counter()  = default;
    ~mutex_counter() = default;

    //使用std::mutex，同一时刻仅有一个线程可以访问value_的值
    unsigned int get() const {
        std::unique_lock<std::mutex> lk(mutex_);
        return value_;
    }

    //使用std::mutex，同一时刻仅有一个线程可以修改value_的值
    void increment() {
        std::unique_lock<std::mutex> lk(mutex_);
        value_++;
    }

private:
    mutable std::mutex mutex_;
    // value_是多个线程的共享资源
    unsigned int value_ = 0;
};

//测试std::shared_mutex
void test_shared_mutex() {
    shared_mutex_counter counter;
    int                  temp;

    //写线程函数
    auto writer = [ &counter ]() {
        for (int i = 0; i < LOOP_COUNT; i++) {
            counter.increment();
        }
    };

    //读线程函数
    auto reader = [ &counter, &temp ]() {
        for (int i = 0; i < LOOP_COUNT; i++) {
            temp = counter.get();
        }
    };

    //存放读线程对象指针的数组
    std::thread **tarray = new std::thread *[ READER_THREAD_COUNT ];

    //记录起始时间
    clock_t start = clock();

    //创建READER_THREAD_COUNT个读线程
    for (int i = 0; i < READER_THREAD_COUNT; i++) {
        tarray[ i ] = new std::thread(reader);
    }

    //创建一个写线程
    std::thread tw(writer);

    for (int i = 0; i < READER_THREAD_COUNT; i++) {
        tarray[ i ]->join();
    }
    tw.join();

    //记录起始时间
    clock_t end = clock();
    printf("[test_shared_mutex]\n");
    printf("thread count: %d\n", READER_THREAD_COUNT);
    printf("result: %d cost: %dms temp: %d \n", counter.get(), end - start, temp);
}

//测试std::mutex
void test_mutex() {
    mutex_counter counter;

    int temp;

    //写线程函数
    auto writer = [ &counter ]() {
        for (int i = 0; i < LOOP_COUNT; i++) {
            counter.increment();
        }
    };

    //读线程函数
    auto reader = [ &counter, &temp ]() {
        for (int i = 0; i < LOOP_COUNT; i++) {
            temp = counter.get();
        }
    };

    //存放读线程对象指针的数组
    std::thread **tarray = new std::thread *[ READER_THREAD_COUNT ];

    //记录起始时间
    clock_t start = clock();

    //创建READER_THREAD_COUNT个读线程
    for (int i = 0; i < READER_THREAD_COUNT; i++) {
        tarray[ i ] = new std::thread(reader);
    }

    //创建一个写线程
    std::thread tw(writer);

    for (int i = 0; i < READER_THREAD_COUNT; i++) {
        tarray[ i ]->join();
    }
    tw.join();

    //记录结束时间
    clock_t end = clock();
    printf("[test_mutex]\n");
    printf("thread count:%d\n", READER_THREAD_COUNT);
    printf("result:%d cost:%dms temp:%d \n", counter.get(), end - start, temp);
}

int main() {
    //为了排除测试程序的无关因素，测试时只开启一个
    test_mutex();
    // test_shared_mutex();
    return 0;
}
```

```
[test_mutex]
thread count:8
result:5000000 cost:33996555ms temp:5000000

[test_shared_mutex]
thread count: 8
result: 5000000 cost: 41062327ms temp: 25987
```


# 2. 锁
C++17 STL 提供了如下的 RAII 辅助锁。其都能接受一个模板参数，其与互斥量的类型相同(在 C++17 中，编译器可以自动推断出相应的类型)：

名称 |描述
--|--
`lock_guard`|这个类没有什么其他的，构造函数中调用 lock ，析构函数中调用 unlock
`scoped_lock`|与 `lock_guard` 类似，构造函数支持多个互斥量。析构函数中会以相反的顺序进行解锁
`unique_lock`|使用独占模式对互斥量进行上锁。构造函数也能接受一个参数用于表示超时到的时间，并不会让锁一直处于上锁的状态。其也可能不对互斥量进行上锁，或是假设互斥量已经锁定，或是尝试对互斥量进行上锁。另外，函数可以在 `unique_lock` 锁的声明周期中，对互斥量进行上锁或解锁
`shared_lock`|与 `unique_lock` 类似，不过所有操作都是在互斥量的共享模式下进行操作

`lock_guard` 和 `scoped_lock` 只拥有构造和析构函数， `unique_lock` 和 `shared_lock` 就比较复杂了，但也更为通用。

* `std::unique_lock` 是 C++11 的内容
* `std::shared_lock` 是 C++14 的内容
* `std::scoped_lock` 是 C++17 的内容


## 2.1. std::lock_guard
C++11 还为互斥量提供了一个 RAII 语法的模板类 `std::lock_guard`。`lock_guard` 的构造函数能接受一个互斥量，其会立即自动调用 lock ，构造函数会直到获取互斥锁为止。当实例进行销毁时，其会对互斥量再次进行解锁。这样互斥量就很难陷入到 lock/unlock 循环错误中。

【避免死锁】：

* `lock` 的顺序一致
* 使用 `lock_guard<std::mutex, std::adopt_lock>` 用于局部函数，用完解锁

```cpp
std::lock_guard<std::mutex> locker1(mutex, std::adopt_lock);
std::lock_guard<std::mutex> locker2(mutex, std::adopt_lock);
// std::adopt_lock ：告诉 locker1，m_mutex 被锁住了
// 这样一来，locker 的顺序可以不一致了
```


```cpp
// 7.2.critical.section.a.cpp

#include <iostream>
#include <mutex>
#include <thread>

int v = 1;

void critical_section(int change_v) {
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);

    // 执行竞争操作
    v = change_v;

    // 离开此作用域后 mtx 会被释放
}

int main() {
    std::thread t1(critical_section, 2), t2(critical_section, 3);
    t1.join();
    t2.join();

    std::cout << v << std::endl;
    return 0;
}
```

由于 C++ 保证了所有栈对象在生命周期结束时会被销毁，所以这样的代码也是异常安全的。无论 `critical_section()` 正常返回、还是在中途抛出异常，都会引发堆栈回退，也就自动调用了 `unlock()`。


## 2.2. std::unique_lock
`std::unique_lock` 则相对于 `std::lock_guard` 出现的，`std::unique_lock` 更加灵活，`std::unique_lock` 的对象会以独占所有权（没有其他的 `unique_lock` 对象同时拥有某个 `mutex` 对象的所有权）的方式管理 `mutex` 对象上的上锁和解锁的操作。所以在并发编程中，推荐使用 `std::unique_lock`。

【区别】：

* `std::lock_guard` 不能显式的调用 `lock` 和 `unlock`， 而 `std::unique_lock` 可以在声明后的任意位置调用，可使用 `locker.unlock()` 解锁掉其他不需要加锁的变量，缩小锁的作用范围，提供更高的并发度。
* 在 locker 的构造函数中还可添加 `std::defer_lock` ，用来告诉 mutex 没被锁住。`defer_lock` 是一个空的全局对象，其不会对互斥量立即上锁，可以通过这个参数对 `unique_lock` 不同的构造函数进行选择，之后可以调用 `l.try_lock()` ，其会告诉我们有没有上锁。
* `std::unique_lock` 可移动，不可复制；而 `std::lock_guard` 既不可移动也不可复制，但 `unique_lock` 提供的弹性功能相较于 `std::lock_guard` 要更消耗内存
* 如果用到了条件变量 `std::condition_variable::wait` 则**必须**使用 `std::unique_lock` 作为参数。

```cpp
// 推荐 std::defer_lock 加上 lock() 和 unlock() 一起使用
// 不然运行时会有 warning info
locker.lock()   
...     // 指定代码
locker.unlock()
...     // 后面还可再调用锁
```

* **可灵活使用 `{ }` 这种构造局部函数的方法来构造线程安全**


```cpp
// 7.3.critical.section.b.cpp

#include <iostream>
#include <mutex>
#include <thread>

int v = 1;

void critical_section(int change_v) {
    static std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    // 执行竞争操作
    v = change_v;
    std::cout << v << std::endl;
    // 将锁进行释放
    lock.unlock();

    // 在此期间，任何人都可以抢夺 v 的持有权

    // 开始另一组竞争操作，再次加锁
    lock.lock();
    v += 1;
    std::cout << v << std::endl;
}

int main() {
    std::thread t1(critical_section, 2), t2(critical_section, 3);
    t1.join();
    t2.join();
    return 0;
}
```

```
2
3
3
4
```

`unique_lock` 常见的 2 种用法：

* 单纯在函数中使用 `unique_lock` 
* 用 `std::defer_lock` ，然后对需要加锁的地方使用 `lock()`，对不需要加锁的地方使用 `unlock()`

```cpp
// 实例4：使用unique_lock
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

class LogFile {
public:
    LogFile() {
        m_flog.open("log.txt");
    }
    ~LogFile() {
        m_flog.close();
    }
    void shared_print(std::string id, int value) {
        // -> 对整个局部变量进行加锁
        // std::lock_guard<std::mutex> guard(m_mutex);  
        
        // -> 能够对局部中其他不需要加锁的变量解锁 
        // 对象构造还可添加std::defer_lock
        std::unique_lock<std::mutex> locker(m_mutex, std::defer_lock);
        locker.lock();
        m_flog << "From " << id << " : " << value << std::endl;
        locker.unlock();
    }

private:
    std::mutex    m_mutex;
    std::ofstream m_flog;
};

void fun1(LogFile &log) {
    for (int i = 100; i >= 0; i--)
        log.shared_print("fun1", i);
}

int main() {
    LogFile     m_logFile;
    std::thread th1(fun1, std::ref(m_logFile));
    for (int i = 0; i <= 100; i++)
        m_logFile.shared_print("main", i);

    th1.join();
    return 0;
}
```

## 2.3. std::shared_lock
可以使用同一个互斥量实例化很多的 `shared_lock` 实例。

【实例】`shared_mutex`、`shared_lock` 和 `unique_lock`：

使用 `std::shared_mutex` 在独占( `exclusive` )和共享( `shared` )模式下来完成一段程序。

```cpp
// 9_4_shared_lock.cpp

#include <iostream>
#include <shared_mutex>
#include <thread>
#include <vector>
// 打造异常安全的共享锁
using namespace std;
using namespace chrono_literals;

shared_mutex shared_mut;

using shrd_lck = shared_lock<shared_mutex>;
// 使用 unique_lock 实例来作为共享互斥锁
// 其构造函数的第二个参数 defer_lock 会告诉对象让锁处于解锁的状态
// 否则，构造函数会尝试对互斥量上锁阻塞程序，直至成功为止
using uniq_lck = unique_lock<shared_mutex>;

void print_exclusive()
{
    uniq_lck l {shared_mut, defer_lock};

    // 调用 exclusive_lock 的成员函数 try_lock
    // 该函数会立即返回，并且返回相应的布尔值
    // 布尔值表示互斥量是否已经上锁，或是在其他地方已经锁住
    if (l.try_lock()) {
        cout << "Got exclusive lock.\n";
    } else {
        cout << "Unable to lock exclusively.\n";
    }
}

// 即便是抛出异常退出， exclusive_throw 函数依旧会让互斥量再度锁上
void exclusive_throw()
{
    // 将程序阻塞，直至其获取相应的锁
    uniq_lck l {shared_mut};
    // 模拟发生错误情况，但是互斥量还是可以被释放
    // 因为 unique_lock 的析构函数在任何情况下都会将对应的锁进行释放
    throw 123;

}

int main()
{
    {
        // 实例化一个 shared_lock
        // 其构造函数将会立即对共享模式下的互斥量进行上锁
        shrd_lck sl1 {shared_mut};

        cout << "shared lock once.\n";

        {
            // 使用同一个互斥量实例化第二个 shared_lock
            // 现在具有两个 shared_lock 实例，并且都具有同一个互斥量的共享锁
            shrd_lck sl2 {shared_mut};

            cout << "shared lock twice.\n";

            // 尝试使用互斥量的独占模式对互斥量进行上锁
            // 这样的调用当然不会成功，因为互斥量已经在共享模式下锁住了
            print_exclusive();
        }   // shared_locks12 的析构函数将会释放互斥量的共享锁

        cout << "shared lock once again.\n";

        // print_exclusive 函数还是失败
        // 这是因为互斥量依旧处于共享锁模式
        print_exclusive();

    }
    cout << "lock is free.\n";

    try {
        exclusive_throw();
    } catch (int e) {
        cout << "Got exception " << e << '\n';
    }

    print_exclusive();

    // 当 sl1 和 sl2 存在的情况下， print_exclusive 无法使用独占模式对互斥量进行上锁
}
```

```
shared lock once.
shared lock twice.
Unable to lock exclusively.
shared lock once again.
Unable to lock exclusively.
lock is free.
Got exception 123
Got exclusive lock.
```

## 2.4. std::scoped_lock

> std::scoped_lock 是 C++17 的内容

* `std::scoped_lock` 的工作原理与 `lock_guard` 和 `unique_lock` 一样：其构造函数会对所有互斥量进行上锁操作，并且析构函数会对互斥量进行解锁操作。`scoped_lock` 特别之处是，可以指定多个互斥量，其内部应用了避免死锁的策略。
* `scoped_lock` 使用 `std::lock` 函数，其会调用一个特殊的算法对所提供的互斥量调用 `try_lock` 函数，这是为了避免死锁。因此，在加锁与解锁的顺序相同的情况下，使用 `scoped_lock` 或对同一组锁调用 `std::lock` 都是非常安全的
* 构造函数的顺序不影响互斥量的上解锁

```cpp
// 9_5_avoid_deadlock.cpp

#include <iostream>
#include <thread>
#include <mutex>
// 避免死锁——std::scoped_lock
using namespace std;
using namespace chrono_literals;

mutex mut_a;
mutex mut_b;

// 对互斥量A进行上锁，然后对互斥量B进行上锁
static void deadlock_func_1()
{
    cout << "bad f1 acquiring mutex A..." << endl;
    lock_guard<mutex> la {mut_a};

    this_thread::sleep_for(100ms);

    cout << "bad f1 acquiring mutex B..." << endl;
    lock_guard<mutex> lb {mut_b};

    cout << "bad f1 got both mutexes" << endl;
}
// 对互斥量B进行上锁，然后对互斥量A进行上锁
static void deadlock_func_2()
{
    cout << "bad f2 acquiring mutex B..." << endl;
    lock_guard<mutex> lb {mut_b};

    this_thread::sleep_for(100ms);

    cout << "bad f2 acquiring mutex A..." << endl;
    lock_guard<mutex> la {mut_a};

    cout << "bad f2 got both mutexes" << endl;
}

static void sane_func_1()
{
    scoped_lock l {mut_a, mut_b};

    cout << "sane f1 got both mutexes" << endl;
}

static void sane_func_2()
{
    scoped_lock l {mut_b, mut_a};

    cout << "sane f2 got both mutexes" << endl;
}

int main()
{
    {
        thread t1 {sane_func_1};
        thread t2 {sane_func_2};

        t1.join();
        t2.join();
    }

    {
        thread t1 {deadlock_func_1};
        thread t2 {deadlock_func_2};

        t1.join();
        t2.join();
    }
}
```

```
sane f1 got both mutexes
sane f2 got both mutexes
bad f1 acquiring mutex A...
bad f2 acquiring mutex B...
bad f1 acquiring mutex B...
bad f2 acquiring mutex A...
```


# 3. 进行延迟初始化——std::call_once

* `std::call_once` 工作原理和栅栏类似。其能对一个函数(或是一个可调用的对象)进行访问。第一个线程达到 `call_once` 的线程会执行对应的函数。直到函数执行结束，其他线程才能不被 `call_once` 所阻塞。当第一个线程从准备函数中返回后，其他线程也就都结束了阻塞（也就是第一个线程在执行该函数时，其他线程此刻都会被阻塞）

* 每一个 `call_once` 都需要一个实例作为参数，来表明预处理函数是否运行了一次

* 如果 `call_once` 执行失败了（因为准备函数抛出了异常），那么下一个线程则会再去尝试执行（这种情况发生在下一次执行不抛出异常的时候）


【lazy Initalization】：`once_flag` 和 `call_once`：

```cpp
// 实例5：lazy Initialization
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

class LogFile {
public:
    LogFile() {
        // m_flog.open("log.txt");
    }
    ~LogFile() {
        m_flog.close();
    }
    // 将该功能修改为只在shared_print被使用时才打开文件
    void shared_print(std::string id, int value) {
        // 要确保这段代码是线程安全的，
        // 1. 新建mutex
        // 2. 用unique_lock加锁，但仍不是线程安全的

        // if(!m_flog.is_open())   // 如果文件没有被打开
        // {
        //     std::unique_lock<std::mutex> locker(m_mutexFile, std::defer_lock); // 锁定后
        //     m_flog.open("log.txt");
        // }

        //  2个线程,线程A发现文件没有被打开，则进入函数，锁住m_mutexFile，然后打开文件
        //  当线程A打开文件之前，线程B也会到这里，发现文件没被打开，就会试着去锁住m_mutexFile，这会被阻止
        // 一旦线程A打开这个文件离开if语句后，线程B就能锁住m_mutexFile，这样线程B就能打开文件，这样文件就会被2个线程打开
        // 这就是不绝对的线程安全
        // 3. 连 is_open() 函数也同步，这样线程就是安全的了

        {
            std::unique_lock<std::mutex> locker(m_mutexFile, std::defer_lock);
            if (!m_flog.is_open()) {
                std::unique_lock<std::mutex> locker(m_mutexFile, std::defer_lock);
                m_flog.open("log.txt");
            }
        }

        // 4. 但这样以来每次都要用unique_lock检测m_mutexFile，还要加锁解锁，纯粹是消耗计算机资源
        //      不再使用mutex（前面代码注释掉m_mutexFile），使用once_flag  -> 参考下个实例

        std::unique_lock<std::mutex> locker(m_mutex, std::defer_lock);
        m_flog << "From " << id << " : " << value << std::endl;
    }

private:
    std::mutex m_mutex;
    std::mutex m_mutexFile;

    std::ofstream m_flog;
};

void fun1(LogFile &log) {
    for (int i = 0; i > -100; i--)
        log.shared_print("fun1", i);
}

int main() {
    LogFile m_logFile;
    std::thread th1(fun1, std::ref(m_logFile));
    for (int i = 0; i < 100; i++)
        m_logFile.shared_print("main", i);

    th1.join();
    return 0;
}
```

`std::once_flag m_flag` 配合 `std::call_once(m_flag, [&](){f.open("log.txt")});` 确保 lamda 只能被线程调用一次。

```cpp
// 实例6：使用 once_flag
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

class LogFile {
public:
    LogFile() {
        // m_flog.open("log.txt");
    }
    ~LogFile() {
        m_flog.close();
    }
    // 将功能修改为只在shared_print被使用时才打开文件
    void shared_print(std::string id, int value) {
        // {
        //     std::unique_lock<std::mutex> locker(m_mutexFile, std::defer_lock);
        //     if (!m_flog.is_open())
        //     {
        //         std::unique_lock<std::mutex> locker(m_mutexFile, std::defer_lock);
        //         m_flog.open("log.txt");
        //     }
        // }

        // 用于确保 lambda 函数只被一个线程调用一次
        std::call_once(m_flag, [ & ]() { m_flog.open("log.txt"); });

        std::unique_lock<std::mutex> locker(m_mutex, std::defer_lock);
        locker.lock();
        m_flog << "From " << id << " : " << value << std::endl;
        locker.unlock();
    }

private:
    std::mutex m_mutex;
    // std::mutex m_mutexFile;
    std::once_flag m_flag;
    std::ofstream  m_flog;
};

void fun1(LogFile &log) {
    for (int i = 0; i > -100; i--)
        log.shared_print("fun1", i);
}

int main() {
    LogFile m_logFile;
    std::thread th1(fun1, std::ref(m_logFile));
    for (int i = 0; i < 100; i++)
        m_logFile.shared_print("main", i);

    th1.join();
    return 0;
}
```

```cpp
// 9_7_call_once.cpp

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
// 进行延迟初始化——std::call_once
using namespace std;

once_flag callflag;
mutex     cout_mutex;

// 只需执行一次
static void once_print(size_t x) {
    cout << '!';
}

static void print(size_t x) {
    std::call_once(callflag, once_print, x);
    cout << x;
}

int main() {
    vector<thread> v;

    for (size_t i{0}; i < 10; ++i) {
        v.emplace_back(print, i);
    }

    for (auto &t : v) {
        t.join();
    }

    cout << '\n';
}
```

```
!0123456789
```

# 4. 实例
## 4.1. 数据竞争和互斥对象
屏幕的 `cout` 输出比较乱，是因为线程在竞争 `cout`。

引出问题：

```cpp
// 第一种
mu.lock()
...
mu.unlock()
--> 但如果抛出异常怎么办？
--> 不推荐使用 lock() 和 unlock()

// 第二种
--> 可以使用 std::lock_guard<std::mutex> guard(mu)
--> 但是 cout 是全局变量，想要在 mu 的保护下

// 第三种
--> 可使用绑定：构建一个类，将 lock_guard 绑定到类中输出，重载输出符号
1. 要保护的变量不能暴露在类之外
2. 不能将保护的变量传递出去（return）
3. 不能将保护的变量作为参数传递出去
4. 没有任何线程能在不使用该类的情况下去使用类中的资源

// 第四种
--> 使用 unique_lock()
```

```cpp
// 实例2: 互斥量
// 解决cout输出顺序比较乱的情况
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

// 1
// 这种情况一旦cout出现异常，mu将出现无法解锁的情况
std::mutex mu;
void shared_print(std::string msg, int id) {
    mu.lock();
    std::cout << msg << "  " << id << std::endl;
    mu.unlock();
}

// 2
// 由于cout是全局变量，在这种情况下无法被mu保护
// std::mutex mu;
// void shared_print(std::string msg, int id){
//     std::lock_guard<std::mutex> guard(mu);
//     std::cout << msg  << "  " << id << std::endl;
// }

// 3
// 绑定，构建一个类，重载输出符号 -> 下一个实例

void fun1() {
    for (int i = 100; i > 0; i--)
        shared_print("From fun1: ", i);
}

int main() {
    std::thread th1(fun1);
    for (int i = 0; i < 100; i++)
        shared_print("From ***main***: ", i);

    th1.join();
    return 0;
}
```

```cpp
// 实例3：构建类，使用 std::lock_guard 对类中对象进行加锁
// 绑定，构建一个类，重载输出符号
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

class LogFile {
public:
    LogFile() {
        m_flog.open("log.txt");
    }
    ~LogFile() {
        m_flog.close();
    }
    void shared_print(std::string id, int value) {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_flog << "From " << id << " : " << value << std::endl;
    }

private:
    std::mutex    m_mutex;
    std::ofstream m_flog;
};

void fun1(LogFile &log) {
    for (int i = 100; i >= 0; i--)
        log.shared_print("fun1", i);
}

int main() {
    LogFile     logFile;
    std::thread th1(fun1, std::ref(logFile));
    for (int i = 0; i <= 100; i++)
        logFile.shared_print("main", i);

    th1.join();
    return 0;
}
// TODO:备注：还是会混乱啊
```

## 4.2. 实现互斥锁
【题目】请使用 `std::atomic<bool>` 实现一个互斥锁。

【参考答案】

```cpp
// exercises/7/7.2.mutex.cpp

#include <atomic>
#include <thread>
#include <iostream>

class mutex {
    std::atomic<bool> flag{false};

public:
    void lock()
    {
        while (flag.exchange(true, std::memory_order_relaxed));
        std::atomic_thread_fence(std::memory_order_acquire);
    }

    void unlock()
    {
        std::atomic_thread_fence(std::memory_order_release);
        flag.store(false, std::memory_order_relaxed);
    }
};

int a = 0;

int main() {

    mutex mtx_a;

    std::thread t1([&](){
        mtx_a.lock();
        a += 1;
        mtx_a.unlock();
    });
    std::thread t2([&](){
        mtx_a.lock();
        a += 2;
        mtx_a.unlock();
    });

    t1.join();
    t2.join();

    std::cout << a << std::endl;
    return 0;
}
```



## 4.3. 同步并行中使用std::cout
有互斥量保护的 `std::cout`：

```cpp
// 9_6_sync_cout.cpp

#include <iostream>
#include <thread>
#include <mutex>
#include <sstream>
#include <vector>
// 同步并行中使用std::cout
using namespace std;

struct pcout : public stringstream {
    static inline mutex cout_mutex;

    ~pcout() {
        lock_guard<mutex> l {cout_mutex};
        cout << rdbuf();    // 打印 stringstream 缓冲区中的内容
    }
};

static void print_cout(int id)
{
    cout << "cout hello from " << id << '\n';
}

static void print_pcout(int id)
{
    pcout{} << "pcout hello from " << id << '\n';
}

int main()
{
    vector<thread> v;

    for (size_t i {0}; i < 10; ++i) {
        v.emplace_back(print_cout, i);
    }

    for (auto &t : v) { t.join(); }

    cout << "=====================\n";

    v.clear();
    for (size_t i {0}; i < 10; ++i) {
        v.emplace_back(print_pcout, i);
    }

    for (auto &t : v) { 
        t.join(); 
    }
    return 0;
}
```
