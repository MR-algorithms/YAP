/********************************************************************
	Filename: 	EcnuRawDataFile.h
	Created:	2010/12/11
	Author:		Xie Haibin, hbxie@phy.ecnu.edu.cn
				Yang Guang, gyang@phy.ecnu.edu.cn
				
	Purpose:	

	All right reserved. 2009 -

	Shanghai Key Laboratory of Magnetic Resonance, 
	East China Normal University
*********************************************************************/

#ifndef EcnuRawDataFile_h__20101211
#define EcnuRawDataFile_h__20101211

#pragma once
#pragma warning(disable:4290)

#include <string>


namespace Hardware
{
	/// This structure is used to store version info and sizes of all sections.
	struct EcnuRawSections
	{
		double  FileVersion;
		int  Section1Size;
		int  Section2Size;
		int  Section3Size;
		int  Section4Size;
	};

    class  CEcnuRawDataFile
	{
	public:
        static float* Read(const char* file_path, unsigned int &width, unsigned int &height, unsigned int &slices, unsigned int &dim4);
	};
};


#endif // EcnuRawDataFile_h__20101211
