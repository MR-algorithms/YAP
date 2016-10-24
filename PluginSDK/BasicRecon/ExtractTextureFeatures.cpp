#include "stdafx.h"
#include "ExtractTextureFeatures.h"


Yap::ExtractTextureFeatures::ExtractTextureFeatures(): ProcessorImpl(L"ExtractTextureFeatures")
{
}


Yap::ExtractTextureFeatures::~ExtractTextureFeatures()
{
}

Yap::IProcessor * Yap::ExtractTextureFeatures::Clone()
{
	return new ExtractTextureFeatures(*this);
}

bool Yap::ExtractTextureFeatures::OnInit()
{
	return false;
}

bool Yap::ExtractTextureFeatures::Input(const wchar_t * name, IData * data)
{
	
	return false;
}
