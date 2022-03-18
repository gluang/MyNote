<h1 align="center">目录</h1>

* [1. future](#1-future)
* [2. packaged_task](#2-packaged_task)
* [3. promise](#3-promise)
* [4. shared_future](#4-shared_future)

---

# 1. future

> `#include <future>`

`std::future`，它提供了一个访问异步操作结果的途径。

【使用场景】如果我们的主线程 A 希望新开辟一个线程 B 去执行某个我们预期的任务，并返回我一个结果。而这时候，线程 A 可能正在忙其他的事情，无暇顾及 B 的结果，所以我们会很自然的希望能够在某个特定的时间获得线程 B 的结果。

【C++11 引入前】在 C++11 的 `std::future` 被引入之前，通常的做法是：创建一个线程 A，在线程 A 里启动任务 B，当准备完毕后发送一个事件，并将结果保存在全局变量中。而主函数线程 A 里正在做其他的事情，当需要结果的时候，调用一个线程等待函数来获得执行的结果。

【C++11】而 C++11 提供的 `std::future` 简化了这个流程，可以用来获取异步任务的结果。自然地，我们很容易能够想象到把它作为一种简单的线程同步手段，即屏障（barrier）。

为了看一个例子，我们这里额外使用 `std::packaged_task`，它**可以用来封装任何可以调用的目标**，从而用于实现异步的调用。

举例来说：

```cpp
// 7.4.futures.cpp

#include <iostream>
#include <future>
#include <thread>

int main() {
    // 将一个返回值为7的 lambda 表达式封装到 task 中
    // std::packaged_task 的模板参数为要封装函数的类型
    std::packaged_task<int()> task([](){return 7;});
    // 获得 task 的期物
    std::future<int> result = task.get_future();
    // 在一个线程中执行 task
    std::thread(std::move(task)).detach();
    std::cout << "waiting...";
    result.wait(); // 在此设置屏障，阻塞到期物的完成
    // 输出执行结果
    std::cout << "done!" << std:: endl << "future result is " << result.get() << std::endl;
    return 0;
}
```

在封装好要调用的目标后，可以使用 `get_future()` 来获得一个 `std::future` 对象，以便之后实施线程同步。


# 2. packaged_task
> `#include <future>`

`packaged_task` 包含一个可调用对象，并且允许异步获取该调用对象的结果，通过 `future` 对象来达到异步访问共享状态的效果。通过 `promise`、`future` 的形式简化解决数据同步问题。

对象内部包含 2 个基本元素：

* 被包装的任务：一个可调用对象 --> 比如：函数指针、成员函数指针、函数对象
* 共享状态：用于保存任务返回值


【与 `thread` 的区别】：

* `packaged_task` 的创建方式与 `thread` 不同，只能传递一个参数到其构造函数中，如果要传递多个参数，可使用 `std::bind`
* 为什么需要 `packaged_task`：它可连接一个可调用对象，关联一个 `future`，通过 `future` 来异步获取可调用对象的结果

```cpp
// 实例12：packaged_task

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <future>
#include <functional>
#include <deque>

int factorial(int N) {
	int res = 1;

	for (int i = N; i > 1; i--)
		res *= i;

	std::cout << "Result is: " << res << std::endl;
	return res;
}

std::deque<std::packaged_task<int()>> task_q;
std::mutex mu;
std::condition_variable cond;

// 由于队列是在主线程和子线程是共享的，所以就涉及到数据竞争的问题
// 需要mutex同步
void thread_1() {
	std::packaged_task<int()> t;
	{
		std::unique_lock<std::mutex> locker(mu);
		cond.wait(locker, [] { return !task_q.empty(); });
		t = std::move(task_q.front());
		// 由于这里的front可能在主线程的push_back之前调用
		// 必须确保front函数在push_back之后调用，需要条件变量
	}

	t();
}

int main() {
	std::thread t1(thread_1);
	// TODO: 这里的<int()>是怎么回事
	// 应该是用于表示函数：返回值为 int 的函数
	std::packaged_task<int()> t(std::bind(factorial, 6));

	// 获得与 packaged_task 共享状态相关联的 future 对象
	std::future<int> ret = t.get_future();

	// 解决了task_q的数据竞争问题
	{
		std::unique_lock<std::mutex> locker(mu);
		task_q.push_back(std::move(t)); //由于t在主线程中不再使用，直接将其move到子线程
	}
	cond.notify_one();

	// push进队列中，然后线程t1就会弹出push进去的对象并调用它
	// 等待任务完成并获取结果
	int value = ret.get(); // 获得阶乘执行结果

	t1.join();

	return 0;
}
/* 输出结果：
Result is: 720
*/

// 只需要通过函数对象就可完成packaged_task的所有事情
int main()
{
	auto t = std::bind(factorial, 6);
	t();
	return 0;
}
// packeaged_task 主要功能在于：它可连接一个可调用对象，关联一个future
//通过future来异步获取可调用对象的执行结果

/* 输出结果
Result is: 720
*/
```


# 3. promise

从主线程中设置变量放置到子线程中。

```cpp
std::promise<int> p;
std::promise<int> fu = p.get_future();

// factorial 函数的形参为：std::future<int>& f

std::promise<int> ft = std::async(std::launch::async, factorial, std::ref(fu));
p.set_value(4);
//❗模板变量的 int 对应 set_value 设置的值 
```

【`future` 和 `promise` 区别】：

* 二者与一般的获得的区别：可设置成一个线程来执行这个步骤而不是单一线程
* `future` ：从子线程中获得返回值 --> 到 --> 主线程（`future` 所在线程）中，可不设计 `promise` 部分
* `promise` ：从主线程中设置变量 --> 放置到 --> 子线程中，常包含 `future` 部分
* `future` 和 `promise` 都不能 copy，只能 move


```cpp
// 实例9：Promise ：从主线程中设置变量放置到子线程中
#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <future>

// 需要修改传入的参数为 std::future<int>&
int factorial(std::future<int>& fu) {
	int res = 1;

	int N = fu.get();   // 获取主线程设置的变量
	// 如果不在主线程中设置该变量值，就会抛出一个 std::future_errc::broken_promise 的异常
	
	for (int i = N; i > 1; i--)
		res *= i;

	std::cout << "Result is: " << res << std::endl;

	std::cout << "factorial id: " << std::this_thread::get_id() << std::endl;
	return res;
}

int main() {
	int x;

	std::promise<int> p;
	std::future<int> fu = p.get_future();
	std::future<int> ft = std::async(std::launch::async, factorial, std::ref(fu)); 
	p.set_value(4); // 设置子线程的变量
	// set_value(int&& _Val)，是右值引用
	x = ft.get();   // 获取子线程返回的值

	std::cout << "Get From child x: " << x << std::endl;    
	std::cout << "main id: " << std::this_thread::get_id() << std::endl;

	return 0;
}
```

```
Result is: 24
factorial id: 140444588476160
Get From child x: 24
main id: 140444606732096
```

# 4. shared_future
* `std::shared_future<int> sf = fu.share();`

	由于要使用 `future` 的 `share` ，所以需先创建一个 `future` 才能用其 `share()` 创建的 `shared_future`。

* 不同于 `future`，`shared_future` 可被拷贝，所以可以直接传递值，可用于使用一个 `promise` ，创建多个 `future`

```cpp
// 实例10：shared_future ：可被复制，可用于一个 promise 创建多个 future
#include <iostream>
#include <string>
#include <thread>
#include <fstream>
#include <future>

// 需要修改传入的参数为
int factorial(std::shared_future<int> fu) {
	int res = 1;

	int N = fu.get();   // 获取主线程设置的变量
	// 如果不在主线程中设置该变量值，就会抛出一个 std::future_errc::broken_promise 的异常
	
	for (int i = N; i > 1; i--)
		res *= i;

	std::cout << "Result is: " << res << std::endl;

	std::cout << "factorial id: " << std::this_thread::get_id() << std::endl;
	return res;
}

int main() {

	std::promise<int> p;
	std::future<int> fu = p.get_future();
	std::shared_future<int> sf = fu.share();
	std::future<int> ft = std::async(std::launch::async, factorial, sf); 
	std::future<int> ft2 = std::async(std::launch::async, factorial, sf); 
	std::future<int> ft3 = std::async(std::launch::async, factorial, sf); 
	std::future<int> ft4 = std::async(std::launch::async, factorial, sf); 
	
	p.set_value(4); // 设置子线程的变量

	
	std::cout << "main id: " << std::this_thread::get_id() << std::endl;

	return 0;
}
```

```
Result is: Result is: 24
factorial id: 140671460030208
main id: 140671486678848
24
factorial id: 140671468422912
Result is: 24
factorial id: 140671443244800
Result is: 24
factorial id: 140671451637504
```
