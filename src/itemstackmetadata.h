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

#pragma once

#include "metadata.h"
#include "tool.h"

class Inventory;
class IItemDefManager;

class ItemStackMetadata : public SimpleMetadata
{
	friend class ItemMetadata;

public:
	ItemStackMetadata() : toolcaps_overridden(false) {}

	// Overrides
	void clear() override;
	bool setString(const std::string &name, const std::string &var) override;

	void serialize(std::ostream &os) const;
	void deSerialize(std::istream &is);

	const ToolCapabilities &getToolCapabilities(
			const ToolCapabilities &default_caps) const
	{
		return toolcaps_overridden ? toolcaps_override : default_caps;
	}

	void setToolCapabilities(const ToolCapabilities &caps);
	void clearToolCapabilities();

private:
	void updateToolCapabilities();

	bool toolcaps_overridden;
	ToolCapabilities toolcaps_override;
};

class ItemMetadata : public IMetadata
{
public:
	ItemMetadata(ItemStackMetadata *parent, u32 index);

	void clear() override;
	bool contains(const std::string &name) const override;
	bool setString(const std::string &name, const std::string &var) override;
	const StringMap &getStrings(StringMap *place) const override;
	const std::vector<std::string> &getKeys(std::vector<std::string> *place) const override;

protected:
	const std::string *getStringRaw(const std::string &name, std::string *place) const override;

private:
	ItemStackMetadata *m_parent;
	u32 m_index;
	std::string m_key_prepend;
};
