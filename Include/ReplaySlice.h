#pragma once
class Command;

struct ReplaySlice
{
	int frame;
	Command* command;
};