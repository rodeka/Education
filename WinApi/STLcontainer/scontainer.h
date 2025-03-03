#ifndef SCONTAINER_H
#define SCONTAINER_H

#include <iterator>
#include <functional>
#include <cstddef>
#include <cassert>

template<typename T, typename Compare = std::less<T> >
class scontainer {
private:
    // структура узла двусвязного списка
    struct Node {
        T data;
        Node* next;
        Node* prev;
        Node(const T& value) : data(value), next(nullptr), prev(nullptr) {}
    };

    Node* _head;       // указатель на первый элемент
    Node* _tail;       // указатель на последний элемент
    std::size_t _size; // количество элементов
    Compare comp;      // функция сравнения

public:
    // определения типов, как в стандарте STL
    typedef T value_type;
    typedef Compare value_compare;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    // итератор
    class iterator {
    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef T value_type;
        typedef T& reference;
        typedef T* pointer;
        typedef std::ptrdiff_t difference_type;
    private:
        Node* node;
        friend class scontainer<T, Compare>;
    public:
        iterator(Node* n = nullptr) : node(n) {}

        reference operator*() const { return node->data; }
        pointer operator->() const { return &(node->data); }

        // префиксный инкремент
        iterator& operator++() {
            if (node) node = node->next;
            return *this;
        }
        // постфиксный инкремент
        iterator operator++(int) {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }
        // префиксный декремент
        iterator& operator--() {
            if (node) node = node->prev;
            return *this;
        }
        // постфиксный декремент
        iterator operator--(int) {
            iterator tmp(*this);
            --(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const { return node == other.node; }
        bool operator!=(const iterator& other) const { return node != other.node; }
    };

    // константный итератор
    class const_iterator {
    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef T value_type;
        typedef const T& reference;
        typedef const T* pointer;
        typedef std::ptrdiff_t difference_type;
    private:
        const Node* node;
        friend class scontainer<T, Compare>;
    public:
        const_iterator(const Node* n = nullptr) : node(n) {}
        const_iterator(const iterator& it) : node(it.node) {}

        reference operator*() const { return node->data; }
        pointer operator->() const { return &(node->data); }

        const_iterator& operator++() {
            if (node) node = node->next;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp(*this);
            ++(*this);
            return tmp;
        }
        const_iterator& operator--() {
            if (node) node = node->prev;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator tmp(*this);
            --(*this);
            return tmp;
        }

        bool operator==(const const_iterator& other) const { return node == other.node; }
        bool operator!=(const const_iterator& other) const { return node != other.node; }
    };

    // реверсивный итератор
    class reverse_iterator {
    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef T value_type;
        typedef T& reference;
        typedef T* pointer;
        typedef std::ptrdiff_t difference_type;
    private:
        Node* node;
        friend class scontainer<T, Compare>;
    public:
        reverse_iterator(Node* n = nullptr) : node(n) {}

        reference operator*() const { return node->data; }
        pointer operator->() const { return &(node->data); }

        // при движении вперёд реверсивного итератора перемещаемся к предыдущему элементу
        reverse_iterator& operator++() {
            if (node) node = node->prev;
            return *this;
        }
        reverse_iterator operator++(int) {
            reverse_iterator tmp(*this);
            ++(*this);
            return tmp;
        }
        // при движении назад реверсивного итератора перемещаемся к следующему элементу
        reverse_iterator& operator--() {
            if (node) node = node->next;
            return *this;
        }
        reverse_iterator operator--(int) {
            reverse_iterator tmp(*this);
            --(*this);
            return tmp;
        }

        bool operator==(const reverse_iterator& other) const { return node == other.node; }
        bool operator!=(const reverse_iterator& other) const { return node != other.node; }
    };

    // константный реверсивный итератор
    class const_reverse_iterator {
    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef T value_type;
        typedef const T& reference;
        typedef const T* pointer;
        typedef std::ptrdiff_t difference_type;
    private:
        const Node* node;
        friend class scontainer<T, Compare>;
    public:
        const_reverse_iterator(const Node* n = nullptr) : node(n) {}
        const_reverse_iterator(const reverse_iterator& it) : node(it.node) {}

        reference operator*() const { return node->data; }
        pointer operator->() const { return &(node->data); }

        const_reverse_iterator& operator++() {
            if (node) node = node->prev;
            return *this;
        }
        const_reverse_iterator operator++(int) {
            const_reverse_iterator tmp(*this);
            ++(*this);
            return tmp;
        }
        const_reverse_iterator& operator--() {
            if (node) node = node->next;
            return *this;
        }
        const_reverse_iterator operator--(int) {
            const_reverse_iterator tmp(*this);
            --(*this);
            return tmp;
        }

        bool operator==(const const_reverse_iterator& other) const { return node == other.node; }
        bool operator!=(const const_reverse_iterator& other) const { return node != other.node; }
    };

    // конструктор по умолчанию
    scontainer() : _head(nullptr), _tail(nullptr), _size(0), comp(Compare()) {}

    // конструктор копирования
    scontainer(const scontainer& other) : _head(nullptr), _tail(nullptr), _size(0), comp(other.comp) {
        for (const T& value : other) {
            insert(value);
        }
    }

    // оператор присваивания
    scontainer& operator=(const scontainer& other) {
        if (this != &other) {
            clear();
            comp = other.comp;
            for (const T& value : other) {
                insert(value);
            }
        }
        return *this;
    }

    // деструктор
    ~scontainer() {
        clear();
    }

    // возвращает количество элементов
    size_type size() const { return _size; }
    bool empty() const { return _size == 0; }

    // доступ к первому элементу
    T& front() {
        assert(_head != nullptr);
        return _head->data;
    }
    const T& front() const {
        assert(_head != nullptr);
        return _head->data;
    }

    // доступ к последнему элементу
    T& back() {
        assert(_tail != nullptr);
        return _tail->data;
    }
    const T& back() const {
        assert(_tail != nullptr);
        return _tail->data;
    }

    // итераторы
    iterator begin() { return iterator(_head); }
    const_iterator begin() const { return const_iterator(_head); }
    const_iterator cbegin() const { return const_iterator(_head); }

    iterator end() { return iterator(nullptr); }
    const_iterator end() const { return const_iterator(nullptr); }
    const_iterator cend() const { return const_iterator(nullptr); }

    reverse_iterator rbegin() { return reverse_iterator(_tail); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(_tail); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(_tail); }

    reverse_iterator rend() { return reverse_iterator(nullptr); }
    const_reverse_iterator rend() const { return const_reverse_iterator(nullptr); }
    const_reverse_iterator crend() const { return const_reverse_iterator(nullptr); }

    // удаляет все элементы контейнера
    void clear() {
        Node* current = _head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        _head = _tail = nullptr;
        _size = 0;
    }

    // вставка элемента с сохранением сортировки.
    iterator insert(const T& value) {
        Node* newNode = new Node(value);
        // Если список пуст, вставка в начало
        if (!_head) {
            _head = _tail = newNode;
            _size++;
            return iterator(newNode);
        }
        // поиск места для вставки: первый узел, для которого comp(value, current->data) == true
        Node* current = _head;
        while (current && !comp(value, current->data)) {
            current = current->next;
        }
        if (!current) { // вставка в конец
            newNode->prev = _tail;
            _tail->next = newNode;
            _tail = newNode;
        } else {
            // вставка перед current
            newNode->next = current;
            newNode->prev = current->prev;
            if (current->prev)
                current->prev->next = newNode;
            else
                _head = newNode;
            current->prev = newNode;
        }
        _size++;
        return iterator(newNode);
    }

    // удаляет элемент по позиции, возвращает итератор на следующий элемент
    iterator erase(iterator pos) {
        Node* current = pos.node;
        if (!current) return end();
        Node* next = current->next;
        if (current->prev)
            current->prev->next = current->next;
        else
            _head = current->next;
        if (current->next)
            current->next->prev = current->prev;
        else
            _tail = current->prev;
        delete current;
        _size--;
        return iterator(next);
    }
};

#endif // SCONTAINER_H
