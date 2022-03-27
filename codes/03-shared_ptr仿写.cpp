#include <iostream>
#include <memory>

template <typename T> class my_shared_ptr {
private:
    T      *_ptr;
    size_t *_count;

public:
    explicit my_shared_ptr(T *ptr = nullptr)
        : _ptr(ptr) {
        if (_ptr)
            _count = new size_t(1);
        else
            _count = new size_t(0);
    }
    my_shared_ptr(const my_shared_ptr &other) {
        if (this != &other) {
            this->_ptr   = other._ptr;
            this->_count = other._count;
            (*this->_count)++;
        }
    }
    my_shared_ptr &operator=(const my_shared_ptr &other) {
        if (this->_ptr == other._ptr)
            return *this;
        if (this->_ptr) {
            (*this->_count)--;
            if (*this->_count == 0) {
                delete this->_ptr;
                delete this->_count;
            }
        }
        this->_ptr   = other._ptr;
        this->_count = other._count;
        (*this->_count)++;
        return *this;
    }
    ~my_shared_ptr() {
        (*this->_count)--;
        if (*this->_count == 0) {
            delete this->_ptr;
            delete this->_count;
        }
    }
    T &operator*() {
        assert(this->_ptr == nullptr);
        return *(this->_ptr);
    }
    T *operator->() {
        assert(this->_ptr == nullptr);
        return this->_ptr;
    }
    size_t use_count() const {
        return *this->_count;
    }
};

int main() {
    {
        my_shared_ptr<int> sp1(new int(10));
        my_shared_ptr<int> sp2 = sp1;
        my_shared_ptr<int> sp3(new int(20));
        sp2 = sp3;
        std::cout << sp1.use_count() << std::endl; // 1
        std::cout << sp3.use_count() << std::endl; // 2
    }
    {
        std::shared_ptr<int> sp1(new int(10));
        std::shared_ptr<int> sp2 = sp1;
        std::shared_ptr<int> sp3(new int(20));
        sp2 = sp3;
        std::cout << sp1.use_count() << std::endl; // 1
        std::cout << sp3.use_count() << std::endl; // 2
    }
    return 0;
}