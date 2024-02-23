/*
Minetest
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>

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

#include "version.h"
#include "config.h"
#include "porting.h"
#include "IrrCompileConfig.h"

#if USE_CMAKE_CONFIG_H
	#include "cmake_config_githash.h"
#endif

extern "C" {
#if USE_LUAJIT
	#include <luajit.h>
#else
	#include <lua.h>
#endif
}

#ifndef VERSION_GITHASH
	#define VERSION_GITHASH VERSION_STRING
#endif

#define STRINGIFY(x) #x
#define STR(x) STRINGIFY(x)

const char *g_version_string = VERSION_STRING;
const char *g_version_hash = VERSION_GITHASH;
const char *g_build_info =
	"BUILD_TYPE=" BUILD_TYPE "\n"
	"RUN_IN_PLACE=" STR(RUN_IN_PLACE) "\n"
	"USE_CURL=" STR(USE_CURL) "\n"
#ifndef SERVER
	"USE_GETTEXT=" STR(USE_GETTEXT) "\n"
	"USE_SOUND=" STR(USE_SOUND) "\n"
#endif
	"STATIC_SHAREDIR=" STR(STATIC_SHAREDIR)
#if USE_GETTEXT && defined(STATIC_LOCALEDIR)
	"\n" "STATIC_LOCALEDIR=" STR(STATIC_LOCALEDIR)
#endif
;

void write_version(std::ostream &os)
{
	os << PROJECT_NAME_C " " << g_version_hash
		<< " (" << porting::getPlatformName() << ")" << std::endl;
#ifndef SERVER
	os << "Using Irrlicht " IRRLICHT_SDK_VERSION << std::endl;
#endif
#if USE_LUAJIT
	os << "Using " << LUAJIT_VERSION << std::endl;
#else
	os << "Using " << LUA_RELEASE << std::endl;
#endif
#if defined(__clang__)
	os << "Built by Clang " << __clang_major__ << "." << __clang_minor__ << std::endl;
#elif defined(__GNUC__)
	os << "Built by GCC " << __GNUC__ << "." << __GNUC_MINOR__ << std::endl;
#elif defined(_MSC_VER)
	os << "Built by MSVC " << (_MSC_VER / 100) << "." << (_MSC_VER % 100) << std::endl;
#endif
	os << "Running on " << porting::get_sysinfo() << std::endl;
	os << g_build_info << std::endl;
}
