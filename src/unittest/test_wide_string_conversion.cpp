/*
Minetest
Copyright (C) 2023 Gregor Parzefall

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "test.h"

#include "util/string.h"
#include "irrString.h"

class TestWideStringConversion : public TestBase {
public:
	TestWideStringConversion() { TestManager::registerTestModule(this); }
	const char *getName() { return "TestWideStringConversion"; }

	void runTests(IGameDef *gamedef);

	void testWideStringConversion();
};

static TestWideStringConversion g_test_instance;

void TestWideStringConversion::runTests(IGameDef *gamedef)
{
	TEST(testWideStringConversion);
}

void TestWideStringConversion::testWideStringConversion()
{
	{
		std::wstring std_wide = utf8_to_wide("🪴äöüß🪴");
		std::string std_utf8 = wide_to_utf8(std_wide);
		UASSERT(std_utf8 == "🪴äöüß🪴");
	}
	{
		core::stringw irr_wide;
		core::multibyteToWString(irr_wide, "🪴äöüß🪴");
		core::stringc irr_utf8;
		core::wStringToMultibyte(irr_utf8, irr_wide);
		UASSERT(irr_utf8 == "🪴äöüß🪴");
	}
	{
		core::stringw irr_wide;
		core::multibyteToWString(irr_wide, "🪴äöüß🪴");
		std::string std_utf8 = wide_to_utf8(irr_wide.c_str());
		UASSERT(std_utf8 == "🪴äöüß🪴");
	}
	{
		std::wstring std_wide = utf8_to_wide("🪴äöüß🪴");
		core::stringc irr_utf8;
		core::wStringToMultibyte(irr_utf8, std_wide.c_str());
		UASSERT(irr_utf8 == "🪴äöüß🪴");
	}
}
