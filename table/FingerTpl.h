#pragma once
//https://github.com/Polina-Kachanova/Project-LazyCook-/blob/5a25b14ad89fb4e1bfb9c2786fe3c9e0d47bee24/headers/Object.h
class FingerTpl
{

protected:
	long id;
	char*  mobile;
	char*  fingerindex;
	char*  template_10;
public:
	FingerTpl();
	virtual ~FingerTpl();
	void SetId(int id);
	char* GetMobile();
	void SetMobile(char* mobile);
};

