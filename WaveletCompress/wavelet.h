#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <memory>

#include "bmp.h"


template<typename T>
std::vector<T> operator+(const std::vector<T>& lhs, const std::vector<T>& rhs);

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& collection);

namespace wv
{

	std::vector<double> RecursiveDirectTransform1D(const std::vector<double>& data, bool consolLogging = false);

	void DirectTransform1D(std::vector<double>& data, size_t endIdx);

	void DirectTransform1D(std::vector<double*>& data, size_t endIdx);

	void InverseTransform1D(std::vector<double>& data, size_t endIdx);

	void InverseTransform1D(std::vector<double*>& data, size_t endIdx);

	void DirectTransformLeftToRight(std::vector<std::vector<double>>& data, size_t endRow, size_t endCol);

	void DirectTransformTopToDown(std::vector<std::vector<double>>& data, size_t endRow, size_t endCol);

	void InverseTransformLeftToRight(std::vector<std::vector<double>>& data, size_t endRow, size_t endCol);

	void InverseTransformTopToDown(std::vector<std::vector<double>>& data, size_t endRow, size_t endCol);

	void DirectTransform2D(std::vector<std::vector<double>>& data, size_t endRow, size_t endCol);

	void InverseTransform2D(std::vector<std::vector<double>>& data, size_t startRow, size_t startCol);

	void DirectTransform2D2(std::vector<std::vector<double>>& data, size_t nIter);

	void InverseTransform2D2(std::vector<std::vector<double>>& data, size_t nIter);

	std::vector<double> To1D(const std::vector<std::vector<double>>& data, bool rowByRow = true);
	
	std::vector<double> To1D(const RGB& rgb, bool rowByRow = true);

	std::vector<std::vector<double>> To2D(const std::vector<double>& data, size_t elemInline, bool rowByRow = true);
	
	RGB To3D(const std::vector<double>& data, size_t elemInline, bool rowByRow = true);

	void Quantize(std::vector<double>& data, double quantPercent);

	void Quantize(std::vector<std::vector<double>>& data, size_t nIter, double quantPercent);

	bool Compress(const std::string& fileName, double quantCoef, size_t nIter);

	bool Compress(const std::string& fileName, const std::string& newFileName, double quantCoef, size_t nIter);

	bool Decompress(const std::string& fileName);

	bool Decompress(const std::string& fileName, const std::string& newFileName);

	void Compress(YCbCr& ycbcr, double quantCoef, size_t nIter);

	bool Save(const YCbCr& ycbcr, size_t nIter, const std::string& newFileName);

	std::pair<YCbCr, size_t> Open(const std::string& fileName);

	void Decompress(YCbCr& ycbcr, size_t nIter);
}


