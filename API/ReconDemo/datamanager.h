#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QString>
#include <vector>
#include <complex>
#include "Interface/Interfaces.h"

namespace Yap{struct IData; }

class DataManager
{
public:
    static DataManager& GetHandle();
    bool Load(const QString& image_path);
/*

    bool ViewPrescan(const std::wstring& pipe);

    bool Save(const QString &raw_data_path, bool save_param = true);
    bool ExportJpeg(Yap::SmartPtr<Yap::IData> data, const QString &jpeg_path);

    bool SetImage(Yap::IData* data);
*/
private:
    DataManager();
    static DataManager s_instance;

    bool LoadFidRecon(const QString& file_path);

    bool DisplayCompile(const QString& file_path, const QString& pipe);

    /*
    bool LoadImage(const std::wstring& file_path);
    bool LoadNiuImage(const std::wstring& file_path);
//    bool LoadFid(const std::wstring& file_path);



    void ToModule(unsigned short * dest, std::complex<float> * data, size_t size);

    bool CheckFiles();
    bool CopyFile(const QString& in_file_path, const QString& out_file_path);


    std::vector<std::pair<Yap::SmartPtr<Yap::IData>, Yap::SmartPtr<Yap::IVariableContainer>>> _images;
    Yap::SmartPtr<Yap::IVariableContainer> _properties;
    */
};

#endif // DATAMANAGER_H
