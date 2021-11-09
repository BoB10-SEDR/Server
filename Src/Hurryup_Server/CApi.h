#pragma once
#include "stdafx.h"
#include "CDatabase.h"
#include "CMessage.h"

template <typename ROUTER>
class CApi
{
public:
	CDatabase dbcon;
	CApi() :dbcon("14.138.200.178", "bob", "bob10-sedr12!@", "33333", "bob10_sedr") {}
	virtual void Routing(ROUTER &router) = 0;
};
