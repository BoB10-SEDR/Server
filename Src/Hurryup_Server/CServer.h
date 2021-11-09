#pragma once
#include "stdafx.h"

class CServer
{
public:
	virtual void Init() = 0;
	virtual void Start() = 0;
	virtual void End() = 0;
	virtual void SetupRoutes() = 0;
};

