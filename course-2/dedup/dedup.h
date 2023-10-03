#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <iostream>

using std::string;

std::vector<std::unique_ptr<string>> Duplicate(const std::vector<std::shared_ptr<string>>& items) {
    std::vector<std::unique_ptr<string>> ans(items.size());
    for (size_t i = 0; i < items.size(); ++i) {
        ans[i] = std::make_unique<string>(*items[i].get());
    }
    return ans;
}

std::vector<std::shared_ptr<string>> DeDuplicate(
    const std::vector<std::unique_ptr<string>>& items) {
    std::vector<std::shared_ptr<string>> ans(items.size());
    std::map<string, std::shared_ptr<string>> mp;
    for (size_t i = 0; i < items.size(); ++i) {
        if (mp.find(*items[i].get()) == mp.end()) {
            ans[i] = std::make_shared<string>(*items[i].get());
            mp.insert({*items[i].get(), ans[i]});
        } else {
            ans[i] = mp[*items[i].get()];
        }
    }
    return ans;
}
