/**
 * BlackTek Server Engine
 * https://github.com/Black-Tek/BlackTek-Server
 *
 * ============================================================================
 * COPYRIGHT & ATTRIBUTION NOTICE
 * ============================================================================
 *
 * Copyright (C) 2024-2026 BlackTek (Author: Codinablack)
 *
 * This file is an original BlackTek contribution, created since this project's
 * inception, and contains no code inherited from The Forgotten Server.
 * It is licensed solely under the MIT License (see the licensing/License file
 * in the project root for the full text).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <vector>
#include <cstdint>
#include <utility>

namespace BlackTek
{

    class MatrixArea
    {
        using Center = std::pair<uint32_t, uint32_t>;
        using Storage = std::vector<uint8_t>;

    public:
        struct View
        {
            uint8_t* data;
            uint32_t rows, cols;
            View(uint8_t* d, uint32_t r, uint32_t c) : data(d), rows(r), cols(c) {}
            uint8_t& operator[](uint32_t row, uint32_t col) { return data[row * cols + col]; }
        };

        struct ConstView
        {
            const uint8_t* data;
            uint32_t rows, cols;
            ConstView(const uint8_t* d, uint32_t r, uint32_t c) : data(d), rows(r), cols(c) {}
            const uint8_t& operator[](uint32_t row, uint32_t col) const { return data[row * cols + col]; }
        };

        MatrixArea() = default;
        MatrixArea(uint32_t rows, uint32_t cols) : storage(rows * cols, 0), rows{rows}, cols{cols} {}

        bool operator()(uint32_t row, uint32_t col) const { return GetView()[row, col] != 0; }
        uint8_t& operator()(uint32_t row, uint32_t col) { return GetView()[row, col]; }

        View GetView() { return View{storage.data(), rows, cols}; }
        ConstView GetView() const { return ConstView{storage.data(), rows, cols}; }

        void SetCenter(uint32_t y, uint32_t x) { center = {x, y}; }
        const Center& GetCenter() const { return center; }

        uint32_t GetRows() const { return rows; }
        uint32_t GetCols() const { return cols; }

        [[nodiscard]] MatrixArea Rotate90()  const;
        [[nodiscard]] MatrixArea Rotate180() const;
        [[nodiscard]] MatrixArea Rotate270() const;

        operator bool() const { return rows == 0 or cols == 0; }

    private:
        MatrixArea(Center center, uint32_t rows, uint32_t cols, Storage&& storage)
            : storage{std::move(storage)}, center{std::move(center)}, rows{rows}, cols{cols}
        {}

        Storage storage = {};
        Center center = {};
        uint32_t rows = 0;
        uint32_t cols = 0;
    };

MatrixArea CreateArea(const std::vector<uint32_t>& vec, uint32_t rows);

}