// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

template<typename T>
class intrusive_ptr
{
	T* p_ = nullptr;

	void Add()  noexcept { if (p_) intrusive_ptr_add_ref(p_); }
	void Drop() noexcept { if (p_) intrusive_ptr_release(p_); }

public:
	using element_type = T;

	intrusive_ptr() noexcept = default;

	explicit intrusive_ptr(T* p, bool addRef = true) noexcept
		: p_(p)
	{
		if (addRef) Add();
	}

	intrusive_ptr(const intrusive_ptr& other) noexcept
		: p_(other.p_)
	{
		Add();
	}

	intrusive_ptr(intrusive_ptr&& other) noexcept
		: p_(other.p_)
	{
		other.p_ = nullptr;
	}

	~intrusive_ptr() noexcept { Drop(); }

	intrusive_ptr& operator=(const intrusive_ptr& other) noexcept
	{
		if (p_ != other.p_)
		{
			Drop();
			p_ = other.p_;
			Add();
		}
		return *this;
	}

	intrusive_ptr& operator=(intrusive_ptr&& other) noexcept
	{
		if (p_ != other.p_)
		{
			Drop();
			p_ = other.p_;
			other.p_ = nullptr;
		}
		return *this;
	}

	// Release ownership without decrementing the reference count.
	// The caller becomes responsible for managing the object's lifetime.
	[[nodiscard]] T* release() noexcept
	{
		T* tmp = p_;
		p_ = nullptr;
		return tmp;
	}

	void reset() noexcept { Drop(); p_ = nullptr; }

	[[nodiscard]] T& operator*()  const noexcept { return *p_; }
	[[nodiscard]] T* operator->() const noexcept { return p_; }
	[[nodiscard]] T* Get()        const noexcept { return p_; }
	[[nodiscard]] T* get()        const noexcept { return p_; }
	explicit operator bool()         const noexcept { return p_ != nullptr; }

	bool operator==(const intrusive_ptr& other) const noexcept { return p_ == other.p_; }
	bool operator!=(const intrusive_ptr& other) const noexcept { return p_ != other.p_; }
};