<h1 align="center">目录</h1>

* [1. condition_variable](#1-condition_variable)
* [2. 单生产者和单消费者模型](#2-单生产者和单消费者模型)
    * [2.1. 实例 1](#21-实例-1)
    * [2.2. 实例 2](#22-实例-2)
    * [2.3. 实例 3](#23-实例-3)
* [3. 实现多生产者/多消费者模型](#3-实现多生产者多消费者模型)

---

# 1. condition_variable
条件变量 `std::condition_variable` 是为了**解决死锁**而生，当互斥操作不够用而引入的。

【场景】比如，线程可能需要等待某个条件为真才能继续执行，而一个忙等待循环中可能会导致所有其他线程都无法进入临界区使得条件为真时，就会发生死锁。所以，`condition_variable` 实例被创建出现主要就是用于唤醒等待线程从而避免死锁。

* 线程1：

```cpp
std::conditional_variable cond;
cond.notify_one(); 
// 激活一个等待这个条件的线程
```

* 线程2：

```cpp
cond.wait(locker) 
// 这条语句会将线程2休眠，直到线程1调用 notify_one() 才会把它激活
// 在 wait 中会解锁互斥量 mu，因为一个线程不会在加锁的情况下休眠
```

【注意】：使用条件变量，**只能用 `unique_lock`**，而不能用 `lock_guard`，不然会报错，原因如下：

```cpp
std::unique_lock<std::mutex> lock(mu);
cond.wait(locker)
/* 先解锁 mu，然后将其休眠，休眠后又会重新加锁
 * 因为传入的互斥量需要重复加解锁，所以只能用 unique_lock
 * 不能使用 lock_guard，因为 lock_guard不能满足重复加解锁
 * /
```

在条件过程中可能会被自己的函数伪激活，如果不希望其自身伪激活，可添加第二个参数 lamda 进行判断。

```cpp
cond.wait(locker, [](){return !q.empty();});
```

`std::condition_variable`的 `notify_one()` 用于唤醒一个线程；`notify_all()` 则是通知所有线程。


# 2. 单生产者和单消费者模型
## 2.1. 实例 1
下面是一个**生产者**和**消费者**模型的例子：

```cpp
// 7.5.producer.consumer.cpp

#include <queue>
#include <chrono>
#include <mutex>
#include <thread>
#include <iostream>
#include <condition_variable>


int main() {
    std::queue<int> produced_nums;
    std::mutex mtx;
    std::condition_variable cv;
    bool notified = false;  // 通知信号

    // 生产者
    auto producer = [&]() {
        for (int i = 0; ; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(900));
            std::unique_lock<std::mutex> lock(mtx);
            std::cout << "producing " << i << std::endl;
            produced_nums.push(i);
            notified = true;
            cv.notify_all(); // 此处也可以使用 notify_one
        }
    };

    // 消费者
    auto consumer = [&]() {
        //备注：一直处于循环中等待处理数据
        while (true) {
            std::unique_lock<std::mutex> lock(mtx);
            while (!notified) {  // 避免虚假唤醒
                cv.wait(lock);
            }
            // 短暂取消锁，使得生产者有机会在消费者消费空前继续生产
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // 消费者慢于生产者
            lock.lock();
            while (!produced_nums.empty()) {
                std::cout << "consuming " << produced_nums.front() << std::endl;
                produced_nums.pop();
            }
            notified = false;
        }
    };

    // 分别在不同的线程中运行
    std::thread p(producer);
    std::thread cs[2];
    for (int i = 0; i < 2; ++i) {
        cs[i] = std::thread(consumer);
    }
    p.join();
    for (int i = 0; i < 2; ++i) {
        cs[i].join();
    }
    return 0;
}
```

值得一提的是，在生产者中我们虽然可以使用 `notify_one()`，但实际上并不建议在此处使用，因为在多消费者的情况下，我们的消费者实现中简单放弃了锁的持有，这使得可能让其他消费者争夺此锁，从而更好的利用多个消费者之间的并发。

话虽如此，但实际上因为 `std::mutex` 的排他性，我们根本无法期待多个消费者能真正意义上的并行消费队列的中生产的内容，我们仍需要粒度更细的手段。

## 2.2. 实例 2
生产者：生产数据 --> 条件变量  --> 消费者：处理数据

```cpp
// 实例7：条件变量
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <fstream>
#include <deque>
#include <functional>
#include <condition_variable>

std::deque<int> q;
std::mutex mu;
std::condition_variable cond;

// 数据的生成者
// q 是共享内存，因此要同步
void fun1()
{
	int count = 10;
	while (count > 0)
	{
		std::unique_lock<std::mutex> locker(mu);
		q.push_front(count);
		locker.unlock();
		cond.notify_one();    // 激活一个等待这个条件的线程
		// cond.notify_all(); // 激活所有等待线程
		std::this_thread::sleep_for(std::chrono::seconds(1));
		count--;
	}
}

// 数据的消费者
void fun2()
{
	int data = 0;

	// // 引入条件变量后，
	// while (data != 1)
	// {
	//     std::unique_lock<std::mutex> locker(mu);
	//     if (!q.empty())
	//     {
	//         data = q.back();
	//         q.pop_back();
	//         locker.unlock();
	//         std::cout << "t2 got a value from t1:" << data << std::endl;
	//     }
	//     // 如果为空，则解锁
	//     // 但是每次都要这样判断，效率比较低
	//     else
	//     {
	//         locker.unlock();
	//     }
	// }
	while (data != 1)
	{
		std::unique_lock<std::mutex> locker(mu);
		// 将此线程休眠，等待另外一个线程调用notify_one，才能激活此线程
		// 将在wait中解锁互斥量mu，因为一个线程不会在加锁的情况下进入休眠
		// wait中：解锁，休眠，加锁
		// cond.wait(locker);
		// 如果不希望其自身伪激活，可添加第二个参数
		cond.wait(locker, []() { return !q.empty(); });
		data = q.back();
		q.pop_back();
		locker.unlock();
		std::cout << "t2 got a value from t1:" << data << std::endl;
	}
}

int main()
{
	std::thread t1(fun1);
	std::thread t2(fun2);

	t1.join();
	t2.join();
	return 0;
}
```

```
t2 got a value from t1:10
t2 got a value from t1:9
t2 got a value from t1:8
t2 got a value from t1:7
t2 got a value from t1:6
t2 got a value from t1:5
t2 got a value from t1:4
t2 got a value from t1:3
t2 got a value from t1:2
t2 got a value from t1:1
```


## 2.3. 实例 3
本节使用多线程实现一个经典的生产者/消费者模型。其过程就是一个生产者线程将商品放到队列中，然后另一个消费者线程对这个商品进行消费。如果不需要生产，生产者线程休眠。如果队列中没有商品能够消费，消费者休眠。

```cpp
// 9_9_producer_consumer.cpp

#include <iostream>
#include <queue>
#include <tuple>
#include <condition_variable>
#include <thread>
// 实现生产者/消费者模型——std::condition_variable
using namespace std;
using namespace chrono_literals;


queue<size_t>      q;
mutex              mut;
condition_variable cv;
bool               finished {false};

static void producer(size_t items) {
	for(size_t i {0}; i < items; ++i) {
        this_thread::sleep_for(100ms);
		{
			lock_guard<mutex> lk {mut};
			q.push(i);
		}
		cv.notify_all();	// 唤醒所有消费者线程
	}

	{
		lock_guard<mutex> lk {mut};
		finished = true;
	}
	cv.notify_all();
}

static void consumer() {
	while (!finished) {
		unique_lock<mutex> l {mut};
		// cv.wait 的调用会对锁进行解锁，并且会等到给予的条件达成时才会继续运行
		// 然后，其会再次对互斥量上锁，并对队列上的商品进行消费，直到队列为空
		// 不为空时，或者尚未完成，则等待通知，也就是判断条件为 true 时，结束等待
		// 阻塞循环
		cv.wait(l, [] { return !q.empty() || finished; });

		while (!q.empty()) {
			cout << "Got " << q.front() << " from queue.\n";
			q.pop();
		}
	}
}

int main() {

	thread t1 {producer, 10};
	thread t2 {consumer};
	t1.join();
	t2.join();
	cout << "finished!\n";
}
```

```cpp
// 每一行差不多隔100毫秒打印出来，因为生产者需要时间进行生产
Got 0 from queue.
Got 1 from queue.
Got 2 from queue.
Got 3 from queue.
Got 4 from queue.
Got 5 from queue.
Got 6 from queue.
Got 7 from queue.
Got 8 from queue.
Got 9 from queue.
finished!
```


# 3. 实现多生产者/多消费者模型
当队列中没有商品时，消费者会处于等待状态，而当队列中没有空间可以盛放商品时，生产者也会处于等待状态，需要生产者持续生产，以保证队列上一直有可消费的商品存在。如果商品囤积到一定程度，则生产者休息。

【TODO:】`static inline mutex cout_mutex;`　为什么要使用静态内联类，有什么用途

```cpp
// 9_10_multi_producer_consumer.cpp

#include <chrono>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <vector>
// 实现多生产者/多消费者模型——std::condition_variable
using namespace std;
using namespace chrono_literals;

struct pcout : public stringstream {
    static inline mutex cout_mutex;

    ~pcout() {
        lock_guard<mutex> l{cout_mutex};
        cout << rdbuf();
    }
};

mutex         q_mutex;
bool          production_stopped{false};
queue<size_t> q;

condition_variable go_produce; // 用来提醒生产者进行生产
condition_variable go_consume; // 用来提醒消费者消费

static void producer(size_t id, size_t items, size_t stock) {
    for (size_t i = 0; i < items; ++i) {
        unique_lock<mutex> lock(q_mutex);
        // 当队列上的商品数量未达到囤积阈值时，满足等待条件
        go_produce.wait(lock, [ & ] { return q.size() < stock; });

        q.push(id * 100 + i);

        pcout{} << "   Producer " << id << " --> item " << setw(3) << q.back() << '\n';

        go_consume.notify_all();
        this_thread::sleep_for(90ms); // 模拟生产者生产商品的时间
    }

    pcout{} << "EXIT: Producer " << id << '\n';
}

static void consumer(size_t id) {
    // 当生产者停止生产，或是队列上没有商品，消费者就会继续等待
    while (!production_stopped || !q.empty()) {
        unique_lock<mutex> lock(q_mutex);

        // 当队列中有商品的时候我们就会对其进行获取。
        // 1s 说明，我们并不想等太久。如果等待时间超过1秒，我们将不会等待
        // 当谓词条件达成，则 wait_for 函数返回；否则就会因为超时而放弃等待
        if (go_consume.wait_for(lock, 1s, [] { return !q.empty(); })) {
            pcout{} << "                  item " << setw(3) << q.front() << " --> Consumer " << id << '\n';
            q.pop();
            go_produce.notify_all();
            this_thread::sleep_for(130ms); // 模拟消费时间
        }
    }

    pcout{} << "EXIT: Consumer " << id << '\n';
}

int main() {
    vector<thread> workers;
    vector<thread> consumers;

    // 3 个生产者和 5 个消费者
    for (size_t i = 0; i < 3; ++i) {
        workers.emplace_back(producer, i, 15, 5);
    }

    for (size_t i = 0; i < 5; ++i) {
        consumers.emplace_back(consumer, i);
    }

    for (auto &t : workers) {
        t.join();
    } // 先让生产者线程终止

    production_stopped = true; // 让消费者线程同时停止
    for (auto &t : consumers) {
        t.join();
    }
    return 0;
}
```