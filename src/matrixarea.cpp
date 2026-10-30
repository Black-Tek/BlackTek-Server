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
        if (rows == 0 or vec.empty() or vec.size() % rows != 0)
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
