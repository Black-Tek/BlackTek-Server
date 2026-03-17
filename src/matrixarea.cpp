// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"
#include "matrixarea.h"

namespace BlackTek
{

    MatrixArea MatrixArea::Rotate90() const
    {
        Storage newStorage(storage.size(), 0);
        View dst{newStorage.data(), cols, rows};
        auto src = GetView();

        for (uint32_t i = 0; i < rows; ++i)
        {
            for (uint32_t j = 0; j < cols; ++j)
            {
                dst[j, rows - 1 - i] = src[i, j];
            }
        }

        const auto [cx, cy] = center;
        return {{rows - cy - 1, cx}, cols, rows, std::move(newStorage)};
    }

    MatrixArea MatrixArea::Rotate180() const
    {
        Storage newStorage(storage.size());
        std::ranges::reverse_copy(storage, newStorage.begin());

        const auto [cx, cy] = center;
        return {{cols - cx - 1, rows - cy - 1}, rows, cols, std::move(newStorage)};
    }

    MatrixArea MatrixArea::Rotate270() const
    {
        // Clockwise 270°: dst[i, j] = src[j, cols-1-i]
        // Produces a (cols × rows) matrix from a (rows × cols) source.
        Storage newStorage(storage.size(), 0);
        View dst{newStorage.data(), cols, rows};
        auto src = GetView();

        for (uint32_t i = 0; i < cols; ++i)
        {
            for (uint32_t j = 0; j < rows; ++j)
            {
                dst[i, j] = src[j, cols - 1 - i];
            }
        }

        const auto [cx, cy] = center;
        return {{cy, cols - cx - 1}, cols, rows, std::move(newStorage)};
    }

    MatrixArea CreateArea(const std::vector<uint32_t>& vec, const uint32_t rows)
    {
        if (rows == 0 or vec.empty())
        {
            return MatrixArea{};
        }

        const uint32_t cols = static_cast<uint32_t>(vec.size() / rows);
        if (cols == 0)
        {
            return MatrixArea{};
        }

        MatrixArea area{rows, cols};

        for (uint32_t idx = 0; const uint32_t value : vec)
        {
            const uint32_t y = idx / cols;
            const uint32_t x = idx % cols;

            if (value == 1 or value == 3)
            {
                area(y, x) = 1;
            }

            if (value == 2 or value == 3)
            {
                area.SetCenter(y, x);
            }

            ++idx;
        }

        return area;
    }

}