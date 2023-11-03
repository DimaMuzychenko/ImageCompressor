#include <vector>
#include <string>
#include <iostream>

#include <SFML/Graphics/Image.hpp>

#include "wavelet.h"
#include "bmp.h"


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



// WaveletCompress -c(-d) originalFileName compressedFileName

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

	try
	{
		double coef = 95;
		std::cout << wv::Compress("oplot.bmp", coef, 4) << std::endl;
		std::cout << wv::Decompress("oplot.wv", "newOplot") << std::endl;
	}
	catch (std::invalid_argument& ex)
	{
		std::cout << ex.what() << std::endl;
		return -1;
	}


	return 0;
}