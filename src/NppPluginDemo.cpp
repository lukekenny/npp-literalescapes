//this file is part of notepad++
//Copyright (C)2021 Don HO <donho@altern.org>
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <https://www.gnu.org/licenses/>.

// This plugin was modified for "npp-literalescapes":
// It adds a Plugins menu command that decodes common literal escape
// sequences in the current selection (or whole document).

#include "PluginDefinition.h"

#include <string>
#include <vector>
#include <cstring>

// The plugin template provides these globals in PluginDefinition.cpp
extern NppData nppData;

namespace
{
	HWND getCurrentScintilla()
	{
		int which = 0;
		::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, reinterpret_cast<LPARAM>(&which));
		return (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;
	}

	static void replaceAll(std::string& s, const std::string& from, const std::string& to)
	{
		if (from.empty()) return;
		size_t pos = 0;
		while ((pos = s.find(from, pos)) != std::string::npos)
		{
			s.replace(pos, from.size(), to);
			pos += to.size();
		}
	}

	// One-step decode: handles either single-backslash or double-backslash forms.
	// Examples in the *document text*:
	//   "\\n" OR "\n" -> LF
	//   "\\t" OR "\t" -> TAB
	//   "\\\"" OR "\"" -> '"'
	//   "\\\\" OR "\\" -> '\\'
	static std::string decodeLiteralEscapes(std::string s)
	{
		// Order matters. Decode the longer (double-backslash) patterns first,
		// then the single-backslash ones.
		//
		// We intentionally do NOT produce CRLF; \n becomes LF (Linux style), as requested.

		replaceAll(s, "\\\\n", "\n");
		replaceAll(s, "\\\\t", "\t");
		replaceAll(s, "\\\\\"", "\"");
		replaceAll(s, "\\\\\\\\", "\\");

		replaceAll(s, "\\n", "\n");
		replaceAll(s, "\\t", "\t");
		replaceAll(s, "\\\"", "\"");
		replaceAll(s, "\\\\", "\\");

		return s;
	}

	static std::string getTextRange(HWND hSci, Sci_Position start, Sci_Position end)
	{
		Sci_TextRangeFull tr{};
		tr.chrg.cpMin = start;
		tr.chrg.cpMax = end;

		const auto len = static_cast<size_t>(end - start);
		std::string buf;
		buf.resize(len + 1); // +1 for NUL

		tr.lpstrText = buf.empty() ? nullptr : &buf[0]; // writable char*
		::SendMessage(hSci, SCI_GETTEXTRANGEFULL, 0, reinterpret_cast<LPARAM>(&tr));

		// SCI_GETTEXTRANGEFULL guarantees NUL-termination; shrink to actual C-string length
		buf.resize(std::strlen(buf.c_str()));
		return buf;
	}

	static void replaceRange(HWND hSci, Sci_Position start, Sci_Position end, const std::string& replacement)
	{
		::SendMessage(hSci, SCI_BEGINUNDOACTION, 0, 0);
		::SendMessage(hSci, SCI_SETTARGETRANGE, start, end);
		::SendMessage(hSci, SCI_REPLACETARGET, static_cast<WPARAM>(replacement.size()), reinterpret_cast<LPARAM>(replacement.c_str()));
		::SendMessage(hSci, SCI_ENDUNDOACTION, 0, 0);
	}
}

void decodeEscapesCommand()
{
	HWND hSci = getCurrentScintilla();
	if (!hSci) return;

	Sci_Position selStart = static_cast<Sci_Position>(::SendMessage(hSci, SCI_GETSELECTIONSTART, 0, 0));
	Sci_Position selEnd = static_cast<Sci_Position>(::SendMessage(hSci, SCI_GETSELECTIONEND, 0, 0));

	Sci_Position start = selStart;
	Sci_Position end = selEnd;

	// If no selection, operate on whole document
	if (selStart == selEnd)
	{
		start = 0;
		end = static_cast<Sci_Position>(::SendMessage(hSci, SCI_GETLENGTH, 0, 0));
	}

	if (end <= start) return;

	std::string original = getTextRange(hSci, start, end);
	std::string decoded = decodeLiteralEscapes(original);

	if (decoded == original) return; // nothing to do

	replaceRange(hSci, start, end, decoded);
}
