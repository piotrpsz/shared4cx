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
// Date: 10.11.2025
// E-mail: piotr@beesoft.pl.
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include "types.h"
#include <date/date.h>
#include <date/tz.h>

namespace bee {
    using zoned_time_t = date::zoned_time<std::chrono::seconds>;
    using Minutes = std::chrono::minutes;
    using Seconds = std::chrono::seconds;
    using Days = std::chrono::days;

    class Datime {
        date::time_zone const* zone =  date::locate_zone("Europe/Warsaw");
        zoned_time_t tp_{};
    public:
        struct Date { int y{}, m{}, d{}; };
        struct Time { int h{}, m{}, s{}; };

        /// CTOR default: aktualna data i czas na komputerze.
        Datime() {
            auto const now = std::chrono::system_clock::now();
            tp_ = date::make_zoned(zone, std::chrono::floor<Seconds>(now));
        }

        /// CTOR: data i czas z tekstu.
        explicit Datime(String const& str) {
            std::stringstream ss{str};
            date::local_time<Seconds> tmp;
            date::from_stream(ss, "%F %X", tmp);
            tp_ = make_zoned(zone, tmp);
        }

        /// CTOR: data i czas z liczby sekund od początku epoki.
        explicit Datime(i64 const timestamp) {
            auto const tp = std::chrono::system_clock::from_time_t(timestamp);
            tp_ = date::make_zoned(zone, std::chrono::floor<Seconds>(tp));
        }

        /// CTOR: data i czas z lokalnego punktu-w-czasie
        explicit Datime(zoned_time_t const tp) : tp_{tp} {}

        /// CTOR: data i czas ze struktur daty i czasu.
        explicit Datime(Date const dt, Time const tm) : tp_{from_components(dt, tm)} {}

        Datime(Datime const& other) = default;
        Datime(Datime&& other) = default;
        Datime& operator=(Datime const& other) = default;
        Datime& operator=(Datime&& other) = default;
        ~Datime() = default;

        /// Zwraca liczbę sekund od początku epoki.
        [[nodiscard]] i64 timestamp() const noexcept {
            auto const seconds = tp_.get_sys_time().time_since_epoch().count();
            return seconds;
        }

        /// Sprawdzenie, czy dwa obiekty odnoszą się do tego samego punktu-w-czasie.
        bool operator==(Datime const &rhs) const noexcept {
            return timestamp() == rhs.timestamp();
        }
        bool operator!=(Datime const &rhs) const noexcept {
            return !(*this == rhs);
        }
        bool operator<(Datime const &rhs) const noexcept {
            return timestamp() < rhs.timestamp();
        }
        bool operator>(Datime const &rhs) const noexcept {
            return timestamp() > rhs.timestamp();
        }

        /// Prezentacja daty i czasy w postaci tekstu (mp. "2025-11-11 19:09:28").
        [[nodiscard]] String to_string() const noexcept {
            return date::format("%F %X", tp_.get_local_time());
        }

        /// Zwraca informację o dacie.
        [[nodiscard]] Date date() const noexcept {
            auto const days = date::floor<Days>(tp_.get_local_time());
            date::year_month_day ymd{days};
            auto const year = static_cast<int>(ymd.year());
            auto const month = static_cast<int>(static_cast<unsigned>(ymd.month()));
            auto const day = static_cast<int>(static_cast<unsigned>(ymd.day()));
            return {year, month, day};
        }

        /// Zwraca informację o czasie.
        [[nodiscard]] Time time() const noexcept {
            auto const days = date::floor<Days>(tp_.get_local_time());
            date::hh_mm_ss const hms{tp_.get_local_time() - days};
            auto const hour = static_cast<int>(hms.hours().count());
            auto const min = static_cast<int>(hms.minutes().count());
            auto const sec = static_cast<int>(hms.seconds().count());
            return {hour, min, sec};
        }

        /// Ustawienie czasu. Data pozostaje bez zmian.
        Datime& time(Time const& tm) noexcept {
            auto const t = date::floor<Days>(tp_.get_local_time())
                + std::chrono::hours(tm.h)
                + std::chrono::minutes(tm.m)
                + std::chrono::seconds(tm.s);
            tp_ = make_zoned(zone, t);
            return *this;
        }

        /// Wyzerowanie czasu, zostaje tylko data.
        Datime& clear_time() noexcept {
            auto const t = date::floor<Days>(tp_.get_local_time())
                + std::chrono::hours(0)
                + std::chrono::minutes(0)
                + std::chrono::seconds(0);
            tp_ = make_zoned(zone, t);
            return *this;
        }

        /// Wyzerowanie sekund.
        /// Minuty są zaokrąglane w zależności od zastanej liczby sekund.
        Datime& clear_seconds() noexcept {
            auto const days = date::floor<Days>(tp_.get_local_time());
            date::hh_mm_ss const hms{tp_.get_local_time() - days};
            auto t = date::floor<Days>(tp_.get_local_time())
                    + hms.hours()
                    + hms.minutes()
                    + std::chrono::seconds(hms.seconds().count() >= 30 ? 60 : 0);
            tp_ = make_zoned(zone, t);
            return *this;
        }

        /// Pobranie składowych, czyli daty i czasu.
        /// \return Para: data i czas.
        [[nodiscard]] Pair<Date,Time> components() const noexcept {
            return { date(), time() };
        }

        /// Zwraca liczbę minut od wskazanego obiektu data-czas.
        /// \return Liczba minut.
        [[nodiscard]] i64 minutes_since(Datime const& other) const noexcept {
            auto const a = date::floor<Minutes>(tp_.get_sys_time());
            auto const b = date::floor<Minutes>(other.tp_.get_sys_time());
            return (b - a).count();
        }

        /// Data i czas na początek dnia.
        Datime& beginning_day() noexcept {
            return clear_time();
        }

        /// Data i czas na koniec dnia.
        Datime& end_day() noexcept {
            return time({23,59,59});
        }

        /// Utworzenie nowego dla daty odległej o wskazaną liczbę dni.
        /// \param n Liczba dni przesunięcia (dodatnia lub ujemna)
        /// \return Obiekt nowej daty.
        [[nodiscard]] Datime add_days(int const n) const noexcept {
            auto const days = date::floor<Days>(tp_.get_local_time());
            date::hh_mm_ss const hms{tp_.get_local_time() - days};

            auto const added = days + std::chrono::days(n);
            auto const secs = added + hms.hours() + hms.minutes() + hms.seconds();
            return Datime(make_zoned(zone, secs));
        }

        /// Wyznaczanie obiektu daty dla następnego dnia.
        [[nodiscard]] Datime next_day() const noexcept {
            return add_days(1);
        }

        /// Wyznaczenie obiektu daty dla poprzedniego dnia.
        [[nodiscard]] Datime prev_day() const noexcept {
            return add_days(-1);
        }

        /// Zwraca numer tego dnia w tym tygodniu.
        /// 1-poniedziałek, 2-wtorek, ... , 7-niedziela.
        [[nodiscard]] auto week_day() const noexcept {
            auto const wd = date::floor<Days>(tp_.get_local_time());
            return date::weekday(wd).iso_encoding();
        }

        /// Wyznaczanie daty początku i końca tego tygodnia.
        /// \return Para dat odpowiadających początkowi i końcowi tygodnia.
        [[nodiscard]] Pair<Datime,Datime> week_range() const noexcept {
            auto const today_idx = static_cast<int>(week_day());
            return {add_days(-today_idx + 1), add_days(7 - today_idx)};
        }

    private:
        [[nodiscard]] zoned_time_t from_components(Date const dt, Time const tm) const noexcept {
            namespace chrono = std::chrono;
            date::year_month_day const ymd = date::year(dt.y) / dt.m / dt.d;
            auto const days = static_cast<date::local_days>(ymd);
            auto const time = days + chrono::hours{tm.h} + chrono::minutes{tm.m} + chrono::seconds{tm.s};
            return date::make_zoned(zone, time);
        }
    };
}

template<>
struct std::formatter<bee::Datime> : std::formatter<std::string> {
    auto format(bee::Datime const& dt, std::format_context& ctx) const {
        return formatter<std::string>::format(std::format("{}", dt.to_string()), ctx);
    }
};