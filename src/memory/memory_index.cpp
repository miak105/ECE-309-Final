// src/memory/memory_index.cpp — YOU IMPLEMENT. Spec §4.3.

#include "memory/memory_index.h"

MemoryIndex::~MemoryIndex() {
    clear();
}

bool MemoryIndex::insert(MemoryRecord rec) {
    Node** current = &root_;

    while ( *current != nullptr ) {
        if (rec.id == (*current)->rec.id) {
            return false;
        }

        if (rec.id < (*current)->rec.id) {
            current = &((*current)->left);
        } else {
            current = &((*current)->right);
        }
    }

    *current = new Node{rec, nullptr, nullptr};
    ++count_;
    return true;
}

const MemoryRecord* MemoryIndex::find(std::uint64_t id) const {
    Node* current = root_;

    while (current != nullptr) {
        if (id == current->rec.id) {
            return &current->rec;
        }

        if (id < current->rec.id) {
            current = current->left;
        } else {
            current = current->right;
        }
    }

    return nullptr;
}

bool MemoryIndex::remove(std::uint64_t id) {
    Node** current = &root_;

    // Find the node
    while (*current != nullptr && (*current)->rec.id != id) {
        if (id < (*current)->rec.id) {
            current = &((*current)->left);
        } else {
            current = &((*current)->right);
        }
    }

    if (*current == nullptr) {
        return false;
    }

    Node* node = *current;

    // No left child
    if (node->left == nullptr) {
        *current = node->right;
        delete node;
    }
    // No right child
    else if (node->right == nullptr) {
        *current = node->left;
        delete node;
    }
    // Two children
    else {
        // Find the smallest node in the right subtree
        Node** successor = &(node->right);

        while ((*successor)->left != nullptr) {
            successor = &((*successor)->left);
        }

        Node* next = *successor;

        node->rec = next->rec;
        *successor = next->right;

        delete next;
    }

    --count_;
    return true;
}

std::size_t MemoryIndex::size() const noexcept { return count_; }

std::size_t MemoryIndex::height() const {
    return height_of(root_);
}

std::size_t MemoryIndex::height_of(const Node* node) {
    if (node == nullptr) {
        return 0;
    }

    std::size_t left = height_of(node->left);
    std::size_t right = height_of(node->right);

    return 1 + (left > right ? left : right);
}

void MemoryIndex::in_order(const Node* node,const std::function<void(const MemoryRecord&)>& fn) {
    if (node == nullptr) {
        return;
    }

    in_order(node->left, fn);
    fn(node->rec);
    in_order(node->right, fn);
}

void MemoryIndex::for_each_in_order(const std::function<void(const MemoryRecord&)>& fn) const {
    in_order(root_, fn);
}

void MemoryIndex::clear() {
    destroy(root_);

    root_ = nullptr;
    count_ = 0;
}

void MemoryIndex::destroy(Node* node) {
    if (node == nullptr) {
        return;
    }

    // Delete children before deleting the parent
    destroy(node->left);
    destroy(node->right);

    delete node;
}