//
// Created by Piotr Pszczolkowski on 30/11/2025.
//

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
