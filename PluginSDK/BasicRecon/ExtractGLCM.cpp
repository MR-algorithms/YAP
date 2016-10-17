#include "stdafx.h"
#include "ExtractGLCM.h"


Yap::ExtractGLCM::ExtractGLCM():
	ProcessorImpl(L"ExtractGLCM")
{
}

Yap::ExtractGLCM::ExtractGLCM(const ExtractGLCM & rh):
	ProcessorImpl(rh)
{
}

Yap::ExtractGLCM::~ExtractGLCM()
{
}

bool Yap::ExtractGLCM::Input(const wchar_t * name, IData * data)
{
	if (data == nullptr)
		return false;
	if ()
}

Yap::IProcessor * Yap::ExtractGLCM::Clone()
{

}

bool Yap::ExtractGLCM::OnInit()
{

}
Yap::IData * Yap::ExtractGLCM::GLCM(IData * data)
{
	if (data == nullptr)
		return nullptr;

}