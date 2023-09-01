/*
Minetest
Copyright (C) 2017-8 rubenwardy <rw@rubenwardy.com>

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


#include "itemstackmetadata.h"
#include "util/serialize.h"
#include "util/strfnd.h"
#include <algorithm>

#define DESERIALIZE_START '\x01'
#define DESERIALIZE_KV_DELIM '\x02'
#define DESERIALIZE_PAIR_DELIM '\x03'
#define DESERIALIZE_START_STR "\x01"
#define DESERIALIZE_KV_DELIM_STR "\x02"
#define DESERIALIZE_PAIR_DELIM_STR "\x03"

#define TOOLCAP_KEY "tool_capabilities"

void ItemStackMetadata::clear()
{
	SimpleMetadata::clear();
	updateToolCapabilities();
}

static void sanitize_string(std::string &str)
{
	str.erase(std::remove(str.begin(), str.end(), DESERIALIZE_START), str.end());
	str.erase(std::remove(str.begin(), str.end(), DESERIALIZE_KV_DELIM), str.end());
	str.erase(std::remove(str.begin(), str.end(), DESERIALIZE_PAIR_DELIM), str.end());
}

bool ItemStackMetadata::setString(const std::string &name, const std::string &var)
{
	std::string clean_name = name;
	std::string clean_var = var;
	sanitize_string(clean_name);
	sanitize_string(clean_var);

	bool result = SimpleMetadata::setString(clean_name, clean_var);
	if (clean_name == TOOLCAP_KEY)
		updateToolCapabilities();
	return result;
}

void ItemStackMetadata::serialize(std::ostream &os) const
{
	std::ostringstream os2(std::ios_base::binary);
	os2 << DESERIALIZE_START;
	for (const auto &stringvar : m_stringvars) {
		if (!stringvar.first.empty() || !stringvar.second.empty())
			os2 << stringvar.first << DESERIALIZE_KV_DELIM
				<< stringvar.second << DESERIALIZE_PAIR_DELIM;
	}
	os << serializeJsonStringIfNeeded(os2.str());
}

void ItemStackMetadata::deSerialize(std::istream &is)
{
	std::string in = deSerializeJsonStringIfNeeded(is);

	m_stringvars.clear();

	if (!in.empty()) {
		if (in[0] == DESERIALIZE_START) {
			Strfnd fnd(in);
			fnd.to(1);
			while (!fnd.at_end()) {
				std::string name = fnd.next(DESERIALIZE_KV_DELIM_STR);
				std::string var  = fnd.next(DESERIALIZE_PAIR_DELIM_STR);
				m_stringvars[name] = var;
			}
		} else {
			// BACKWARDS COMPATIBILITY
			m_stringvars[""] = in;
		}
	}
	updateToolCapabilities();
}

void ItemStackMetadata::updateToolCapabilities()
{
	if (contains(TOOLCAP_KEY)) {
		toolcaps_overridden = true;
		toolcaps_override = ToolCapabilities();
		std::istringstream is(getString(TOOLCAP_KEY));
		toolcaps_override.deserializeJson(is);
	} else {
		toolcaps_overridden = false;
	}
}

void ItemStackMetadata::setToolCapabilities(const ToolCapabilities &caps)
{
	std::ostringstream os;
	caps.serializeJson(os);
	setString(TOOLCAP_KEY, os.str());
}

void ItemStackMetadata::clearToolCapabilities()
{
	setString(TOOLCAP_KEY, "");

}

ItemMetadata::ItemMetadata(ItemStackMetadata *parent, u32 index) :
		m_parent(parent), m_index(index)
{
	m_key_prepend = "ItemMetadata_" + std::to_string(m_index) + "_";
}

void ItemMetadata::clear()
{
	std::vector<std::string> keys;
	m_parent->getKeys(&keys);

	for (const auto &key : keys) {
		if (key.rfind(m_key_prepend, 0) == 0) {
			m_parent->removeString(key);
		}
	}
}

bool ItemMetadata::contains(const std::string &name) const
{
	return m_parent->contains(m_key_prepend + name);
}

bool ItemMetadata::setString(const std::string &name, const std::string &var)
{
	return m_parent->setString(m_key_prepend + name, var);
}

const StringMap &ItemMetadata::getStrings(StringMap *place) const
{
	StringMap strings;
	strings = m_parent->getStrings(&strings);

	place->clear();
	for (const auto &[name, var] : strings) {
		if (name.rfind(m_key_prepend, 0) == 0) {
			place->insert(std::pair(name.substr(m_key_prepend.length()), var));
		}
	}
	return *place;
}

const std::vector<std::string> &ItemMetadata::getKeys(std::vector<std::string> *place) const
{
	std::vector<std::string> keys;
	keys = m_parent->getKeys(&keys);

	place->clear();
	for (const auto &key : keys) {
		if (key.rfind(m_key_prepend, 0) == 0) {
			place->push_back(key.substr(m_key_prepend.length()));
		}
	}
	return *place;
}

const std::string *ItemMetadata::getStringRaw(const std::string &name, std::string *place) const
{
	return m_parent->getStringRaw(m_key_prepend + name, place);
}
