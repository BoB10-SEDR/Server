#pragma once
#include "stdafx.h"
#include "CDatabase.h"
#include "CMessage.h"
#include "DataPattern.h"

template <typename ROUTER>
class CApi
{
public:
	CDatabase dbcon;
	CApi() :dbcon("192.168.181.134", "bob", "bob10-sedr12!@", "3306", "hurryup_sedr") {}
	virtual void Routing(ROUTER &router) = 0;
};
