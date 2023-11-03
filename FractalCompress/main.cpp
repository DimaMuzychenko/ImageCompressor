#include <iostream>

#include "fractal.h"
#include "../WaveletCompress/bmp.h"

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& collection)
{
	os << "[ ";
	for (auto& elem : collection)
	{
		os << (int)elem << ' ';
	}
	os << "]";

	return os;
}

template<typename T>
void Print(const std::vector<T>& collection, const std::string& splitter = " ")
{
	std::cout << "[ ";
	for (auto& elem : collection)
	{
		std::cout << elem << splitter;
	}
	std::cout << "]\n";
}


//void TestGrayImg()
//{
//	auto rgb = rgbByteFromBmp("monkey.gif");
//	auto gray = RgbToMatrix(rgb);
//	//gray = fr::Reduce(gray, 4);
//	auto s = 16;
//	auto d = 8;
//	auto iter = 16;
//	auto transformations = fr::Compress(gray, s, d, s);
//	auto res = fr::Decompress(transformations, s, d, s, iter);
//	MatrixToBmp(res, "iter" + std::to_string(iter));
//}
//
//void TestRgbImg()
//{
//	auto rgb = rgbDoubleFromBmp("test_img.bmp");
//	auto s = 16;
//	auto d = 8;
//	auto iter = 16;
//
//	auto trR = fr::Compress(fr::Reduce(rgb[0], 2), s, d, s);
//	auto trG = fr::Compress(fr::Reduce(rgb[1], 2), s, d, s);
//	auto trB = fr::Compress(fr::Reduce(rgb[2], 2), s, d, s);
//
//	auto itR = fr::Decompress(trR, s, d, s, iter);
//	auto itG = fr::Decompress(trG, s, d, s, iter);
//	auto itB = fr::Decompress(trB, s, d, s, iter);
//
//	MatricesToBmp(itR, itG, itB, "rgb_iter" + std::to_string(iter));
//}


int main(int argc, const char** argv)
{
	/*if (argc < 3)
	{
		std::cout << "Too few arguments!" << std::endl;
		return -1;
	}
	else if (argc > 4)
	{
		std::cout << "Too many arguments!" << std::endl;
	}

	std::string key = argv[1];
	std::string originName = argv[2];
	std::string newName = argc == 4 ? argv[3] : argv[2];

	if (!originName.ends_with(".bmp"))
	{
		std::cout << "Invalid file format!" << std::endl;
		return -1;
	}*/

	fr::Compress("monkey.gif", 16, 8);
	fr::Decompress("monkey.fr", 16);
	

	return 0;
}