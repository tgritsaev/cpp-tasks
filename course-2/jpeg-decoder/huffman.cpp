#include "huffman.h"

void HuffmanTree::Clear() {
    for (auto &node : nodes_) {
        delete node;
    }
    nodes_.resize(0);
}

void HuffmanTree::Throw(const std::string &str) {
    Clear();
    throw std::invalid_argument(str);
}

void HuffmanTree::InplacePrint(Node *node, std::string path) {
    if (node == nullptr) {
        return;
    }
    DLOG_IF(INFO, node->is_terminated) << path << "\t" << static_cast<int>(node->value);
    InplacePrint(node->left, path + "0");
    InplacePrint(node->right, path + "1");
}

void HuffmanTree::Print() {
    InplacePrint(head_node_, "");
}

void HuffmanTree::Build(const std::vector<uint8_t> &code_lengths,
                        const std::vector<uint8_t> &values) {
    if (code_lengths.size() > 16) {
        throw std::invalid_argument("code_lengths.size() invalid");
    }
    Clear();

    now_node_ = new Node();
    head_node_ = now_node_;
    nodes_.emplace_back(now_node_);
    size_t left = 0;
    size_t value_idx = 0;
    for (uint8_t terminal_nodes_on_this_level_cnt : code_lengths) {
        // spawn start
        size_t right = nodes_.size();
        size_t inc = 0;
        for (size_t i = left; i < right; ++i) {
            auto process_new_node = [&](Node *new_node) {
                if (terminal_nodes_on_this_level_cnt > 0) {
                    --terminal_nodes_on_this_level_cnt;
                    new_node->is_terminated = true;
                    if (value_idx >= values.size()) {
                        delete new_node;
                        throw std::invalid_argument(
                            "values.size() is not consistent with code_lengths");
                    }
                    new_node->value = values[value_idx++];
                    ++inc;
                }
                nodes_.emplace_back(new_node);
            };

            nodes_[i]->left = new Node();
            process_new_node(nodes_[i]->left);

            nodes_[i]->right = new Node();
            process_new_node(nodes_[i]->right);
        }
        if (terminal_nodes_on_this_level_cnt > 0) {
            throw std::invalid_argument("too many terminal nodes_ for this level");
        }
        left = right + inc;
        // spawn end
    }
    DLOG_IF(INFO, kDebugHuffman == 1) << "HuffmanTree::Build end";
}

bool HuffmanTree::Move(bool bit, int &value) {
    if (now_node_ == nullptr) {
        throw std::invalid_argument("now_node_ is nullptr");
    }
    if (bit == true) {
        now_node_ = now_node_->right;
    } else {
        now_node_ = now_node_->left;
    }

    if (now_node_ == nullptr) {
        throw std::invalid_argument("now_node_ became nullptr");
    }
    if (now_node_->is_terminated) {
        value = now_node_->value;
        now_node_ = head_node_;
        return true;
    } else {
        return false;
    }
}
