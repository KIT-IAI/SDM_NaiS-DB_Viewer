#pragma once

#include <string>
#include <sqlite3.h>

#include "wxprec.hpp"

inline std::string quoteWord(const std::string& str)
{
	return "'" + str + "'";
}

inline std::string searchText(const std::string& str)
{
	return "'%" + str + "%'";
}

inline std::string transform(sqlite3_stmt* stmt, int index)
{
	auto c = sqlite3_column_text(stmt, index);
	if (c == NULL)
		return std::string();
	else
	{
		auto str = std::string(reinterpret_cast<const char*>(c));
		return wxString::FromUTF8(str).ToStdString();
	}
}