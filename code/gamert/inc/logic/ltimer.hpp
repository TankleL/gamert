#pragma once

#include "pre-req.hpp"


class LTimer
{
public:
	LTimer();
	virtual ~LTimer();

public:
	void snapshot();
	float elapsed();

private:
	int64_t		_snapshot;
	int64_t		_freq;
};

