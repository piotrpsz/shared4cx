// MIT License
//
// Copyright (c) 2024 Piotr Pszczółkowski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Author: Piotr Pszczółkowski
// Date: 9.11.2025
// E-mail: piotr@beesoft.pl.
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include "types.h"
#include <format>

namespace bee {
    struct Error final {
        int code{-1};
        String message{};
        String extra_message{};

        Error() = default;
        Error(int const code_, String message_) : code(code_), message(std::move(message_)) {}
        Error(int const code_, String message_, String extra_) : code(code_), message(std::move(message_)), extra_message(std::move(extra_)) {}
        explicit Error(String message_) : message(std::move(message_)) {}
        Error(Error const&) = default;
        Error& operator=(Error const&) = default;
        Error(Error&&) = default;
        Error& operator=(Error&&) = default;
        ~Error() = default;
    };
    struct Unit {};
    static constexpr auto Success = Unit{};
    enum class ErrorType {Normal, Critical};
}

/// Funkcja formatująca obiekt (jako string).
template<>
struct std::formatter<bee::Error> : std::formatter<std::string> {
    auto format(bee::Error const& err, std::format_context& ctx) const {
        if (err.extra_message.empty())
            return formatter<std::string>::format(std::format("Error: {} ({}).", err.message, err.code), ctx);
        return formatter<std::string>::format(std::format("Error: [{}] {}. {}.", err.code, err.message, err.extra_message), ctx);
    }
};
