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
// Date: 2.11.2025
// E-mail: piotr@beesoft.pl.

/*------- include files:
-------------------------------------------------------------------*/
#include "gzip.h"
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>

namespace bee {
    /********************************************************************
    *                                                                   *
    *                        c o m p r e s s                            *
    *                                                                   *
    ********************************************************************/

    Vector<char> compress(Span<const char> const plain) noexcept {
        namespace bio = boost::iostreams;
        std::vector<char> buffer{};

        bio::filtering_ostream in;
        in.push(bio::gzip_compressor(bio::gzip::best_compression));
        in.push(bio::back_inserter(buffer));
        bio::write(in, plain.data(), static_cast<uint>(plain.size()));
        in.flush();

        buffer.shrink_to_fit();
        return buffer;
    }

    /********************************************************************
    *                                                                   *
    *                      d e c o m p r e s s                          *
    *                                                                   *
    ********************************************************************/

    Vector<char> decompress(Span<const char> const compressed) noexcept {
        namespace bio = boost::iostreams;
        std::vector<char> buffer{};

        bio::filtering_ostream in;
        in.push(bio::gzip_decompressor());
        in.push(bio::back_inserter(buffer));
        bio::write(in, compressed.data(), static_cast<uint>(compressed.size()));
        in.flush();

        buffer.shrink_to_fit();
        return buffer;
    }
}
