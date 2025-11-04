#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
    struct Node {
        T data;
        Node *left, *right;
        int dist;

        Node(const T &value) : data(value), left(nullptr), right(nullptr), dist(0) {}
    };

    Node *root;
    size_t queue_size;
    Compare cmp;

    int getDist(Node *node) const {
        return node ? node->dist : -1;
    }

    Node* mergeNodes(Node *a, Node *b) {
        if (!a) return b;
        if (!b) return a;

        try {
            if (cmp(a->data, b->data)) {
                std::swap(a, b);
            }
        } catch (...) {
            throw;
        }

        a->right = mergeNodes(a->right, b);

        if (getDist(a->left) < getDist(a->right)) {
            std::swap(a->left, a->right);
        }

        a->dist = getDist(a->right) + 1;
        return a;
    }

    void clear(Node *node) {
        if (node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

    Node* copyTree(Node *other) {
        if (!other) return nullptr;
        Node *node = new Node(other->data);
        node->left = copyTree(other->left);
        node->right = copyTree(other->right);
        node->dist = other->dist;
        return node;
    }

public:
    /**
     * @brief default constructor
     */
    priority_queue() : root(nullptr), queue_size(0) {}

    /**
     * @brief copy constructor
     * @param other the priority_queue to be copied
     */
    priority_queue(const priority_queue &other) : root(nullptr), queue_size(0) {
        root = copyTree(other.root);
        queue_size = other.queue_size;
    }

    /**
     * @brief deconstructor
     */
    ~priority_queue() {
        clear(root);
    }

    /**
     * @brief Assignment operator
     * @param other the priority_queue to be assigned from
     * @return a reference to this priority_queue after assignment
     */
    priority_queue &operator=(const priority_queue &other) {
        if (this != &other) {
            clear(root);
            root = copyTree(other.root);
            queue_size = other.queue_size;
        }
        return *this;
    }

    /**
     * @brief get the top element of the priority queue.
     * @return a reference of the top element.
     * @throws container_is_empty if empty() returns true
     */
    const T & top() const {
        if (empty()) {
            throw container_is_empty();
        }
        return root->data;
    }

    /**
     * @brief push new element to the priority queue.
     * @param e the element to be pushed
     */
    void push(const T &e) {
        Node *newNode = new Node(e);
        try {
            root = mergeNodes(root, newNode);
            queue_size++;
        } catch (...) {
            delete newNode;
            throw;
        }
    }

    /**
     * @brief delete the top element from the priority queue.
     * @throws container_is_empty if empty() returns true
     */
    void pop() {
        if (empty()) {
            throw container_is_empty();
        }

        Node *oldRoot = root;
        try {
            root = mergeNodes(root->left, root->right);
            queue_size--;
            delete oldRoot;
        } catch (...) {
            root = oldRoot;
            throw;
        }
    }

    /**
     * @brief return the number of elements in the priority queue.
     * @return the number of elements.
     */
    size_t size() const {
        return queue_size;
    }

    /**
     * @brief check if the container is empty.
     * @return true if it is empty, false otherwise.
     */
    bool empty() const {
        return queue_size == 0;
    }

    /**
     * @brief merge another priority_queue into this one.
     * The other priority_queue will be cleared after merging.
     * The complexity is at most O(logn).
     * @param other the priority_queue to be merged.
     */
    void merge(priority_queue &other) {
        if (this == &other) return;

        try {
            root = mergeNodes(root, other.root);
            queue_size += other.queue_size;
            other.root = nullptr;
            other.queue_size = 0;
        } catch (...) {
            throw;
        }
    }
};

}

#endif