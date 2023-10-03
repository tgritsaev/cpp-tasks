#pragma once

#include <vector>
#include <cstddef>
#include <cstdint>
#include <glog/logging.h>

// HuffmanTree decoder for DHT section.
class HuffmanTree {
private:
    struct Node;

public:
    // code_lengths is the array of size no more than 16 with number of
    // terminated nodes in the Huffman tree.
    // values are the values of the terminated nodes in the consecutive
    // level order.
    void Build(const std::vector<uint8_t>& code_lengths, const std::vector<uint8_t>& values);

    // Moves the state of the huffman tree by |bit|. If the node is terminated,
    // returns true and overwrites |value|. If it is intermediate, returns false
    // and value is unmodified.
    bool Move(bool bit, int& value);

    void Clear();

    void Print();

    void Throw(const std::string& str);

    ~HuffmanTree() {
        Clear();
    }

private:
    struct Node {
        bool is_terminated = false;
        uint8_t value = 0;

        Node* left = nullptr;
        Node* right = nullptr;
    };
    void InplacePrint(Node* node, std::string path);

    Node* now_node_ = nullptr;
    Node* head_node_ = nullptr;
    std::vector<Node*> nodes_;
};

constexpr int kDebugHuffman = 0;