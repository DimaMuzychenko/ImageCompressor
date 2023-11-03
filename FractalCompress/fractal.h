#pragma once
#include <vector>
#include <algorithm>
#include <limits>
#include <random>
#include <fstream>
#include "../WaveletCompress/bmp.h"

using Byte = uint8_t;
//using GrayImg = std::vector<std::vector<double>>;
using Matrix = std::vector<std::vector<double>>;

namespace fr
{
	

	struct Transformation
	{
		size_t x;
		size_t y;
		Byte direction;
		Byte angle;
		double contrast;
		double brightness;
	};

	struct TransformedBlock
	{
		size_t x;
		size_t y;
		Byte direction;
		Byte angle;
		Matrix img;
	};

	struct ImageProps
	{
		std::vector<std::vector<std::vector<Transformation>>> transformations;
		uint32_t width;
		uint32_t height;
		uint32_t dSize;
		uint32_t rSize;
		uint32_t step;
	};

	Matrix GetBlock(const Matrix& img, size_t sX, size_t sY, size_t eX, size_t eY);

	double Mean(const Matrix& img, size_t sX, size_t sY, size_t eX, size_t eY);

	Matrix Reduce(const Matrix& img, Byte factor);

	Matrix Rotate(const Matrix& img, Byte angle);

	Matrix Flip(const Matrix& img, Byte direction);

	Matrix ApplyTransformation(const Matrix& img, Byte direction, Byte angle, double contrast = 1.0, double brightness = 0.0);

	std::pair<double, double> FindContrastAndBrightness(const Matrix& D, const Matrix& S);

	std::pair<double, double> FindContrastAndBrightness2(const Matrix& D, const Matrix& S);

	std::vector<std::vector<fr::Transformation>> Compress(const Matrix& img, size_t domenBlockSize, size_t rangeBlockSize, size_t step);

	Matrix Decompress(const std::vector<std::vector<fr::Transformation>>& transformations, size_t sourceSize, size_t destinationSize, size_t step, size_t nIter);

	bool Compress(const std::string& fileName, size_t domenBlockSize, size_t rangeBlockSize);

	bool Compress(const std::string& fileName, const std::string& newFileName, size_t domenBlockSize, size_t rangeBlockSize);

	bool Decompress(const std::string& fileName, size_t nIter);

	bool Decompress(const std::string& fileName, const std::string& newFileName, size_t nIter);

	std::vector<std::vector<std::vector<fr::Transformation>>> Compress(const RGB& rgb, size_t domenBlockSize, size_t rangeBlockSize, size_t step);

	bool Save(size_t _width, size_t _height, size_t domenSize, size_t rangeSize, size_t step, const std::vector<std::vector<std::vector<fr::Transformation>>>& transformations, const std::string& newFileName);

	RGB Decompress(const std::vector<std::vector<std::vector<fr::Transformation>>>& transformations, size_t dSize, size_t rSize, size_t step, size_t nIter);

	fr::ImageProps Open(const std::string& fileName);
}
