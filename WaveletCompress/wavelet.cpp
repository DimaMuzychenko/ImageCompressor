#include "wavelet.h"


#include <iostream>
#include <algorithm>


enum class Direction
{
    TopDown,
    LeftRight
};


template<typename T>
std::vector<T> operator+(const std::vector<T>& lhs, const std::vector<T>& rhs)
{
    std::vector<T> result(lhs);
    result.insert(std::end(result), std::begin(rhs), std::end(rhs));
    return result;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& collection)
{
    os << "[ ";
    for (auto& elem : collection)
    {
        os << elem << ' ';
    }
    os << "]";

    return os;
}



std::vector<double> wv::RecursiveDirectTransform1D(const std::vector<double>& data, bool consolLogging)
{
    using namespace std;

    if (data.size() == 1)
    {
        return data;
    }

    vector<double> sArr;
    vector<double> dArr;

    for (size_t i = 0; i < data.size() - 1; i += 2)
    {
        auto a = data[i];
        auto b = data[i + 1];
        sArr.push_back((a + b) / 2.); // s
        dArr.push_back((a - b) / 2.); // d
    }

    if (consolLogging)
    {
        cout << sArr + dArr << endl;
    }
    auto tmp = RecursiveDirectTransform1D(sArr, consolLogging);
    dArr.insert(std::begin(dArr), std::begin(tmp), std::end(tmp));

    return dArr;
}


void wv::DirectTransform1D(std::vector<double>& data, size_t endIdx)
{
    std::vector<double> sArr;
    std::vector<double> dArr;

    for (size_t i = 0; i < endIdx; i += 2)
    {
        auto a = data[i];
        auto b = data[i + 1];
        sArr.push_back((a + b) / sqrt(2.0)); // s
        dArr.push_back((a - b) / sqrt(2.0)); // d
    }

    size_t i = 0;
    size_t j = 0;
    for (; i < endIdx / 2; ++i, ++j)
    {
        data[i] = sArr[j];
    }
    for (j = 0; i < endIdx; ++i, ++j)
    {
        data[i] = dArr[j];
    }
}

void wv::DirectTransform1D(std::vector<double*>& data, size_t endIdx)
{
    std::vector<double> sArr;
    std::vector<double> dArr;

    for (size_t i = 0; i < endIdx - 1; i += 2)
    {
        auto a = *data[i];
        auto b = *data[i + 1];
        sArr.push_back((a + b) / sqrt(2.0)); // s
        dArr.push_back((a - b) / sqrt(2.0)); // d
    }

    size_t i = 0;
    size_t j = 0;
    for (; i < endIdx / 2; ++i, ++j)
    {
        *data[i] = sArr[j];
    }
    for (j = 0; i < endIdx; ++i, ++j)
    {
        *data[i] = dArr[j];
    }
}

void wv::InverseTransform1D(std::vector<double>& data, size_t endIdx)
{
    std::vector<double> restoredArr;

    for (size_t i = 0; i < endIdx / 2; ++i)
    {
        auto s = data[i];
        auto d = data[i + endIdx / 2];
        //restoredArr.push_back((s + d)); // a
        //restoredArr.push_back((s - d)); // b
        double coef = sqrt(2.0) / 2;
        restoredArr.push_back(coef * (s + d)); // a
        restoredArr.push_back(coef * (s - d)); // b
    }

    for (size_t i = 0; i < restoredArr.size(); ++i)
    {
        data[i] = restoredArr[i];
    }
}

void wv::InverseTransform1D(std::vector<double*>& data, size_t endIdx)
{
    std::vector<double> restoredArr;

    for (size_t i = 0; i < endIdx / 2; ++i)
    {
        auto s = *data[i];
        auto d = *data[i + endIdx / 2];
        //restoredArr.push_back((s + d)); // a
        //restoredArr.push_back((s - d)); // b
        double coef = sqrt(2.0) / 2;
        restoredArr.push_back(coef * (s + d)); // a
        restoredArr.push_back(coef * (s - d)); // b
    }

    for (size_t i = 0; i < restoredArr.size(); ++i)
    {
        *data[i] = restoredArr[i];
    }
}


void wv::DirectTransformLeftToRight(std::vector<std::vector<double>>& data, size_t endRow, size_t endCol)
{
    for (size_t i = 0; i < endRow; ++i)
    {
        DirectTransform1D(data[i], endCol);
    }
}

void wv::DirectTransformTopToDown(std::vector<std::vector<double>>& data, size_t endRow, size_t endCol)
{
    for (size_t x = 0; x < endCol; ++x)
    {
        std::vector<double*> column;
        column.reserve(data[0].size());

        for (size_t y = 0; y < endRow; ++y)
        {
            column.push_back(&data[y][x]);
        }

        DirectTransform1D(column, endRow);
    }
}

void wv::InverseTransformLeftToRight(std::vector<std::vector<double>>& data, size_t endRow, size_t endCol)
{
    for (size_t i = 0; i < endRow; ++i)
    {
        InverseTransform1D(data[i], endCol);
    }
}

void wv::InverseTransformTopToDown(std::vector<std::vector<double>>& data, size_t endRow, size_t endCol)
{
    for (size_t x = 0; x < endCol; ++x)
    {
        std::vector<double*> column;
        column.reserve(data[0].size());

        for (size_t y = 0; y < endRow; ++y)
        {
            column.push_back(&data[y][x]);
        }

        InverseTransform1D(column, endRow);
    }
}


void wv::DirectTransform2D(std::vector<std::vector<double>>& data, size_t endRow, size_t endCol)
{
    while (endRow > 1)
    {

        DirectTransformLeftToRight(data, endRow, endCol);        
        endCol /= 2;
        DirectTransformTopToDown(data, endRow, endCol);
        endRow /= 2;
    }
}


void wv::InverseTransform2D(std::vector<std::vector<double>>& data, size_t startRow, size_t startCol)
{
    while (startCol != std::size(data[0]))
    {
        startRow *= 2;
        InverseTransformTopToDown(data, startRow, startCol);
        startCol *= 2;
        InverseTransformLeftToRight(data, startRow, startCol);
    }
}

void wv::DirectTransform2D2(std::vector<std::vector<double>>& data, size_t nIter)
{
    auto endRow = std::size(data);
    auto endCol = std::size(data[0]);

    while (endRow > 1 && nIter > 0)
    {
        DirectTransformLeftToRight(data, endRow, endCol);
        endCol /= 2;
        DirectTransformTopToDown(data, endRow, endCol);
        endRow /= 2;

        --nIter;
    }
}


void wv::InverseTransform2D2(std::vector<std::vector<double>>& data, size_t nIter)
{
    auto height = std::size(data);
    auto startCol = height >> nIter;
    auto startRow = height >> nIter;
    startCol = startCol < 1 ? 1 : startCol;
    startRow = startRow < 1 ? 1 : startRow;

    while (startCol != height)
    {
        startRow *= 2;
        InverseTransformTopToDown(data, startRow, startCol);
        startCol *= 2;
        InverseTransformLeftToRight(data, startRow, startCol);
    }
}

std::vector<double> wv::To1D(const std::vector<std::vector<double>>& data, bool rowByRow)
{
    std::vector<double> retVec;
    retVec.reserve(data.size() * data[0].size());
    if (rowByRow)
    {
        for (const auto& row : data)
        {
            retVec.insert(std::end(retVec), std::begin(row), std::end(row));
        }
    }
    else
    {
        for (size_t x = 0; x < data[0].size(); ++x)
        {
            for (size_t y = 0; y < data.size(); ++y)
            {
                retVec.push_back(data[y][x]);
            }
        }
    }
    return retVec;
}

std::vector<double> wv::To1D(const RGB& rgb, bool rowByRow)
{
    std::vector<double> retVec;
    retVec.reserve(rgb[0].size() * rgb[0].size() * 3);
    
    for (auto& data : rgb)
    {
        if (rowByRow)
        {
            for (const auto& row : data)
            {
                retVec.insert(std::end(retVec), std::begin(row), std::end(row));
            }
        }
        else
        {
            for (size_t x = 0; x < data[0].size(); ++x)
            {
                for (size_t y = 0; y < data.size(); ++y)
                {
                    retVec.push_back(data[y][x]);
                }
            }
        }
    }

    return retVec;
}

std::vector<std::vector<double>> wv::To2D(const std::vector<double>& data, size_t elemInline, bool rowByRow)
{
    using namespace std;
    std::vector<std::vector<double>> retVec;
    retVec.resize(std::size(data) / elemInline);
    for (auto& vec : retVec)
    {
        vec.reserve(std::size(data) / elemInline);
    }

    if (rowByRow)
    {
        auto width = elemInline;
        auto height = std::size(data) / elemInline;
        retVec.resize(height);
        for_each(begin(retVec), end(retVec), [width](auto& row) {row.reserve(width); });

        auto it = begin(data);
        for (auto& row : retVec)
        {
            for (size_t i = 0; i < elemInline; ++i)
            {
                row.push_back(*it++);
            }
        }
    }
    else
    {
        auto width = std::size(data) / elemInline;
        auto height = elemInline;
        retVec.resize(height);
        for_each(begin(retVec), end(retVec), [width](auto& row) {row.resize(width); });

        auto it = begin(data);
        for (size_t y = 0; y < height; ++y)
        {
            for (size_t x = 0; x < width; ++x)
            {
                retVec[y][x] = *it++;
            }
        }
    }

    return retVec;
}

std::vector<std::vector<std::vector<double>>> wv::To3D(const std::vector<double>& data, size_t elemInline, bool rowByRow)
{
    using namespace std;
    RGB retRgb(3);
    auto it = begin(data);
    for (auto& mat : retRgb)
    {
        mat.resize(std::size(data) / 3 / elemInline);
        for (auto& vec : mat)
        {
            vec.reserve(std::size(data) / elemInline);
        }

        if (rowByRow)
        {
            auto width = elemInline;
            auto height = std::size(data) / 3 / elemInline;
            mat.resize(height);
            for_each(begin(mat), end(mat), [width](auto& row) {row.reserve(width); });

            for (auto& row : mat)
            {
                for (size_t i = 0; i < elemInline; ++i)
                {
                    row.push_back(*it++);
                }
            }
        }
        else
        {
            auto width = std::size(data) / 3 / elemInline;
            auto height = elemInline;
            mat.resize(height);
            for_each(begin(mat), end(mat), [width](auto& row) {row.resize(width); });

            for (size_t y = 0; y < height; ++y)
            {
                for (size_t x = 0; x < width; ++x)
                {
                    mat[y][x] = *it++;
                }
            }
        }
    }

    return retRgb;
}

void wv::Quantize(std::vector<double>& data, double quantPercent)
{
    auto threshold = 255. / 100. * quantPercent;
    for (auto& coef : data)
    {
        //coef = round(coef);
        if (abs(coef) < threshold)
        {
            coef = 0;
        }
    }
}

void wv::Compress(YCbCr& ycbcr, double quantCoef, size_t nIter)
{
    uint16_t height = std::size(ycbcr[0]);
    uint16_t width = std::size(ycbcr[0][0]);
    uint8_t nIt = nIter;

    DirectTransform2D2(ycbcr[0], nIter / 2);
    DirectTransform2D2(ycbcr[1], nIter);
    DirectTransform2D2(ycbcr[2], nIter);

    Quantize(ycbcr[0], nIter / 2, quantCoef);
    Quantize(ycbcr[1], nIter, quantCoef);
    Quantize(ycbcr[2], nIter, quantCoef);
}




bool wv::Compress(const std::string& _fileName, double quantCoef, size_t nIter)
{
    auto fileName = _fileName;
    HandleWhiteSpaces(fileName);
    auto ycbcr = rgbToYcbcr2(rgbDoubleFromBmp(fileName));

    uint16_t height = std::size(ycbcr[0]);
    uint16_t width = std::size(ycbcr[0][0]);
    uint8_t nIt = nIter;

    DirectTransform2D2(ycbcr[0], nIter / 2);
    DirectTransform2D2(ycbcr[1], nIter);
    DirectTransform2D2(ycbcr[2], nIter);

    Quantize(ycbcr[0], nIter / 2, quantCoef);
    Quantize(ycbcr[1], nIter, quantCoef);
    Quantize(ycbcr[2], nIter, quantCoef);



    auto pureName = fileName.substr(0, fileName.find('.'));
    auto tempName = "tmp_" + pureName;
    std::ofstream file(tempName + ".wv", std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        file.close();
        return false;
    }

    file.write((char*)&width, sizeof(width));
    file.write((char*)&height, sizeof(height));
    file.write((char*)&nIt, sizeof(nIt));

    float* data = nullptr;

    for (const auto& mat : ycbcr)
    {
        for (const auto& row : mat)
        {
            for (const auto& elem : row)
            {
                float val = elem;
                if (elem < 0)
                    val = 0;
                file.write((char*)&val, sizeof(val));
            }
        }
    }

    file.close();
    if (!file.good())
    {
        return false;
    }

    if (std::system(("7z.exe a -t7z " + pureName + ".wv " + tempName + ".wv").c_str()))
    {
        return false;
    }
    else
    {
        std::system(("del " + tempName + ".wv").c_str());
    }

    return true;
}


bool wv::Save(const YCbCr& ycbcr, size_t nIter, const std::string& newFileName)
{

    auto newFullPath = SplitFullFileName(newFileName);
    auto tempName = newFullPath[0] + "tmp_" + newFullPath[1];
    std::ofstream file(tempName + ".wv", std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        file.close();
        return false;
    }

    uint16_t height = std::size(ycbcr[0]);
    uint16_t width = std::size(ycbcr[0][0]);
    uint8_t nIt = nIter;

    file.write((char*)&width, sizeof(width));
    file.write((char*)&height, sizeof(height));
    file.write((char*)&nIt, sizeof(nIt));

    float* data = nullptr;

    for (const auto& mat : ycbcr)
    {
        for (const auto& row : mat)
        {
            for (const auto& elem : row)
            {
                float val = elem;
                if (elem < 0)
                    val = 0;
                file.write((char*)&val, sizeof(val));
            }
        }
    }

    file.close();


    if (std::system(("7z.exe a -t7z " + newFullPath[0] + newFullPath[1] + ".wv " + tempName + ".wv").c_str()))
    {
        return false;
    }
    else
    {
        std::system(("del " + tempName + ".wv").c_str());
    }

    return true;
}

bool wv::Compress(const std::string& fileName, const std::string& newFileName, double quantCoef, size_t nIter)
{

    
    auto ycbcr = rgbToYcbcr2(rgbDoubleFromBmp(fileName));

    uint16_t height = std::size(ycbcr[0]);
    uint16_t width = std::size(ycbcr[0][0]);
    uint8_t nIt = nIter;

    DirectTransform2D2(ycbcr[0], nIter / 2);
    DirectTransform2D2(ycbcr[1], nIter);
    DirectTransform2D2(ycbcr[2], nIter);

    Quantize(ycbcr[0], nIter / 2, quantCoef);
    Quantize(ycbcr[1], nIter, quantCoef);
    Quantize(ycbcr[2], nIter, quantCoef);



    auto fullPath = SplitFullFileName(fileName);
    auto newFullPath = SplitFullFileName(newFileName);
    if (newFullPath[0].empty())
    {
        newFullPath[0] = fullPath[0];
    }
    auto tempName = newFullPath[0] + "tmp_" + newFullPath[1];
    std::ofstream file(tempName + ".wv", std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        file.close();
        return false;
    }

    file.write((char*)&width, sizeof(width));
    file.write((char*)&height, sizeof(height));
    file.write((char*)&nIt, sizeof(nIt));

    float* data = nullptr;

    for (const auto& mat : ycbcr)
    {
        for (const auto& row : mat)
        {
            for (const auto& elem : row)
            {
                float val = elem;
                if (elem < 0)
                    val = 0;
                file.write((char*)&val, sizeof(val));
            }
        }
    }

    file.close();


    if (std::system(("7z.exe a -t7z " + newFullPath[0] + newFullPath[1] + ".wv " + tempName + ".wv").c_str()))
    {
        return false;
    }
    else
    {
        std::system(("del " + tempName + ".wv").c_str());
    }

    return true;
}


bool wv::Decompress(const std::string& fileName)
{
    auto pureName = fileName.substr(0, fileName.rfind('.'));
    auto tempName = "tmp_" + pureName;
    if (!fileName.ends_with(".wv"))
    {
        return false;
    }
    if (std::system(("7z.exe e -t7z " + pureName + ".wv").c_str()))
    {
        return false;
    }
    else
    {
        std::system(("del " + pureName + ".wv").c_str());
    }    


    std::ifstream file(tempName + ".wv", std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    uint16_t width;
    uint16_t height;
    uint8_t nIt;

    file.read((char*)&width, sizeof(width));
    file.read((char*)&height, sizeof(height));
    file.read((char*)&nIt, sizeof(nIt));

    float data = 0;

    YCbCr ycbcr(3);
    for (auto& mat : ycbcr)
    {
        mat.resize(height);
        for (auto& row : mat)
        {
            row.resize(width);
            for (auto& elem : row)
            {
                file.read((char*)&data, sizeof(data));
                elem = data;
            }
        }
    }
    file.close();
    std::system(("del " + tempName + ".wv").c_str());

    InverseTransform2D2(ycbcr[0], nIt / 2);
    InverseTransform2D2(ycbcr[1], nIt);
    InverseTransform2D2(ycbcr[2], nIt);

    rgbToBmp(ycbcrToRgb2(ycbcr), pureName);
    return true;
}

std::pair<YCbCr, size_t> wv::Open(const std::string& fileName)
{
    auto fullPath = SplitFullFileName(fileName);
    auto tempName = fullPath[0] + "tmp_" + fullPath[1];
    if (std::system(("7z.exe e -t7z " + fullPath[0] + fullPath[1] + ".wv").c_str()))
    {
        return {};
    }
    std::ifstream file("tmp_" + fullPath[1] + ".wv", std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        return {};
    }

    uint16_t width;
    uint16_t height;
    uint8_t nIt;

    file.read((char*)&width, sizeof(width));
    file.read((char*)&height, sizeof(height));
    file.read((char*)&nIt, sizeof(nIt));

    float data = 0;

    YCbCr ycbcr(3);
    for (auto& mat : ycbcr)
    {
        mat.resize(height);
        for (auto& row : mat)
        {
            row.resize(width);
            for (auto& elem : row)
            {
                file.read((char*)&data, sizeof(data));
                elem = data;
            }
        }
    }
    file.close();
    std::system(("del tmp_" + fullPath[1] + ".wv").c_str());

    return { std::move(ycbcr), nIt };
}


void wv::Decompress(YCbCr& ycbcr, size_t nIter)
{
    InverseTransform2D2(ycbcr[0], nIter / 2);
    InverseTransform2D2(ycbcr[1], nIter);
    InverseTransform2D2(ycbcr[2], nIter);
}

bool wv::Decompress(const std::string& fileName, const std::string& newFileName)
{
    auto fullPath = SplitFullFileName(fileName);
    auto newFullPath = SplitFullFileName(newFileName);
    if (newFullPath[0].empty())
    {
        newFullPath[0] = fullPath[0];
    }
    auto tempName = fullPath[0] + "tmp_" + fullPath[1];
    if (std::system(("7z.exe e -t7z " + fullPath[0] + fullPath[1] + ".wv").c_str()))
    {
        return false;
    }
    else
    {
        //std::system(("del " + fullPath[0] + fullPath[1] + ".wv").c_str());
    }


    std::ifstream file("tmp_" + fullPath[1] + ".wv", std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }

    uint16_t width;
    uint16_t height;
    uint8_t nIt;

    file.read((char*)&width, sizeof(width));
    file.read((char*)&height, sizeof(height));
    file.read((char*)&nIt, sizeof(nIt));

    float data = 0;

    YCbCr ycbcr(3);
    for (auto& mat : ycbcr)
    {
        mat.resize(height);
        for (auto& row : mat)
        {
            row.resize(width);
            for (auto& elem : row)
            {
                file.read((char*)&data, sizeof(data));
                elem = data;
            }
        }
    }
    file.close();
    std::system(("del tmp_" + fullPath[1] + ".wv").c_str());

    InverseTransform2D2(ycbcr[0], nIt / 2);
    InverseTransform2D2(ycbcr[1], nIt);
    InverseTransform2D2(ycbcr[2], nIt);

    rgbToBmp(ycbcrToRgb2(ycbcr), newFullPath[0] + newFullPath[1]);
    return true;
}


void Decimate(std::vector<std::vector<double>>& data, size_t nIter, double quantPercent)
{
    auto height = size(data);
    auto width = size(data[0]);
    std::vector<double*>d;;
    auto n = sizeof(size_t) * 8 - std::countl_zero(std::size(data)) - 1;
    nIter = n < nIter ? n : nIter;
    auto startCol = (size_t)pow(2, n - nIter);
    auto startRow = (size_t)pow(2, n - nIter);
    auto threshold = 255. / 100. * quantPercent;
    size_t i = 0;
    for (size_t y = startRow; y < std::size(data); ++y)
    {
        for (size_t x = startCol; x < std::size(data); ++x)
        {
            d.push_back(&data[y][x]);
        }
    }

    std::sort(std::begin(d), std::end(d), [](auto l, auto r) {return abs(*l) < abs(*r); });
    for (size_t j = 0; j < size(d) * quantPercent / 100; ++j)
    {
        *d[j] = 0;
    }
}

void wv::Quantize(std::vector<std::vector<double>>& data, size_t nIter, double quantPercent)
{
    Decimate(data, nIter, quantPercent);
    /*auto n = sizeof(size_t) * 8 - std::countl_zero(std::size(data)) - 1;
    nIter = n < nIter ? n : nIter;
    auto startCol = (size_t)pow(2, n - nIter);
    auto startRow = (size_t)pow(2, n - nIter);
    auto threshold = 255. / 100. * quantPercent;

    for (size_t y = startRow; y < std::size(data); ++y)
    {
        for (size_t x = startCol; x < std::size(data); ++x)
        {
            auto& coef = data[y][x];
            if (abs(coef) < threshold)
            {
                coef = 0;
            }

        }
    }*/
}




