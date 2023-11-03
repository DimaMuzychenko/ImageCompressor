#pragma once

#include <vector>
#include <iostream>

#include<SFML/Graphics/Image.hpp>
#include <SFML/System/MemoryInputStream.hpp>

using Byte = uint8_t;
using RGB = std::vector<std::vector<std::vector<double>>>;
using RgbByte = std::vector<std::vector<std::vector<Byte>>>;
using YCbCr = std::vector<std::vector<std::vector<double>>>;
using Matrix = std::vector<std::vector<double>>;
//using GrayImg = std::vector<std::vector<Byte>>;

RGB rgbDoubleFromBmp(const std::string& fileName);

RgbByte rgbByteFromBmp(const std::string& fileName);

void rgbToBmp(const RGB& rgb, const std::string& fileName);

void rgbToBmp(const RgbByte& rgb, const std::string& fileName);

YCbCr rgbToYcbcr(const RGB& rgb);

RGB ycbcrToRgb(const YCbCr& ycbcr);

YCbCr rgbToYcbcr2(const RGB& rgb);

RGB ycbcrToRgb2(const YCbCr& ycbcr);

Matrix RgbToMatrix(const RgbByte& rgb);

void MatrixToBmp(const Matrix& gray, const std::string& fileName);

void MatricesToBmp(const Matrix& R, const Matrix& G, const Matrix& B, const std::string& fileName);

std::vector<std::string> SplitFullFileName(const std::string& fullPath);

void HandleWhiteSpaces(std::string& path);
