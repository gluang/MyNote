#include <iostream>

class DoubleList {
private:
    struct Node {
        int   data;
        Node *prev;
        Node *next;
        explicit Node(int d)
            : data(d)
            , prev(nullptr)
            , next(nullptr) {
        }
    };

    Node  *head;
    Node  *tail;
    size_t len;

public:
    DoubleList()
        : head(new Node(0))
        , tail(head)
        , len(0) {
    }
    ~DoubleList() {
        clear();
    }

    void clear() {
        Node *temp;
        Node *node = head->next;
        while (node != nullptr) {
            temp = node;
            node = node->next;
            delete temp;
        }
        delete head;
        head = nullptr;
    }

    void push_front(int data) {
        Node *node       = new Node(data);
        node->next       = head->next;
        node->prev       = head;
        head->next       = node;
        node->next->prev = node;
        len++;
    }
    void push_back(int data) {
        Node *node = new Node(data);
        node->next = nullptr;
        node->prev = tail;

        tail->next = node;
        tail       = node;
        len++;
    }
    // position 从1开始
    void insert(int data, int position) {
        Node *p = head->next;
        if (position > len || position <= 0) {
            std::cout << "over stack!" << std::endl;
            return;
        }
        for (int i = 0; i < position - 1; ++i) {
            p = p->next;
        }
        Node *new_node = new Node(data);
        new_node->next = p;
        new_node->prev = p->prev;
        p->prev->next  = new_node;
        p->prev        = new_node;
        len++;
    }

    void pop_front() {
        Node *temp       = head->next;
        head->next       = temp->next;
        temp->next->prev = temp->prev;
        delete temp;
        len--;
    }
    void pop_back() {
        Node *temp = tail;
        tail       = temp->prev;
        tail->next = nullptr;
        delete temp;
        len--;
    }
    void erase(int position) {
        Node *p = head->next;
        if (position > len || position <= 0) {
            std::cout << "over stack!" << std::endl;
            return;
        }
        for (int i = 0; i < position - 1; ++i) {
            p = p->next;
        }
        p->prev->next = p->next;
        p->next->prev = p->prev;
        delete p;
        len--;
    }

    void forward_traversal() const {
        Node *temp = head->next;
        while (temp != nullptr) {
            std::cout << temp->data << " ";
            temp = temp->next;
        }
        std::cout << std::endl;
    }
    void reverse_traversal() const {
        Node *temp = tail;
        while (temp->prev != nullptr) {
            std::cout << temp->data << " ";
            temp = temp->prev;
        }
        std::cout << std::endl;
    }

    void bubble_sort() {
        int   temp;
        Node *q;
        for (Node *p = head->next; p->next != nullptr; p = p->next) {
            for (q = p->next; q != nullptr; q = q->next) {
                if (q->data < p->data) {
                    temp    = q->data;
                    q->data = p->data;
                    p->data = temp;
                }
            }
        }
        // 重新查找 tail 的位置
        Node *point = head;
        while (point->next) {
            point = point->next;
        }
        tail = point;
    }

    void insert_sort() {
        if (head->next == nullptr || head->next->next == nullptr) {
            return;
        }
        Node *p1         = head;
        Node *p2         = head->next->next;
        head->next->next = nullptr;
        while (p2 != nullptr) {
            Node *p2_next = p2->next;
            while (p1->next != nullptr) {
                // 小的插在前面
                if (p2->data < p1->next->data) {
                    p2->next       = p1->next;
                    p2->prev       = p1;
                    p1->next->prev = p2;
                    p1->next       = p2;
                    break;
                }
                p1 = p1->next;
            }
            if (p1->next == nullptr) {
                // 如果没找到小值，则就是大值，插在后面
                p2->next = nullptr;
                // 此时p1已经在尾部了
                p2->prev = p1;
                p1->next = p2;
            }
            p2 = p2_next;
        }

        // 重新查找 tail 的位置
        Node *point = head;
        while (point->next) {
            point = point->next;
        }
        tail = point;
    }

    void update(int data, int position) {
        Node *p = head->next;
        if (position > len || position <= 0) {
            std::cout << "over stack!" << std::endl;
            return;
        }
        for (int i = 0; i < position; ++i) {
            p = p->next;
        }
        p->data = data;
    }
};

int main() {
    DoubleList dl;

    dl.push_back(35);
    dl.push_back(30);
    dl.push_back(32);
    dl.push_front(4);
    dl.push_front(5);
    dl.pop_back();
    dl.pop_front();
    dl.forward_traversal();

    std::cout << "排序" << std::endl;
    dl.bubble_sort(); //冒泡排序
//  dl.insert_sort(); //插入排序
    dl.forward_traversal();
    // 4  30  35

    std::cout << "插入：" << std::endl;
    dl.insert(55, 1);
    dl.forward_traversal();
    // 55  4  30  35

    std::cout << "删除：" << std::endl;
    dl.erase(1);
    dl.forward_traversal();
    // 4  30  35

    std::cout << "反向遍历" << std::endl;
    dl.reverse_traversal();
    // 35  30  4

    dl.update(12, 1);
    dl.forward_traversal();
    // 4  12  35

    return 0;
}