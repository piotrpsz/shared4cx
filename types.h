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
// Date: 29.11.2025
// E-mail: piotr@beesoft.pl.
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <optional>
#include <variant>
#include <array>
#include <expected>
#include <unordered_set>
#include <unordered_map>

template<typename T>
concept Stringable =
        std::is_class_v<std::string> ||
            std::is_class_v<std::string_view>;

template<typename T>
concept Vectorizable =
    std::is_class_v<std::vector<T>> ||
        std::is_class_v<std::vector<const T>> ||
            std::is_class_v<std::span<T>> ||
                std::is_class_v<std::span<const T>>;

template<typename T>
concept BytesContainer =
        std::is_class_v<std::string> ||
            std::is_class_v<std::vector<char> >||
                std::is_class_v<std::vector<char const>> ||
                    std::is_class_v<std::vector<unsigned char>> ||
                        std::is_class_v<std::vector<unsigned char const>>;

template <typename T>
concept BytesView =
    std::is_class_v<std::string_view> ||
        std::is_class_v<std::span<char>> ||
            std::is_class_v<std::span<char const>> ||
                std::is_class_v<std::span<unsigned char>> ||
                    std::is_class_v<std::span<unsigned char const>>;

template <typename T>
concept Bytes = BytesContainer<T> || BytesView<T>;

namespace bee {
    static constexpr auto Yes = true;
    static constexpr auto No = false;

    using i8 = char;
    using u8 = unsigned char;
    using i16 = int16_t;
    using u16 = uint16_t;
    using i32 = int32_t;
    using u32 = uint32_t;
    using i64 = int64_t;
    using u64 = uint64_t;
    using f32 = float;
    using f64 = double;
    using uint = unsigned int;

    using Errc = std::errc;
    using String = std::string;
    using StringView = std::string_view;
    using Bytes = std::vector<char>;
    using BytesView = std::span<char>;
    using BytesViewConst = std::span<const char>;
    template<typename T, std::size_t N> using Array = std::array<T, N>;
    template<typename T> using Set = std::unordered_set<T>;
    template<typename K, typename V> using Map = std::unordered_map<K, V>;
    template<typename T> using Vector = std::vector<T>;
    template<typename T> using Span = std::span<T>;
    template<typename T> using Option = std::optional<T>;
    template<typename T, typename E> using Result = std::expected<T, E>;
    template<typename T> using Failure = std::unexpected<T>;
    template<typename T, typename U> using Pair = std::pair<T, U>;
    template<typename... T> using Variant = std::variant<T...>;

}
