// Copyright 2022 Anton Korobeynikov

// This file is part of Bandage

// Bandage is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Bandage is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with Bandage.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "cigar.h"

#include <string_view>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <ostream>
#include <algorithm>

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cinttypes>

namespace gfa {
using cigar::tag;
using cigar::cigarop;
using cigar::getTag;
using cigar::cigar_string;

struct header {
    std::vector<gfa::tag> tags;

    header() = default;

    explicit header(std::vector<gfa::tag> t)
            : tags(std::move(t)) {}

    void print() const {
        std::fputs("H", stdout);
        for (const auto &tag : tags) {
            fputs("\t", stdout);
            tag.print();
        }
    }
};

struct segment {
    std::string_view name;
    std::string_view seq;
    std::vector<gfa::tag> tags;

    explicit segment(std::string_view n, std::vector<gfa::tag> t)
            : name{n}, tags(std::move(t)) {}

    template<typename Seq>
    explicit segment(std::string_view n, Seq s, std::vector<gfa::tag> t)
            : name{n}, seq{s.data(), s.size()}, tags(std::move(t)) {}

    void print() const {
        std::fputs("S", stdout);
        std::fprintf(stdout, "\t%s", std::string(name).c_str());
        std::fprintf(stdout, "\t%s", !seq.empty() ? std::string(seq).c_str() : "*");
        for (const auto &tag : tags) {
            fputs("\t", stdout);
            tag.print();
        }
    }
};

struct link {
    std::string_view lhs;
    bool lhs_revcomp;
    std::string_view rhs;
    bool rhs_revcomp;
    cigar_string overlap;
    std::vector<gfa::tag> tags;

    explicit link(std::string_view l, std::string_view lr, std::string_view r, std::string_view rr,
                  std::vector<gfa::tag> t)
            : lhs{l}, lhs_revcomp(lr.front() == '-'), rhs{r}, rhs_revcomp(rr.front() == '-'),
              tags(std::move(t)) {}

    explicit link(std::string_view l, std::string_view lr, std::string_view r, std::string_view rr,
                  cigar_string o,
                  std::vector<gfa::tag> t)
            : lhs{l}, lhs_revcomp(lr.front() == '-'), rhs{r}, rhs_revcomp(rr.front() == '-'),
              overlap(std::move(o)), tags(std::move(t)) {}

    void print() const {
        std::fputs("L", stdout);
        std::fprintf(stdout, "\t%s\t%c", std::string(lhs).c_str(), lhs_revcomp ? '-' : '+');
        std::fprintf(stdout, "\t%s\t%c", std::string(rhs).c_str(), rhs_revcomp ? '-' : '+');

        std::fputc('\t', stdout);
        if (overlap.empty())
            std::fputc('*', stdout);
        else {
            for (const auto &ovl : overlap)
                ovl.print();
        }

        for (const auto &tag : tags) {
            fputs("\t", stdout);
            tag.print();
        }
    }
};

struct gaplink {
    std::string_view lhs;
    bool lhs_revcomp;
    std::string_view rhs;
    bool rhs_revcomp;
    int64_t distance;
    std::vector<gfa::tag> tags;

    explicit gaplink(std::string_view l, std::string_view lr, std::string_view r, std::string_view rr,
                     int64_t d,
                     std::vector<gfa::tag> t)
            : lhs{l}, lhs_revcomp(lr.front() == '-'),
              rhs{r}, rhs_revcomp(rr.front() == '-'),
              distance{d},
              tags(std::move(t)) {}

    explicit gaplink(std::string_view l, std::string_view lr, std::string_view r, std::string_view rr,
                     std::vector<gfa::tag> t)
            : lhs{l}, lhs_revcomp(lr.front() == '-'),
              rhs{r}, rhs_revcomp(rr.front() == '-'),
              distance{std::numeric_limits<int64_t>::min()},
              tags(std::move(t)) {}

    void print() const {
        std::fputs("J", stdout);
        std::fprintf(stdout, "\t%s\t%c", std::string(lhs).c_str(), lhs_revcomp ? '-' : '+');
        std::fprintf(stdout, "\t%s\t%c", std::string(rhs).c_str(), rhs_revcomp ? '-' : '+');
        std::fprintf(stdout, "\t%" PRId64, distance);

        for (const auto &tag : tags) {
            fputs("\t", stdout);
            tag.print();
        }
    }
};

struct path {
    std::string_view name;
    std::vector<std::string_view> segments;
    std::vector<cigar_string> overlaps;
    std::vector<gfa::tag> tags;

    explicit path(std::string_view n, std::vector<std::string_view> s, std::vector<gfa::tag> t)
            : name{n}, segments(std::move(s)),  tags(std::move(t)) {}

    explicit path(std::string_view n, std::vector<std::string_view> s, std::vector<cigar_string> o, std::vector<gfa::tag> t)
            : name{n}, segments(std::move(s)), overlaps(std::move(o)), tags(std::move(t)) {}

    void print() const {
        std::fputc('P', stdout);
        std::fprintf(stdout, "\t%s\t", std::string(name).c_str());
        for (size_t i = 0; i < segments.size(); ++i) {
            std::fprintf(stdout, "%s", std::string(segments[i]).c_str());
            if ((i+1) < segments.size())
                std::fputc(',', stdout);
        }

        std::fputc('\t', stdout);
        if (overlaps.empty())
            std::fputc('*', stdout);
        else {
            for (size_t i = 0; i < overlaps.size(); ++i) {
                for (const auto &ovl : overlaps[i])
                    ovl.print();
                if ((i+1) < overlaps.size())
                    std::fputc(',', stdout);
            }
        }

        for (const auto &tag : tags) {
            std::fputc('\t', stdout);
            tag.print();
        }
    }
};

using record = std::variant<header, segment, link, gaplink, path>;

std::optional<gfa::record> parseRecord(const char* line, size_t len);
} // namespace gfa
