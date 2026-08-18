// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_LOCKFREE_H
#define FS_LOCKFREE_H

#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cstddef>
#include <limits>
#include <new>
#include <utility>

namespace BlackTek::Memory
{
    inline constexpr std::size_t CacheSlot = 64;

    template <std::size_t Align>
    inline void* aligned_new(std::size_t bytes)
    {
        if constexpr (Align > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
            return ::operator new(bytes, std::align_val_t{ Align });

        else
            return ::operator new(bytes);
    }

    template <std::size_t Align>
    inline void aligned_delete(void* p) noexcept
    {
        if constexpr (Align > __STDCPP_DEFAULT_NEW_ALIGNMENT__)
            ::operator delete(p, std::align_val_t{ Align });

        else
            ::operator delete(p);
    }

    template <std::size_t Capacity>
    class FixedPointerQueue
    {
        static_assert(std::has_single_bit(Capacity) and Capacity >= 2, "Capacity must be a power of two and at least 2");

    public:
        FixedPointerQueue() noexcept
        {
            for (std::size_t i = 0; i < Capacity; ++i)
                cells[i].sequence.store(i, std::memory_order_relaxed);
        }

        FixedPointerQueue(const FixedPointerQueue&) = delete;
        FixedPointerQueue& operator=(const FixedPointerQueue&) = delete;

        bool try_push(void* value) noexcept
        {
            std::size_t pos = enqueue_position.load(std::memory_order_relaxed);

            for (;;)
            {
                Cell& cell = cells[pos & Mask];
                const std::size_t seq = cell.sequence.load(std::memory_order_acquire);
                const auto diff = static_cast<std::ptrdiff_t>(seq) - static_cast<std::ptrdiff_t>(pos);

                if (diff == 0)
                {
                    if (enqueue_position.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                    {
                        cell.value = value;
                        cell.sequence.store(pos + 1, std::memory_order_release);
                        return true;
                    }
                }
                else if (diff < 0)
                {
                    return false; // full
                }
                else
                {
                    pos = enqueue_position.load(std::memory_order_relaxed);
                }
            }
        }

        bool try_pop(void*& out) noexcept
        {
            std::size_t pos = dequeue_position.load(std::memory_order_relaxed);

            for (;;)
            {
                Cell& cell = cells[pos & Mask];
                const std::size_t seq = cell.sequence.load(std::memory_order_acquire);
                const auto diff = static_cast<std::ptrdiff_t>(seq) - static_cast<std::ptrdiff_t>(pos + 1);

                if (diff == 0)
                {
                    if (dequeue_position.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                    {
                        out = cell.value;
                        cell.sequence.store(pos + Capacity, std::memory_order_release);
                        return true;
                    }
                }
                else if (diff < 0)
                {
                    return false; // empty
                }
                else
                {
                    pos = dequeue_position.load(std::memory_order_relaxed);
                }
            }
        }

    private:
        struct alignas(CacheSlot) Cell
        {
            std::atomic<std::size_t> sequence;
            void* value;
        };

        static constexpr std::size_t Mask = Capacity - 1;

        alignas(CacheSlot) std::array<Cell, Capacity> cells{};
        alignas(CacheSlot) std::atomic<std::size_t> enqueue_position{ 0 };
        alignas(CacheSlot) std::atomic<std::size_t> dequeue_position{ 0 };
    };

    template <std::size_t BlockSize, std::size_t BlockAlign, std::size_t Capacity>
    class CentralPool
    {
    public:
        static CentralPool& instance()
        {
            static CentralPool& pool = *new CentralPool();
            return pool;
        }

        bool try_pop(void*& out) noexcept { return queue.try_pop(out); }
        bool try_push(void* block) noexcept { return queue.try_push(block); }

        CentralPool(const CentralPool&) = delete;
        CentralPool& operator=(const CentralPool&) = delete;

    private:
        CentralPool() = default;

        FixedPointerQueue<std::bit_ceil(Capacity)> queue;
    };

    template <std::size_t BlockSize, std::size_t BlockAlign, std::size_t Capacity, std::size_t LocalCapacity = 16>
    class ThreadLocalMagazine
    {
    public:
        using Pool = CentralPool<BlockSize, BlockAlign, Capacity>;

        ThreadLocalMagazine() = default;

        ~ThreadLocalMagazine() noexcept
        {
            while (size > 0)
            {
                void* block = storage[--size];

                if (not Pool::instance().try_push(block))
                    aligned_delete<BlockAlign>(block);
            }
        }

        ThreadLocalMagazine(const ThreadLocalMagazine&) = delete;
        ThreadLocalMagazine& operator=(const ThreadLocalMagazine&) = delete;

        [[nodiscard]] void* allocate() noexcept
        {
            if (size > 0) [[likely]]
                return storage[--size];

            constexpr std::size_t BatchSize = LocalCapacity / 2;
            void* block = nullptr;
            auto& pool = Pool::instance();

            for (std::size_t i = 0; i < BatchSize; ++i)
            {
                void* batchBlock = nullptr;

                if (not pool.try_pop(batchBlock))
                    break;

                if (i == 0)
                    block = batchBlock;

                else
                    storage[size++] = batchBlock;
            }

            return block;
        }

        bool deallocate(void* block) noexcept
        {
            if (size < LocalCapacity) [[likely]]
            {
                storage[size++] = block;
                return true;
            }

            constexpr std::size_t FlushCount = LocalCapacity / 2;
            auto& pool = Pool::instance();

            for (std::size_t i = 0; i < FlushCount; ++i)
            {
                if (not pool.try_push(storage[i]))
                    aligned_delete<BlockAlign>(storage[i]);
            }

            std::move(storage.begin() + FlushCount, storage.begin() + size, storage.begin());
            size -= FlushCount;
            storage[size++] = block;
            return true;
        }

    private:
        std::array<void*, LocalCapacity> storage{};
        std::size_t size{ 0 };
    };

}

template <typename T, std::size_t Capacity>
class LockfreePoolingAllocator;


template <std::size_t Capacity>
class LockfreePoolingAllocator<void, Capacity>
{
public:
    using value_type = void;

    template <typename U>
    struct rebind
    {
        using other = LockfreePoolingAllocator<U, Capacity>;
    };

    constexpr LockfreePoolingAllocator() noexcept = default;

    template <typename U>
    constexpr LockfreePoolingAllocator(
        const LockfreePoolingAllocator<U, Capacity>&) noexcept
    {
    }

    template <typename U>
    constexpr bool operator==(
        const LockfreePoolingAllocator<U, Capacity>&) const noexcept
    {
        return true;
    }
};

// todo: move to the above namespace (BlackTek::Memory)
template <typename T, std::size_t Capacity>
class LockfreePoolingAllocator
{
public:
    using value_type = T;

    template <class U>
    struct rebind
    {
        using other = LockfreePoolingAllocator<U, Capacity>;
    };

    LockfreePoolingAllocator() = default;

    template <typename U>
    constexpr LockfreePoolingAllocator(const LockfreePoolingAllocator<U, Capacity>&) noexcept {}

    [[nodiscard]] T* allocate(std::size_t n) const
    {
        if (n != 1) [[unlikely]]
        {
            if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
                throw std::bad_array_new_length();

            return static_cast<T*>(BlackTek::Memory::aligned_new<alignof(T)>(n * sizeof(T)));
        }

        void* block = local_magazine().allocate();

        if (block) [[likely]]
            return static_cast<T*>(block);

        return static_cast<T*>(BlackTek::Memory::aligned_new<alignof(T)>(sizeof(T)));
    }

    void deallocate(T* p, std::size_t n) const noexcept
    {
        if (n != 1) [[unlikely]]
        {
            BlackTek::Memory::aligned_delete<alignof(T)>(p);
            return;
        }

        local_magazine().deallocate(p);
    }

    template <typename U>
    constexpr bool operator==(const LockfreePoolingAllocator<U, Capacity>&) const noexcept
    {
        return true;
    }

private:
    static auto& local_magazine() noexcept
    {
        thread_local BlackTek::Memory::ThreadLocalMagazine<sizeof(T), alignof(T), Capacity> magazine;
        return magazine;
    }
};

#endif // FS_LOCKFREE_H