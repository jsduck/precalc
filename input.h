#pragma once

#include "defines.h"

#include "reagentdb.h"

#define INPUT "input.txt"

class Input
{
	struct IOdata
	{
		std::vector<std::string> els;
		ReagentDB rdb;
	} _self;

public:
	Input() = default;
	Input(std::string f);

	void parse(std::string f);

	const IOdata& operator()() const;
};