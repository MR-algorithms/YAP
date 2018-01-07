#pragma once
#include "rawdatareader.h"

class CEcnuRawDataReader :
	public Hardware::CRawDataReader
{
public:
	explicit CEcnuRawDataReader();
	virtual ~CEcnuRawDataReader(void);
    virtual bool Prepare(const std::string& info) override;

    virtual float* ReadAllData(RawDataInfo* pid, const std::vector<std::string>& files) override;
    virtual float* ReadChannelData(RawDataInfo* pid, const std::string& file_path) override;
};

