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

char*  FingerTpl::GetPushTime(){
	return push_time;
}

void FingerTpl::SetPushTime(char* push_time){
	this->push_time = new char[strlen(push_time) + 1];
	memcpy(this->push_time, push_time, strlen(push_time) + 1);
}


char*  FingerTpl::GetSign(){
	return sign;
}

void FingerTpl::SetSign(char* sign){
	this->sign = new char[strlen(sign) + 1];
	memcpy(this->sign, sign, strlen(sign) + 1);
}



