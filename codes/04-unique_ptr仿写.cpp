#include <cassert>
#include <iostream>

template <typename T> class my_unique_ptr {
private:
    T   *_ptr;
    void del() {
        if (*this) {
            if (_ptr) {
                delete _ptr;
            }
            _ptr = nullptr;
        }
    }

public:
    my_unique_ptr(T *ptr = nullptr)
        : _ptr(ptr) {
    }
    ~my_unique_ptr() {
        del();
    }
    my_unique_ptr(const my_unique_ptr &) = delete;
    my_unique_ptr &operator=(const my_unique_ptr &) = delete;

    void reset(T *ptr) {
        del();
        _ptr = ptr;
    }
    T *release() {
        T *ptr = _ptr;
        _ptr   = nullptr;
        return ptr;
    }
    T *get() {
        return _ptr;
    }
    operator bool() const {
        return _ptr != nullptr;
    }
    T &operator*() {
        assert(*this);
        return *_ptr;
    }
    T *operator->() {
        return _ptr;
    }
};
struct CTest {
    int val;
    CTest()
        : val(0) {
        std::cout << "new object this =" << this << std::endl;
    }
    ~CTest() {
        std::cout << "delete object this =" << this << std::endl;
    }
};

int main() {
    my_unique_ptr<CTest> uptr(new CTest);
    assert(uptr);
    uptr->val = 100;
    std::cout << "val = " << (*uptr).val << std::endl;

    CTest *p = uptr.release();
    std::cout << "p =" << p << std::endl;
    if (p) {
        delete p;
        p = nullptr;
    }

    return 0;
}

// new object this =0x55703df8ceb0
// val = 100
// p =0x55703df8ceb0
// delete object this =0x55703df8ceb0