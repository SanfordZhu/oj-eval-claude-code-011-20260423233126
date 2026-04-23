#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include <utility>
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
        T val;
        int npl;
        Node *left, *right;
        Node(const T &v) : val(v), npl(0), left(nullptr), right(nullptr) {}
    };

    Node *root;
    size_t sz;
    Compare comp;

    void clearTree(Node *node) {
        if (!node) return;
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }

    Node* copyTree(const Node *other) {
        if (!other) return nullptr;
        Node *node = new Node(other->val);
        node->npl = other->npl;
        node->left = copyTree(other->left);
        node->right = copyTree(other->right);
        return node;
    }

    Node* merge(Node *a, Node *b) {
        if (!a) return b;
        if (!b) return a;
        if (comp(a->val, b->val)) {
            std::swap(a, b);
        }
        Node *merged_right = merge(a->right, b);
        a->right = merged_right;
        if (!a->left || (a->left->npl < (a->right ? a->right->npl : -1))) {
            std::swap(a->left, a->right);
        }
        a->npl = (a->right ? a->right->npl : 0) + 1;
        return a;
    }

public:
	/**
	 * @brief default constructor
	 */
	priority_queue() : root(nullptr), sz(0) {}

	/**
	 * @brief copy constructor
	 * @param other the priority_queue to be copied
	 */
	priority_queue(const priority_queue &other) : root(nullptr), sz(other.sz) {
        if (other.root) {
            root = copyTree(other.root);
        }
    }

	/**
	 * @brief deconstructor
	 */
	~priority_queue() {
        clearTree(root);
    }

	/**
	 * @brief Assignment operator
	 * @param other the priority_queue to be assigned from
	 * @return a reference to this priority_queue after assignment
	 */
	priority_queue &operator=(const priority_queue &other) {
        if (this == &other) return *this;
        clearTree(root);
        sz = other.sz;
        if (!other.root) {
            root = nullptr;
        } else {
            root = copyTree(other.root);
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
        return root->val;
    }

	/**
	 * @brief push new element to the priority queue.
	 * @param e the element to be pushed
	 */
	void push(const T &e) {
        Node *node = nullptr;
        try {
            node = new Node(e);
        } catch (...) {
            delete node;
            throw;
        }
        try {
            root = merge(root, node);
        } catch (...) {
            delete node;
            throw sjtu::runtime_error();
        }
        sz++;
    }

	/**
	 * @brief delete the top element from the priority queue.
	 * @throws container_is_empty if empty() returns true
	 */
	void pop() {
        if (empty()) {
            throw container_is_empty();
        }
        Node *old_root = root;
        try {
            root = merge(root->left, root->right);
        } catch (...) {
            root = old_root;
            throw sjtu::runtime_error();
        }
        delete old_root;
        sz--;
    }

	/**
	 * @brief return the number of elements in the priority queue.
	 * @return the number of elements.
	 */
	size_t size() const {
        return sz;
    }

	/**
	 * @brief check if the container is empty.
	 * @return true if it is empty, false otherwise.
	 */
	bool empty() const {
        return sz == 0;
    }

	/**
	 * @brief merge another priority_queue into this one.
	 * The other priority_queue will be cleared after merging.
	 * The complexity is at most O(logn).
	 * @param other the priority_queue to be merged.
	 */
	void merge(priority_queue &other) {
        if (this == &other) return;
        Node *old_this_root = root;
        Node *old_other_root = other.root;
        size_t old_this_sz = sz;
        size_t old_other_sz = other.sz;

        try {
            root = merge(root, other.root);
        } catch (...) {
            root = old_this_root;
            other.root = old_other_root;
            sz = old_this_sz;
            other.sz = old_other_sz;
            throw sjtu::runtime_error();
        }
        sz += other.sz;
        other.root = nullptr;
        other.sz = 0;
    }
};

}

#endif