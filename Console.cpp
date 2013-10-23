#include <sstream>
#include <string>

#include "Console.h"
#include "ScriptEngine.h"
#include "Helpers.h"
#include "D2Ptrs.h"
#include "Core.h"

bool Console::visible = false;
bool Console::enabled = false;
std::deque<std::string> Console::history = std::deque<std::string>();
std::deque<std::string> Console::lines = std::deque<std::string>();
std::deque<std::string> Console::commands = std::deque<std::string>();
std::stringstream Console::cmd = std::stringstream();
unsigned int Console::lineCount = 14;
unsigned int Console::commandPos = 0;
unsigned int Console::height = 0;
unsigned int Console::scrollIndex = 0;

void Console::AddKey(unsigned int key)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	cmd << (char)key;
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ExecuteCommand(void)
{
	if(cmd.str().length() < 1)
		return;

	commands.push_back(cmd.str());
	commandPos = commands.size();

	ProcessCommand(cmd.str().c_str(), true);

	cmd.str("");
}

void Console::RemoveLastKey(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	int len = cmd.str().length()-1;
	if(len >= 0)
	{
		cmd.str(cmd.str().substr(0, len));
		if(len > 0)
		{
			cmd.seekg(len);
			cmd.seekp(len);
		}
	}
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::PrevCommand(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);

	if(commandPos < 1 || commandPos > commands.size())
	{
		cmd.str("");
	}
	else
	{
		if(commandPos >= 1)
			commandPos--;
		cmd.str("");
		cmd << commands[commandPos];
	}

	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::NextCommand(void)
{
	if(commandPos >= commands.size())
		return;

	EnterCriticalSection(&Vars.cConsoleSection);

	cmd.str("");
	cmd << commands[commandPos];

	if (commandPos < commands.size() - 1)
		commandPos++;

	LeaveCriticalSection(&Vars.cConsoleSection);
}
void Console::ScrollUp(void)
{
	if(scrollIndex == 0 || history.size() - scrollIndex ==0)
		return;
	EnterCriticalSection(&Vars.cConsoleSection);
	scrollIndex--;
	Console::UpdateLines();
	
	LeaveCriticalSection(&Vars.cConsoleSection);
}
void Console::ScrollDown(void)
{
	if(history.size() < lineCount || (history.size()-lineCount == scrollIndex))
		return;

	EnterCriticalSection(&Vars.cConsoleSection);
	scrollIndex++;
	Console::UpdateLines();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::AddLine(std::string line)
{
	EnterCriticalSection(&Vars.cConsoleSection);


	// add the new line to the list
	history.push_back(line);

	while(history.size() > 300) //set history cap at 300
		history.pop_front();

	if(Vars.bLogConsole)
		LogNoFormat(const_cast<char*>(line.c_str()));

	scrollIndex =history.size() < lineCount ? 0 : history.size() - lineCount ;
	Console::UpdateLines();
	
	LeaveCriticalSection(&Vars.cConsoleSection);
}
void Console::UpdateLines(void)
{
//while(lines.size() > lineCount)
//		lines.pop_front();
	lines.clear();
	unsigned int lin =0;
	for(int j = history.size()-scrollIndex ; j>0; j--){
		lines.push_back(history.at(history.size()-j));
		lin++;
		if (lin > lineCount) break;
	}

}
void Console::Clear(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	lines.clear();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::Toggle(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	ToggleBuffer();
	TogglePrompt();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::TogglePrompt(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	if(!IsEnabled())
		ShowPrompt();
	else
		HidePrompt();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ToggleBuffer(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	if(!IsVisible())
		ShowBuffer();
	else
		HideBuffer();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::Hide(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	HidePrompt();
	HideBuffer();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::HidePrompt(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	enabled = false;
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::HideBuffer(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	visible = false;
	if(IsEnabled())
		HidePrompt();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::Show(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	ShowBuffer();
	ShowPrompt();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ShowPrompt(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	enabled = true;
	if(!IsVisible())
		ShowBuffer();
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::ShowBuffer(void)
{
	EnterCriticalSection(&Vars.cConsoleSection);
	visible = true;
	LeaveCriticalSection(&Vars.cConsoleSection);
}

void Console::Draw(void)
{
	static DWORD count = 0;
	EnterCriticalSection(&Vars.cConsoleSection);
	if(IsVisible())
	{
		POINT size = GetScreenSize();
		int xsize = size.x;
		int ysize = size.y;
		// the default console height is 30% of the screen size
		int height = (int)(((double)ysize)*.3);
		size = CalculateTextLen(">", 0);
		int charsize = size.x;
		int charheight = size.y-10;
		int cx = charsize+9;
		int linelen = 115;
		Console::height = height;

		int cmdsize = CalculateTextLen(cmd.str().c_str(), Vars.dwConsoleFont).x;
		if((cmdsize/xsize) > 0)
			Console::height += (cmdsize/xsize)*charheight + 1;

		// draw the box large enough to hold the whole thing
		D2GFX_DrawRectangle(0, 0, xsize, Console::height, 0xdf, 0);

		std::deque<std::string>::reverse_iterator it = lines.rbegin();
		for(int i = lineCount-1; i >= 0 && it != lines.rend(); i--, it++)
		{
			if(CalculateTextLen(it->c_str(), 0).x > (xsize - (2+charheight)*2))
			{
				std::list<std::string> buf;
				SplitLines(*it, linelen, ' ', buf);
				for(std::list<std::string>::iterator it2 = buf.begin(); it2 != buf.end(); it2++)
					myDrawText(it2->c_str(), 2+charheight, 2+charheight+((i--)*charheight), 0, Vars.dwConsoleFont);
			} else
				myDrawText(it->c_str(), 2+charheight, 2+charheight+(i*charheight), 0, Vars.dwConsoleFont);
		}

		if(IsEnabled())
		{
			myDrawText(">", 1, height, 0, 0);
			int lx = cx + cmdsize - charsize + 5,
				ly = height-charheight;
			if(count % 30)
				D2GFX_DrawLine(lx, ly, lx, height-2, 0xFF, 0xFF);

			std::string cmdstr = cmd.str();
			if(cmdstr.length() > 0)
			{
				if((cmdsize/xsize) > 0)
				{
					std::list<std::string> lines;
					SplitLines(cmdstr, linelen, ' ', lines);
					int i = 0;
					for(std::list<std::string>::iterator it2 = lines.begin(); it2 != lines.end(); it2++, i++)
						myDrawText(it2->c_str(), charsize+5, height+(charheight*i)+1, 0, Vars.dwConsoleFont);
				} else
					myDrawText(cmdstr.c_str(), charsize+5, height+1, 0, Vars.dwConsoleFont);
			}
		}
	}
	LeaveCriticalSection(&Vars.cConsoleSection);

	count++;
}

unsigned int Console::GetHeight(void)
{
	return height;
}
