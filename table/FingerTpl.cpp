#include "../stdafx.h"
#include "FingerTpl.h"


FingerTpl::FingerTpl()
{
}


FingerTpl::~FingerTpl()
{
}

void FingerTpl::SetId(int id)
{
	this->id = id;
}

char* FingerTpl::GetMobile()
{
	return mobile;
}

void FingerTpl::SetMobile(char* mobile){
	this->mobile = new char[strlen(mobile) + 1];
	memcpy(this->mobile, mobile, strlen(mobile) + 1);
}
