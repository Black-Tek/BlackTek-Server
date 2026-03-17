// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

#include <mdspan>
#include <vector>
#include <cstdint>
#include <utility>

namespace BlackTek
{

    class MatrixArea
    {
        using Center = std::pair<uint32_t, uint32_t>;
        using Storage = std::vector<uint8_t>;
        using Extents = std::dextents<uint32_t, 2>;

    public:
        using View = std::mdspan<uint8_t, Extents>;
        using ConstView = std::mdspan<const uint8_t, Extents>;

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