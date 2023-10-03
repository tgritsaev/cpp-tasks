#include <iostream>
#include "error.h"
#include "tokenizer.h"

bool Tokenizer::IsEnd() {
    while (static_cast<char>(in_->peek()) == ' ' || in_->peek() == 10) {
        in_->get();
    }
    return (in_->peek() == -1);
}

Tokenizer::Tokenizer(std::istream* in) : in_(in) {
    for (char s = 'a'; s <= 'z'; ++s) {
        start_symbols_.insert(s);
    }
    for (char s = 'A'; s <= 'Z'; ++s) {
        start_symbols_.insert(s);
    }
    // < = > * / #;
    start_symbols_.insert('<');
    start_symbols_.insert('=');
    start_symbols_.insert('>');
    start_symbols_.insert('*');
    start_symbols_.insert('/');
    start_symbols_.insert('#');
    start_symbols_.insert('-');
    start_symbols_.insert('+');

    symbols_ = start_symbols_;
    // 0-9 ? ! -
    for (char s = '0'; s <= '9'; ++s) {
        symbols_.insert(s);
    }
    symbols_.insert('?');
    symbols_.insert('!');
    symbols_.insert('-');
}

void Tokenizer::Next() {
    if (is_called_get_token_ == false) {
        try {
            GetToken();
        } catch (...) {
            throw;
        }
    }
    is_called_get_token_ = false;
}

Token Tokenizer::GetToken() {
    try {
        if (is_called_get_token_ == true) {
            return last_token_;
        }
        is_called_get_token_ = true;
        char s;
        while (SimpleIsEnd() == false && (s = in_->get()) == ' ') {
        }
        auto is_digit = [](char s) { return '0' <= s && s <= '9'; };
        auto check_if_normal_symbol = [&](int s) {
            auto one_rule = (('0' <= s && s <= '9') || (symbols_.contains(s)) || SimpleIsEnd() ||
                             s == ' ' || s == ')' || s == '(');
            if (one_rule) {
                return true;
            }
            throw SyntaxError(":(");
        };

        if (s == '(') {
            last_token_ = BracketToken::OPEN;
        } else if (s == ')') {
            last_token_ = BracketToken::CLOSE;
        } else if (is_digit(s) || ((s == '-' || s == '+') && SimpleIsEnd() == false &&
                                   is_digit(in_->peek()) == true)) {
            int mul = 1;
            int value = 0;
            if (s == '-') {
                mul = -1;
            } else if (is_digit(s)) {
                value = s - '0';
            }
            while (is_digit(s = in_->peek())) {
                value = value * 10 + (s - '0');
                in_->get();
            }
            try {
                check_if_normal_symbol(in_->peek());
            } catch (...) {
                throw;
            }
            last_token_ = ConstantToken{value * mul};
        } else if (s == '.') {
            last_token_ = DotToken();
        } else if (s == '\'') {
            last_token_ = QuoteToken();
        } else if (start_symbols_.contains(s) == true) {
            std::string str{s};
            while (SimpleIsEnd() == false && symbols_.contains(s = in_->peek())) {
                str += s;
                in_->get();
            }
            last_token_ = SymbolToken{str};
        } else {
            throw SyntaxError("no such symbol");
        }
        return last_token_;
    } catch (...) {
        throw;
    }
}
