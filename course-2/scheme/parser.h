#pragma once

#include <memory>
#include <type_traits>
#include <typeinfo>
#include <iostream>

#include "object.h"
#include "tokenizer.h"

std::shared_ptr<Object> Read(Tokenizer* tokenizer);

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer);