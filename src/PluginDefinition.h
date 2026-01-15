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

#pragma once

#include "PluginInterface.h"
#include "menuCmdID.h"
#include "Notepad_plus_msgs.h"
#include "Scintilla.h"
#include "Sci_Position.h"

// The number of functions in the function array
// (update if you add more menu commands)
constexpr int nbFunc = 1;

// Forward decls for the plugin template
void pluginInit(HANDLE hModule);
void pluginCleanUp();
void commandMenuInit();
void commandMenuCleanUp();

// Helper to register commands
void setCommand(int index, TCHAR* cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey* sk = NULL, bool check0nInit = false);

// Plugin command
void decodeEscapesCommand();
