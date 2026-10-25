// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#pragma once

namespace BlackTek::Build
{
	// True when compiled as part of the Google Test unit test build
	// (config=test; see premake5.lua and BLACKTEK_UNIT_TESTS). Lets any
	// code branch on the unit test build at compile time without relying
	// on the preprocessor macro directly.
#if defined(BLACKTEK_UNIT_TESTS)
	inline constexpr bool IsUnitTestBuild = true;
#else
	inline constexpr bool IsUnitTestBuild = false;
#endif
}
