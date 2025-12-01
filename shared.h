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
// Date: 7.05.2025
// E-mail: piotr@beesoft.pl.
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include "types.h"
#include "error.h"
#include <span>
#include <optional>
#include <string>
#include <iostream>
#include <format>
#include <vector>
#include <algorithm>
#include <charconv>
#include <print>
#include <pwd.h>
#include <unistd.h>
#include <filesystem>

namespace bee {
    inline String str(Bytes const& data) noexcept {
        return String{data.data(), data.size()};
    }

    inline Vector<u8> vec(Bytes const& data) noexcept {
        return Vector<u8>{data.begin(), data.end()};
    }

    /// Odczytanie katalogu domowego użytkownika.
    inline Option<String> homeDirectory() noexcept {
        if (auto const pw = getpwuid(getuid()))
            return pw->pw_dir;
        return {};
    }

    /// Tworzenie ciągu podkatalogów.
    /// \param path Ścieżka do katalogu docelowego.
    /// \return Błąd, jeśli coś poszło nie tak lub nic nie zwraca.
    inline Option<Error> createDirectory(std::string_view const path) noexcept {
        namespace fs = std::filesystem;
        std::error_code ec{};

        // Jeśli wskazany katalog już istnieje to nie ma nic do roboty.
        if (fs::exists(path, ec))
            return {};
        if (ec != std::errc{})
            return Error(ec.value(), ec.message());

        // Tworzymy katalog z podkatalogami.
        if (fs::create_directories(path, ec))
            return {};
        return Error(ec.value(), ec.message());
    }

    /// Sprawdzenie, czy przysłany znak NIE jest białym znakiem.
    /// \param c Znak do sprawdzenia
    /// \return TRUE, jeśli NIE jest białym znakiem, FALSE w przeciwnym przypadku (jest białym znakiem).
    inline bool is_not_space (char const c) noexcept {
        return !std::isspace(c);
    }

    /// Obcięcie początkowych białych znaków.
    /// \param s Tekst, z którego należy usunąć białe znaki
    /// \return Tekst bez początkowych białych znaków.
    inline String trim_left(String s) noexcept {
        s.erase(s.begin(), std::ranges::find_if(s, is_not_space));
        return s;
    }

    /// Usunięcie zamykających (końcowych) białych znaków (z prawej strony).
    /// \param s Tekst, z którego należy usunąć białe znaki
    /// \return Tekst bez zamykających białych znaków.
    inline String trim_right(String s) noexcept {
        s.erase(std::find_if(s.rbegin(), s.rend(), is_not_space).base(), s.end());
        return s;
    }

    /// Usunięcie początkowych i końcowych białych znaków.
    /// \param s Tekst, z którego należy usunąć białe znaki
    /// \return Tekst bez początkowych i końcowych białych znaków.
    inline String trim(String s) noexcept {
        return trim_left(trim_right(std::move(s)));
    }

    /****************************************************************
    *                                                               *
    *                        s p l i t                              *
    *                                                               *
    ****************************************************************/

    /// Podział tekstu na części. Podział w miejscach znaku 'delimiter'.
    /// \param sv String do podziału,
    /// \param delimiter Znak rozdzielający części tekstu,
    /// \param accept_empty Czy puste części też zachować?
    /// \return Wektor zawierający części tekstu.
    Vector<String> split(Stringable auto sv, char delimiter = ',', bool accept_empty = false) noexcept {
        // Liczymy ile będzie znaków podziałów, aby zaalokować wynikowy wektor w stosownym rozmiarze.
        size_t n{};
        for (auto it = sv.begin(); it != sv.end(); ++it)
            if (*it == delimiter)
                ++n;

        // Wynikowy wektor ma 'n + 1' elementów
        Vector<String> result;
        result.reserve(n + 1);
        // Bufor na zbieranie znaków linii.
        String line{};

        // Funkcja, dodająca (lub nie) aktualną linię do wynikowego wektora.
        // Po jej zakończeniu zawartość linii jest usuwana.
        auto add = [&line, &result, accept_empty]() {
            if (auto const text = trim(line); text.empty()) {
                if (accept_empty)
                    result.emplace_back("");
            } else {
                result.emplace_back(text);
            }
            line.clear();
        };

        // Obsługa elementów wewnętrznych.
        for (auto const c : sv)
            (c == delimiter) ? add() : line.push_back(c);

        // Obsłuż ostatni element (niezakończony delimiterem).
        add();

        result.shrink_to_fit();
        return result;
    }


    /// Połączenie wektora tekstów w jeden tekst (jedna linia).
    /// \param data Wektor stringów,
    /// \param delimiter Znak oddzielający składniki (domyślnie przecinek),
    /// \param spacer Znak dodawany po znaku oddzielającym (domyślnie brak).
    /// \return Linia tekstu ze składowymi tekstami z wektora.
    String join(Span<const String> data, char delimiter = ',', std::optional<char> spacer = {}) noexcept;

    /****************************************************************
    *                                                               *
    *                        b y t e s 2 h e x                      *
    *                                                               *
    ****************************************************************/

    /// Utworzenie hex-tekstu z ciągu bajtów.
    /// Bajty będą oddzielone od siebie przecinkiem.
    /// Po przecinku może być dodany opcjonalny separator, np. spacja.
    String bytes2hex(Vectorizable auto const bytes, std::optional<char> const spacer = {}) noexcept {
        Vector<String> result;
        result.reserve(bytes.size());

        for (u8 c : bytes)
            result.push_back(std::format("0x{:02x}", c));

        return join(result, ',', spacer);
    }

    /****************************************************************
    *                                                               *
    *                   f r o m _ b y t e s                         *
    *                                                               *
    ****************************************************************/

    /// Konwersja wektora bajtów na liczbę całkowitą o wskazanym typie (rozmiarze).
    /// Liczba bajtów musi być równa lub większa od wskazanego typu wynikowego.
    template<std::integral T>
    std::optional<T> from_bytes(Vectorizable auto const span) noexcept {
        if (span.size() >= sizeof(T)) {
            T value{};
            value = *reinterpret_cast<T const*>(span.first(sizeof(T)).data());
            return value;
        }
        return {};
    }
    // template<std::integral T>
    // std::optional<T> from_bytes(Span<const char> const span) noexcept {
    //     if (span.size() >= sizeof(T)) {
    //         T value{};
    //         value = *reinterpret_cast<T const*>(span.first(sizeof(T)).data());
    //         return value;
    //     }
    //     return {};
    // }

    /****************************************************************
    *                                                               *
    *                     t o _ b y t e s                           *
    *                                                               *
    ****************************************************************/

    /// Konwersja liczby całkowitej na wektor bajtów.
    template<std::integral T>
    Vector<u8> to_bytes(T const value) noexcept {
        Vector<u8> bytes;
        bytes.resize(sizeof(T));
        memcpy(bytes.data(), &value, sizeof(T));
        return std::move(bytes);
    }

    /****************************************************************
    *                                                               *
    *                    a s _ s t r i n g                          *
    *                                                               *
    ****************************************************************/

    /// Zamiana liczby całkowitej na string.
    String as_string(std::integral auto const v) {
        return std::to_string(static_cast<i64>(v));
    }
    /// Zamiana liczby zmiennoprzecinkowej na string.
    String as_string(std::floating_point auto v) {
        return std::to_string(static_cast<f64>(v));
    }

    /****************************************************************
    *                                                               *
    *                     t o _ l o w e r                           *
    *                                                               *
    ****************************************************************/

    /// Zmiana wszystkich znaków w tekście na małe litery.
    /// \param s Tekst do konwersji,
    /// \return Tekst, w którym wszystkie litery są małe.
    String to_lower(Stringable auto const s) noexcept {
        String data;
        data.reserve(s.size());

        for (auto const c : s)
            data.push_back(static_cast<char>(std::tolower(c)));
        return data;
    }

    /****************************************************************
    *                                                               *
    *                        t o _ i n t                            *
    *                                                               *
    ****************************************************************/

    /// Zamiana tekstu na liczbę całkowitą.
    /// \param s Tekst zawierający liczbę,
    /// \param radix Format-system zapisanej liczby.
    /// \return Liczba całkowita, jeśli wszystko dobrze.
    Option<int> to_int(Stringable auto const s, int const radix = 10) noexcept {
        int value = 0;
        auto const [_, ec] = std::from_chars(s.data(), s.data() + s.size(), value, radix);

        if (ec == std::errc{})
            return value;

        // Coś poszło nie tak.
        if (ec == std::errc::invalid_argument)
            std::println(std::cerr, "This is not a number ({}).", s);
        else if (ec == std::errc::result_out_of_range)
            std::println(std::cerr, "The number is to big ({}).", s);
        return {};
    }

    /****************************************************************
    *                                                               *
    *                 f m t _ a s _ s t r i n g                     *
    *                                                               *
    ****************************************************************/

    /// Zamiana liczby całkowitej na string z separatorami pomiędzy
    /// grupami tysięcy.
    String fmt_as_string(std::integral auto const value, char const separator = '.') noexcept {
        auto const text = std::to_string(value);
        auto const size = text.size();
        String buffer{};
        buffer.reserve(size + size/3);
        auto offset{0};

        if (value < 0) {
            buffer.push_back('-');
            offset = 1;
        }

        auto comma_idx = 3 - (size - offset) % 3;
        if (comma_idx == 3) comma_idx = 0;

        for (auto i = offset; i < size; ++i) {
            if (comma_idx == 3) {
                buffer.push_back(separator);
                comma_idx = 0;
            }
            ++comma_idx;
            buffer.push_back(text[i]);
        }

        return std::move(buffer);
    }
}
