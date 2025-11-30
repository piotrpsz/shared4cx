//
// Created by Piotr Pszczolkowski on 09/11/2025.
//

#pragma once

#include "types.h"
#include <format>

namespace bee {
    struct Error {
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
