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
// Date: 30.11.2025
// E-mail: piotr@beesoft.pl.

/*------- include files:
-------------------------------------------------------------------*/
#include "shared.h"
#include <string>

namespace bee {
    /****************************************************************
     *                                                               *
     *                          j o i n                              *
     *                                                               *
     ****************************************************************/

    String join(Span<const String> const data, char const delimiter, Option<char> const spacer) noexcept {
        if (data.empty())
            return {};

        // Obliczamy sumaryczną długość wszystkich stringów.
        size_t size = 0;
        for (auto const &str: data)
            size += str.length();

        // Liczba dodatkowych znaków, znaki delimiterów.
        auto n = data.size() - 1;
        // Jeśli ma być dodawany spacer, to liczba dodatkowych znaków się podwaja.
        if (spacer)
            n *= 2;

        size += n;
        String buffer;
        buffer.reserve(size);

        for (auto it = data.begin(); it != std::prev(data.end()); ++it) {
            buffer.append(*it);
            buffer.push_back(delimiter);
            if (spacer)
                buffer.push_back(*spacer);
        }
        buffer.append(*std::prev(data.end()));
        buffer.shrink_to_fit();

        return std::move(buffer);
    }
}
