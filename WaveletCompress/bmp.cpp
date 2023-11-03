#include "bmp.h"
#include <stdexcept>






Matrix RgbToMatrix(const RgbByte& rgb)
{
	auto height = size(rgb[0]);
	auto width = size(rgb[0][0]);
	Matrix mat(height);
	for (auto& row : mat)
		row.resize(width);
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
				mat[y][x] = (rgb[0][y][x] + rgb[1][y][x] + rgb[2][y][x]) / 3.0;
		}
	}
	return mat;
}

void MatrixToBmp(const Matrix& mat, const std::string& fileName)
{
	sf::Image img;
	auto width = std::size(mat[0]);
	auto height = std::size(mat);
	img.create(width, height);
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			auto val = mat[y][x];
			sf::Uint8 c = static_cast<sf::Uint8>(val);
			if (val < 0)
			{
				c = 0;
			}			
			sf::Color color(c, c, c);
			img.setPixel(x, y, color);
		}
	}
	auto file = fileName.ends_with(".bmp") ? fileName : fileName + ".bmp";
	if (!img.saveToFile(file))
	{
		std::cout << "Saving to " << file << " failed!" << std::endl;
	}
}

void MatricesToBmp(const Matrix& R, const Matrix& G, const Matrix& B, const std::string& fileName)
{
	sf::Image img;
	auto width = std::size(R[0]);
	auto height = std::size(R);
	img.create(width, height);
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			sf::Uint8 r, g, b;
			r = static_cast<sf::Uint8>(round(R[y][x]));
			g = static_cast<sf::Uint8>(round(G[y][x]));
			b = static_cast<sf::Uint8>(round(B[y][x]));
			if (R[y][x] < 0)
			{
				r = 0;
			}
			if (G[y][x] < 0)
			{
				g = 0;
			}
			if (B[y][x] < 0)
			{
				b = 0;
			}

			sf::Color color(r, g, b);
			img.setPixel(x, y, color);
		}
	}
	auto file = fileName.ends_with(".bmp") ? fileName : fileName + ".bmp";
	if (!img.saveToFile(file))
	{
		std::cout << "Saving to " << file << " failed!" << std::endl;
	}
}

RGB rgbDoubleFromBmp(const std::string& fileName)
{
	sf::Image img;
	if (!img.loadFromFile(fileName))
	{
		throw std::invalid_argument("Cannot open " + fileName);
	}

	uint8_t* pPixels = (uint8_t*)img.getPixelsPtr();
	std::vector<std::vector<std::vector<double>>> rgb(3);
	for (auto& vec : rgb)
	{
		vec.resize(img.getSize().y);
		for (auto& v : vec)
		{
			v.resize(img.getSize().x);
		}
	}

	for (int i = 0; i < img.getSize().x * img.getSize().y * 4; i+=4)
	{
		size_t x = i / 4 % img.getSize().y;
		size_t y = i / 4 / img.getSize().x;

		rgb[0][y][x] = (double)pPixels[i]; // r
		rgb[1][y][x] = (double)pPixels[i+1]; // g
		rgb[2][y][x] = (double)pPixels[i+2]; // b
	}

	return rgb;
}

RgbByte rgbByteFromBmp(const std::string& fileName)
{
	sf::Image img;
	if (!img.loadFromFile(fileName))
	{
		throw std::invalid_argument("Cannot open " + fileName);
	}

	Byte* pPixels = (Byte*)img.getPixelsPtr();
	RgbByte rgb(3);
	for (auto& vec : rgb)
	{
		vec.resize(img.getSize().y);
		for (auto& v : vec)
		{
			v.resize(img.getSize().x);
		}
	}

	for (int i = 0; i < img.getSize().x * img.getSize().y * 4; i += 4)
	{
		size_t x = i / 4 % img.getSize().y;
		size_t y = i / 4 / img.getSize().x;

		rgb[0][y][x] = pPixels[i]; // r
		rgb[1][y][x] = pPixels[i + 1]; // g
		rgb[2][y][x] = pPixels[i + 2]; // b
	}

	return rgb;
}


void rgbToBmp(const RGB& rgb, const std::string& fileName)
{
	sf::Image img;
	auto width = std::size(rgb[0][0]);
	auto height = std::size(rgb[0]);
	img.create(width, height);
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			auto r = static_cast<sf::Uint8>(std::clamp(rgb[0][y][x], 0., 255.));
			auto g = static_cast<sf::Uint8>(std::clamp(rgb[1][y][x], 0., 255.));
			auto b = static_cast<sf::Uint8>(std::clamp(rgb[2][y][x], 0., 255.));
			sf::Color color(r, g, b);
			img.setPixel(x, y, color);
		}
	}
	auto file = fileName.ends_with(".bmp") ? fileName : fileName + ".bmp";
	if (!img.saveToFile(file))
	{
		std::cout << "Saving to " << file << " failed!" << std::endl;
	}
}

void rgbToBmp(const RgbByte& rgb, const std::string& fileName)
{
	sf::Image img;
	auto width = std::size(rgb[0][0]);
	auto height = std::size(rgb[0]);
	img.create(width, height);
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			auto r = static_cast<sf::Uint8>(std::clamp(rgb[0][y][x], (Byte)0, (Byte)255));
			auto g = static_cast<sf::Uint8>(std::clamp(rgb[1][y][x], (Byte)0, (Byte)255));
			auto b = static_cast<sf::Uint8>(std::clamp(rgb[2][y][x], (Byte)0, (Byte)255));
			sf::Color color(r, g, b);
			img.setPixel(x, y, color);
		}
	}
	auto file = fileName.ends_with(".bmp") ? fileName : fileName + ".bmp";
	if (!img.saveToFile(file))
	{
		std::cout << "Saving to " << file << " failed!" << std::endl;
	}
}


YCbCr rgbToYcbcr(const RGB& rgb)
{
	using namespace std;
	auto height = size(rgb[0]);
	auto width = size(rgb[0][0]);
	YCbCr retImg(3);
	for (auto& img : retImg)
	{
		img.resize(height);
		for (auto& row : img)
		{
			row.resize(width);
		}
	}

	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			auto R = rgb[0][y][x];
			auto G = rgb[1][y][x];
			auto B = rgb[2][y][x];

			auto& Y = retImg[0][y][x];
			auto& Cb = retImg[1][y][x];
			auto& Cr = retImg[2][y][x];

			Y = 0 + (0.299 * R) + (0.587 * G) + (0.114 * B);
			Cb = 128 - (0.168736 * R) - (0.331264 * G) + (0.5 * B);
			Cr = 128 + (0.5 * R) - (0.418688 * G) - (0.081312 * B);
		}
	}

	return retImg;
}

RGB ycbcrToRgb(const YCbCr& ycbcr)
{
	using namespace std;
	auto height = size(ycbcr[0]);
	auto width = size(ycbcr[0][0]);
	YCbCr retImg(3);
	for (auto& img : retImg)
	{
		img.resize(height);
		for (auto& row : img)
		{
			row.resize(width);
		}
	}

	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			auto Y = ycbcr[0][y][x];
			auto Cb = ycbcr[1][y][x];
			auto Cr = ycbcr[2][y][x];

			auto& R = retImg[0][y][x];
			auto& G = retImg[1][y][x];
			auto& B = retImg[2][y][x];

			R = Y + 1.402 * (Cr - 128);
			G = Y - 0.34414 * (Cb - 128) - 0.71414 * (Cr - 128);
			B = Y + 1.772 * (Cb - 128);
		}
	}

	return retImg;
}


YCbCr rgbToYcbcr2(const RGB& rgb)
{
	using namespace std;
	auto height = size(rgb[0]);
	auto width = size(rgb[0][0]);
	YCbCr retImg(3);
	for (auto& img : retImg)
	{
		img.resize(height);
		for (auto& row : img)
		{
			row.resize(width);
		}
	}

	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			auto R = rgb[0][y][x];
			auto G = rgb[1][y][x];
			auto B = rgb[2][y][x];

			auto& Y = retImg[0][y][x];
			auto& Cb = retImg[1][y][x];
			auto& Cr = retImg[2][y][x];

			Y = 0.257 * R + 0.504 * G + 0.098 * B + 16;
			Cb = -0.148 * R - 0.291 * G + 0.439 * B + 128;
			Cr = 0.439 * R - 0.368 * G - 0.071 * B + 128;
		}
	}

	return retImg;
}

RGB ycbcrToRgb2(const YCbCr& ycbcr)
{
	using namespace std;
	auto height = size(ycbcr[0]);
	auto width = size(ycbcr[0][0]);
	YCbCr retImg(3);
	for (auto& img : retImg)
	{
		img.resize(height);
		for (auto& row : img)
		{
			row.resize(width);
		}
	}

	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			auto Y = ycbcr[0][y][x];
			auto Cb = ycbcr[1][y][x];
			auto Cr = ycbcr[2][y][x];

			auto& R = retImg[0][y][x];
			auto& G = retImg[1][y][x];
			auto& B = retImg[2][y][x];

			R = 1.164 * (Y - 16) + 1.596 * (Cr - 128);
			G = 1.164 * (Y - 16) - 0.813 * (Cr - 128) - 0.392 * (Cb - 128);
			B = 1.164 * (Y - 16) + 2.017 * (Cb - 128);
		}
	}

	return retImg;
}


std::vector<std::string> SplitFullFileName(const std::string& fullPath)
{
	std::vector<std::string> parts(3);
	auto slashIdx = fullPath.rfind('\\') + 1;
	auto dotIdx = fullPath.rfind('.');
	if (slashIdx != 0)
	{
		parts[0] = fullPath.substr(0, slashIdx);
		
		if (dotIdx != -1)
		{
			parts[1] = fullPath.substr(slashIdx, dotIdx - size(parts[0]));
			parts[2] = fullPath.substr(dotIdx);
		}
		else
		{
			parts[1] = fullPath.substr(slashIdx);
		}
	}
	else
	{
		if (dotIdx != -1)
		{
			parts[1] = fullPath.substr(0, dotIdx);
			parts[2] = fullPath.substr(dotIdx);
		}
		else
		{
			parts[1].append(fullPath.c_str());
		}
	}
	return parts;
}


void HandleWhiteSpaces(std::string& path)
{
	size_t idx = 0;

	while (idx = path.find_first_of(' ', idx), idx != -1)
	{
		auto rhsSlashIdx = path.find('\\', idx);
		if (rhsSlashIdx == -1)
			rhsSlashIdx = size(path);
		auto lhsSlashIdx = path.rfind('\\', idx);
		path.insert(lhsSlashIdx + 1, 1, '\"');
		path.insert(rhsSlashIdx + 1, 1, '\"');
		idx = rhsSlashIdx;
	}
}

