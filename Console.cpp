#include <sstream>
#include <string>

#include "Console.h"
#include "ScriptEngine.h"
#include "Helpers.h"
#include "D2Ptrs.h"
#include "Core.h"

bool Console::visible = false;
bool Console::enabled = false;
std::deque<std::wstring> Console::history = std::deque<std::wstring>();
std::deque<std::wstring> Console::lines = std::deque<std::wstring>();
std::deque<std::wstring> Console::commands = std::deque<std::wstring>();
std::wstringstream Console::cmd = std::wstringstream();
unsigned int Console::lineCount = 14;
unsigned int Console::lineWidth = 625;
unsigned int Console::commandPos = 0;
unsigned int Console::height = 0;
unsigned int Console::scrollIndex = 0;

void Console::AddKey(unsigned int key) {
    EnterCriticalSection(&Vars.cConsoleSection);
    cmd << (char)key;
    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ExecuteCommand(void) {
    if (cmd.str().length() < 1)
        return;

    commands.push_back(cmd.str());
    commandPos = commands.size();
    char* cmdtxt = UnicodeToAnsi(cmd.str().c_str());
    ProcessCommand(cmdtxt, true);
    delete[] cmdtxt;

    cmd.str(L"");
}

void Console::RemoveLastKey(void) {
    EnterCriticalSection(&Vars.cConsoleSection);
    int len = cmd.str().length() - 1;
    if (len >= 0) {
        cmd.str(cmd.str().substr(0, len));
        if (len > 0) {
            cmd.seekg(len);
            cmd.seekp(len);
        }
    }
    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::PrevCommand(void) {
    EnterCriticalSection(&Vars.cConsoleSection);

    if (commandPos < 1 || commandPos > commands.size()) {
        cmd.str(L"");
    } else {
        if (commandPos >= 1)
            commandPos--;
        cmd.str(L"");
        cmd << commands[commandPos];
    }

    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::NextCommand(void) {
    if (commandPos >= commands.size())
        return;

    EnterCriticalSection(&Vars.cConsoleSection);

    cmd.str(L"");
    cmd << commands[commandPos];

    if (commandPos < commands.size() - 1)
        commandPos++;

    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ScrollUp(void) {
    if (scrollIndex == 0 || history.size() - scrollIndex == 0)
        return;
    EnterCriticalSection(&Vars.cConsoleSection);
    scrollIndex--;
    Console::UpdateLines();

    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ScrollDown(void) {
    if (history.size() < lineCount || (history.size() - lineCount == scrollIndex))
        return;

    EnterCriticalSection(&Vars.cConsoleSection);
    scrollIndex++;
    Console::UpdateLines();
    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::AddLine(std::string line) {
    EnterCriticalSection(&Vars.cConsoleSection);
    wchar_t* linetxt = AnsiToUnicode(line.c_str());
    std::wstring ln(linetxt);
    delete[] linetxt;
    std::list<std::wstring> buf;
    SplitLines(ln, Console::MaxWidth(), ' ', buf);
    for (std::list<std::wstring>::iterator it2 = buf.begin(); it2 != buf.end(); it2++) {
        history.push_back(*it2);
    }

    while (history.size() > 300) // set history cap at 300
        history.pop_front();

    if (Vars.bLogConsole)
        LogNoFormat(const_cast<char*>(line.c_str()));

    scrollIndex = history.size() < lineCount ? 0 : history.size() - lineCount ;
    Console::UpdateLines();

    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::UpdateLines(void) {
    lines.clear();
    for (int j = history.size() - scrollIndex; j > 0 && lines.size() < lineCount; j--)
        lines.push_back(history.at(history.size() - j));
}

void Console::Clear(void) {
    EnterCriticalSection(&Vars.cConsoleSection);
    lines.clear();
    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::Toggle(void) {
    EnterCriticalSection(&Vars.cConsoleSection);
    ToggleBuffer();
    TogglePrompt();
    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::TogglePrompt(void) {
    EnterCriticalSection(&Vars.cConsoleSection);
    if (!IsEnabled())
        ShowPrompt();
    else
        HidePrompt();
    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ToggleBuffer(void) {
    EnterCriticalSection(&Vars.cConsoleSection);
    if (!IsVisible())
        ShowBuffer();
    else
        HideBuffer();
    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::Hide(void) {
    EnterCriticalSection(&Vars.cConsoleSection);
    HidePrompt();
    HideBuffer();
    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::HidePrompt(void) {
    EnterCriticalSection(&Vars.cConsoleSection);
    enabled = false;
    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::HideBuffer(void) {
    EnterCriticalSection(&Vars.cConsoleSection);
    visible = false;
    if (IsEnabled())
        HidePrompt();
    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::Show(void) {
    EnterCriticalSection(&Vars.cConsoleSection);
    ShowBuffer();
    ShowPrompt();
    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ShowPrompt(void) {
    EnterCriticalSection(&Vars.cConsoleSection);
    enabled = true;
    if (!IsVisible())
        ShowBuffer();
    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ShowBuffer(void) {
    EnterCriticalSection(&Vars.cConsoleSection);
    visible = true;
    LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::Draw(void) {
    static DWORD count = GetTickCount();
    EnterCriticalSection(&Vars.cConsoleSection);
    if (IsVisible()) {
        POINT size = GetScreenSize();
        int xsize = size.x;
        int ysize = size.y;
        size = CalculateTextLen("@", Vars.dwConsoleFont);
        int charwidth = size.x;
        int charheight = max(12, size.y / 2 + 2);
        // the default console height is 30% of the screen size
        int height = ((int)(((double)ysize) * .3) / charheight) * charheight + charheight;
        lineWidth = xsize - (2 * charwidth);
        lineCount = height / charheight;

        int cmdsize = 0;
        int cmdlines = 0;
        std::list<std::wstring> cmdsplit;
        if (IsEnabled()) {
            std::wstring cmdstr = cmd.str();
            if (cmdstr.length() > 0) {
                SplitLines(cmdstr, Console::MaxWidth(), ' ', cmdsplit);
                cmdsize = CalculateTextLen(cmdsplit.back().c_str(), Vars.dwConsoleFont).x;
                cmdlines += cmdsplit.size() - 1;
            }
        }

        Console::height = height + (cmdlines * charheight) + 6;
        // draw the box large enough to hold the whole thing
        D2GFX_DrawRectangle(0, 0, xsize, Console::height, 0xdf, 0);

        std::deque<std::wstring>::reverse_iterator it = lines.rbegin();
        if (scrollIndex == 0 && lines.size() == lineCount && IsEnabled()) // handle index 0, top of console
            it++;

        for (int i = lineCount - (int)IsEnabled(); i > 0 && it != lines.rend(); i--, it++)
            myDrawText(it->c_str(), charwidth, 4 + (i * charheight), 0, Vars.dwConsoleFont);

        if (IsEnabled()) {
            if (cmdsplit.size() > 0) {
                int dy = height + 3;
                for (std::list<std::wstring>::iterator it2 = cmdsplit.begin(); it2 != cmdsplit.end(); it2++, dy += charheight)
                    myDrawText(it2->c_str(), charwidth, dy, 0, Vars.dwConsoleFont);
            }

            myDrawText(L">", 1, Console::height - 3, 0, Vars.dwConsoleFont);
            DWORD tick = GetTickCount();
            if ((tick - count) < 600) {
                int lx = cmdsize + charwidth, ly = Console::height - (charheight / 3);
                D2GFX_DrawRectangle(lx, ly, lx + ((charwidth * 2) / 3), ly + 2, 0xFF, 0x07);
            } else if ((tick - count) > 1100)
                count = tick;
        }
    }
    LeaveCriticalSection(&Vars.cConsoleSection);
}

unsigned int Console::GetHeight(void) {
    return height;
}
