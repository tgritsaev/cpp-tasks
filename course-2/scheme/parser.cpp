#include "parser.h"
#include <variant>
#include <vector>
#include "error.h"

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    auto token = tokenizer->GetToken();
    if (std::holds_alternative<BracketToken>(token) &&
        std::get<BracketToken>(token) == BracketToken::OPEN) {
        tokenizer->Next();
        try {
            return ReadList(tokenizer);
        } catch (...) {
            throw SyntaxError("1.1. :(");
        }
    } else if (std::holds_alternative<ConstantToken>(token)) {
        tokenizer->Next();
        return std::make_shared<Number>(std::get<ConstantToken>(token).value);
    } else if (std::holds_alternative<SymbolToken>(token)) {
        tokenizer->Next();
        return std::make_shared<Symbol>(std::get<SymbolToken>(token).name);
    } else {
        throw SyntaxError("1.2. :(");
    }
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    std::vector<std::shared_ptr<Object>> objects;
    int last_dot = -1;
    int first_dot = -1;
    int tokens_cnt = 0;
    Token token;
    bool was_close = false;
    while (!(was_close = std::holds_alternative<BracketToken>(token = tokenizer->GetToken()) &&
                         std::get<BracketToken>(token) == BracketToken::CLOSE)) {
        if (std::holds_alternative<DotToken>(token)) {
            last_dot = tokens_cnt;
            if (first_dot == -1) {
                first_dot = tokens_cnt;
            } else {
                first_dot = std::min(first_dot, tokens_cnt);
            }
            tokenizer->Next();
        } else {
            ++tokens_cnt;
            auto x = Read(tokenizer);
            objects.push_back(x);
        }
    }
    if (was_close == false || last_dot >= tokens_cnt || first_dot != last_dot ||
        (first_dot != -1 && first_dot + 1 != tokens_cnt)) {
        throw SyntaxError("2.1. :(");
    }
    tokenizer->Next();
    if (objects.empty() == true) {
        return nullptr;
    } else if (objects.size() == 1) {
        return std::make_shared<Cell>(Cell(nullptr, nullptr));
    }
    std::shared_ptr<Cell> cell = std::make_shared<Cell>(Cell());
    std::shared_ptr<Cell> ret_cell;
    for (size_t i = 0; i + 1 < objects.size(); ++i) {
        cell->first_ = objects[i];
        if (i == 0) {
            ret_cell = cell;
        }
        if (i + 2 == objects.size()) {
            if (last_dot == tokens_cnt - 1) {
                cell->second_ = objects[i + 1];
            } else {
                cell->second_ = std::make_shared<Cell>(Cell{objects[i + 1], nullptr});
            }
            break;
        } else {
            cell->second_ = std::make_shared<Cell>(Cell());
            cell = std::dynamic_pointer_cast<Cell>(cell->second_);
        }
    }
    return ret_cell;
}
