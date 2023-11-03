


#include "fractal.h"


//GrayImg ToGrayImg(const Matrix& A)
//{
//    auto height = size(A);
//    auto width = size(A[0]);
//    GrayImg retImg(height);
//    for (auto& row : retImg)
//    {
//        row.resize(width);
//    }
//    for (size_t y = 0; y < height; ++y)
//    {
//        for (size_t x = 0; x < width; ++x)
//        {
//            retImg[y][x] = A[y][x];
//        }
//    }
//    return retImg;
//}
//
//Matrix ToMatrix(const GrayImg& A)
//{
//    auto height = size(A);
//    auto width = size(A[0]);
//    Matrix retImg(height);
//    for (auto& row : retImg)
//    {
//        row.resize(width);
//    }
//    for (size_t y = 0; y < height; ++y)
//    {
//        for (size_t x = 0; x < width; ++x)
//        {
//            retImg[y][x] = A[y][x];
//        }
//    }
//    return retImg;
//}

Matrix fr::GetBlock(const Matrix& img, size_t sX, size_t eX, size_t sY, size_t eY)
{
    Matrix block(eY - sY);
    for (auto& row : block)
    {
        row.resize(eX - sX);
    }
    for (size_t y = sY; y < eY; ++y)
    {
        for (size_t x = sX; x < eX; ++x)
        {
            block[y - sY][x - sX] = img[y][x];
        }
    }

    return block;
}


//def reduce(img, factor) :
//    result = np.zeros((img.shape[0] // factor, img.shape[1] // factor))
//        for i in range(result.shape[0]) :
//            for j in range(result.shape[1]) :
//                result[i, j] = np.mean(img[i * factor:(i + 1) * factor, j * factor : (j + 1) * factor])
//    return result

double fr::Mean(const Matrix& img, size_t sX, size_t sY, size_t eX, size_t eY)
{
    double sum = 0;
    size_t count = 0;
    for (size_t y = sY; y < eY; ++y)
    {
        for (size_t x = sX; x < eX; ++x)
        {
            sum += img[y][x];
            ++count;
        }
    }

    return sum / count;
}

Matrix fr::Reduce(const Matrix& img, Byte factor)
{
    size_t newHeight = std::size(img) / factor;
    size_t newWidth = std::size(img[0]) / factor;
    Matrix retImg(newHeight);
    for (auto& row : retImg)
    {
        row.resize(newWidth);
    }

    for (size_t y = 0; y < newHeight; ++y)
    {
        for (size_t x = 0; x < newWidth; ++x)
        {
            retImg[y][x] = Mean(img, x * factor, y * factor, (x + 1) * factor, (y + 1) * factor);
        }
    }
    return retImg;
}


//def rotate(img, angle) :
//    return ndimage.rotate(img, angle, reshape = False)

Matrix fr::Rotate(const Matrix& img, Byte angle)
{
    size_t height = std::size(img);
    size_t width = std::size(img[0]);
    Matrix retImg(height);
    for (auto& row : retImg)
    {
        row.resize(width);
    }

    for (size_t y = 0; y < height; ++y)
    {
        for (size_t x = 0; x < width; ++x)
        {
            if (angle == 0)
            {
                retImg[y][x] = img[y][x];
            }
            else if (angle == 1)
            {
                retImg[width - 1 - x][y] = img[y][x];
            }
            else if (angle == 2)
            {
                retImg[height - 1 - y][width - 1 - x] = img[y][x];
            }
            else// if (angle == 3)
            {
                retImg[x][height - 1 - y] = img[y][x];
            }
        }
    }
    return retImg;
}


//def flip(img, direction) :
//    return img[::direction, :]

Matrix fr::Flip(const Matrix& img, Byte direction)
{
    size_t height = std::size(img);
    size_t width = std::size(img[0]);
    Matrix retImg(height);
    for (auto& row : retImg)
    {
        row.resize(width);
    }

    for (size_t y = 0; y < height; ++y)
    {
        for (size_t x = 0; x < width; ++x)
        {
            if (direction == 0)
            {
                retImg[y][x] = img[y][x];
            }
            else
            {
                retImg[height - 1 - y][x] = img[y][x];
            }
        }
    }
    return retImg;
}


//def apply_transformation(img, direction, angle, contrast = 1.0, brightness = 0.0) :
//    return contrast * rotate(flip(img, direction), angle) + brightness

Matrix fr::ApplyTransformation(const Matrix& img, Byte direction, Byte angle, double contrast, double brightness)
{
    using namespace std;

    size_t height = size(img);
    size_t width = size(img[0]);
    Matrix retImg = Rotate(Flip(img, direction), angle);

    for (auto& row : retImg)
    {
        //for_each(begin(row), end(row), [contrast, brightness](auto& elem) {elem = contrast * elem + brightness; });
        for (auto& elem : row)
        {
            auto newVal = contrast * elem + brightness;
            elem = newVal;
            if (newVal < 0)
            {
                elem = 0;
            }
            if (newVal > 255)
            {
                elem = 255;
            }
        }
    }

    return retImg;
}

//def generate_all_transformed_blocks(img, source_size, destination_size, step) :
//    factor = source_size // destination_size
//    transformed_blocks = []
//    for k in range((img.shape[0] - source_size) // step + 1):
//        for l in range((img.shape[1] - source_size) // step + 1):
//            # Extract the source block and reduce it to the shape of a destination block
//            S = reduce(img[k * step:k * step + source_size, l * step : l * step + source_size], factor)
//            # Generate all possible transformed blocks
//            for direction, angle in candidates :
//                transformed_blocks.append((k, l, direction, angle, apply_transformation(S, direction, angle)))
//    return transformed_blocks



std::vector<fr::TransformedBlock> GetAllTransformedBlocks(const Matrix& img, size_t sourceSize, size_t destinationSize, size_t step)
{
    auto factor = sourceSize / destinationSize;
    std::vector<fr::TransformedBlock> transformedBlocks;
    for (size_t y = 0; y < ((size(img) - sourceSize) / step + 1); ++y)
    {
        for (size_t x = 0; x < ((size(img[0]) - sourceSize) / step + 1); ++x)
        {

            auto S = fr::Reduce(fr::GetBlock(img, x * step, x * step + sourceSize, y * step, y * step + sourceSize), factor);
            for (Byte direction = 0; direction < 2; ++direction)
            {
                for (Byte angle = 0; angle < 4; ++angle)
                {
                    transformedBlocks.push_back({ x, y, direction, angle, fr::ApplyTransformation(S, direction, angle) });
                }
            }
        }
    }
    return transformedBlocks;
}


Matrix Multiply(const Matrix& a, double b)
{
    Matrix retMat(size(a));
    for (auto& row : retMat)
    {
        row.resize(size(a[0]));
    }

    for (size_t y = 0; y < size(retMat); ++y)
    {
        for (size_t x = 0; x < size(retMat[0]); ++x)
        {
            retMat[y][x] = a[y][x] * b;
        }
    }

    return retMat;
}

Matrix Add(const Matrix& a, double b)
{
    Matrix retMat(size(a));
    for (auto& row : retMat)
    {
        row.resize(size(a[0]));
    }

    for (size_t y = 0; y < size(retMat); ++y)
    {
        for (size_t x = 0; x < size(retMat[0]); ++x)
        {
            retMat[y][x] = a[y][x] + b;
        }
    }

    return retMat;
}

Matrix Subtract(const Matrix& a, const Matrix& b)
{
    Matrix retMat(size(a));
    for (auto& row : retMat)
    {
        row.resize(size(a[0]));
    }

    for (size_t y = 0; y < size(retMat); ++y)
    {
        for (size_t x = 0; x < size(retMat[0]); ++x)
        {
            retMat[y][x] = a[y][x] - b[y][x];
        }
    }

    return retMat;
}


Matrix GetElementWiseSquare(const Matrix& a)
{
    Matrix retMat(size(a));
    for (auto& row : retMat)
    {
        row.resize(size(a[0]));
    }
    for (size_t y = 0; y < size(retMat); ++y)
    {
        for (size_t x = 0; x < size(retMat[0]); ++x)
        {
            retMat[y][x] = pow(a[y][x], 2);
        }
    }

    return retMat;
}

double Sum(const Matrix& a)
{
    double sum = 0;
    for (size_t y = 0; y < size(a); ++y)
    {
        for (size_t x = 0; x < size(a[0]); ++x)
        {
            sum += a[y][x];
        }
    }
    return sum;
}



//def find_contrast_and_brightness1(D, S) :
//    # Fix the contrastand only fit the brightness
//    contrast = 0.75
//    brightness = (np.sum(D - contrast * S)) / D.size
//    return contrast, brightness
//
//def find_contrast_and_brightness2(D, S) :
//    # Fit the contrastand the brightness
//    A = np.concatenate((np.ones((S.size, 1)), np.reshape(S, (S.size, 1))), axis = 1)
//    b = np.reshape(D, (D.size, ))
//    x, _, _, _ = np.linalg.lstsq(A, b)
//    #x = optimize.lsq_linear(A, b, [(-np.inf, -2.0), (np.inf, 2.0)]).x
//    return x[1], x[0]


std::pair<double, double> fr::FindContrastAndBrightness(const Matrix& D, const Matrix& S)
{
    auto contrast = 0.75;
    auto tmp = Subtract(D, Multiply(S, contrast));
    auto brightness = Mean(tmp, 0, 0, size(tmp[0]), size(tmp));
    return { contrast, brightness };
}

std::pair<double, double> fr::FindContrastAndBrightness2(const Matrix& R, const Matrix& D)
{
    auto width = size(R[0]);
    auto height = size(R);
    auto meanR = Mean(R, 0, 0, width, height);
    auto meanD = Mean(D, 0, 0, width, height);
    auto a = 0.0;
    auto b = 0.0;
    for (size_t y = 0; y < height; ++y)
    {
        for (size_t x = 0; x < width; ++x)
        {
            a += ((D[y][x] - meanD) * (R[y][x] - meanR));
            b += pow(D[y][x] - meanD, 2);
        }
    }
    auto s = a / b;
    auto o = meanR - (a / b) * meanD;
    return { s, o };
}

//def compress(img, source_size, destination_size, step) :
//    transformations = []
//    transformed_blocks = generate_all_transformed_blocks(img, source_size, destination_size, step)
//    i_count = img.shape[0] // destination_size
//    j_count = img.shape[1] // destination_size
//    for i in range(i_count) :
//        transformations.append([])
//        for j in range(j_count) :
//            print("{}/{} ; {}/{}".format(i, i_count, j, j_count))
//            transformations[i].append(None)
//            min_d = float('inf')
//            # Extract the destination block
//            D = img[i * destination_size:(i + 1) * destination_size, j * destination_size : (j + 1) * destination_size]
//            # Test all possible transformations and take the best one
//            for k, l, direction, angle, S in transformed_blocks :
//              contrast, brightness = find_contrast_and_brightness2(D, S)
//              S = contrast * S + brightness
//              d = np.sum(np.square(D - S))
//              if d < min_d :
//                  min_d = d
//                  transformations[i][j] = (k, l, direction, angle, contrast, brightness)
//    return transformations

std::vector<std::vector<fr::Transformation>> fr::Compress(const Matrix& img, size_t domenBlockSize, size_t rangeBlockSize, size_t step)
{
    auto height = size(img);
    auto width = size(img[0]);
    std::vector<std::vector<fr::Transformation>> transformations(height / rangeBlockSize);
    std::for_each(std::begin(transformations), std::end(transformations), [width, rangeBlockSize](auto& row) {row.resize(width / rangeBlockSize); });

    auto TransformedBlocks = GetAllTransformedBlocks(img, domenBlockSize, rangeBlockSize, step);

    for (size_t y = 0; y < height / rangeBlockSize; ++y)
    {
        for (size_t x = 0; x < width / rangeBlockSize; ++x)
        {
            auto minDiff = DBL_MAX;
            auto R = fr::GetBlock(img, x * rangeBlockSize, (x + 1) * rangeBlockSize, y * rangeBlockSize, (y + 1) * rangeBlockSize);
            for (const auto& Domen : TransformedBlocks)
            {
                auto tmp = fr::FindContrastAndBrightness2(R, Domen.img);
                auto contrast = tmp.first;
                auto brightness = tmp.second;
                auto diff = Sum(GetElementWiseSquare(Subtract(Add(Multiply(Domen.img, contrast), brightness), R)));
                if (diff < minDiff)
                {
                    transformations[y][x] = { Domen.x, Domen.y, Domen.direction, Domen.angle, contrast, brightness };
                    if (diff < 500)
                    {
                        transformations[y][x] = { Domen.x, Domen.y, Domen.direction, Domen.angle, contrast, brightness };
                        break;
                    }
                    minDiff = diff;                    
                }
            }
        }
    }
    return transformations;
}


Matrix GetNoiseImg(size_t width, size_t height)
{
    Matrix img(height);
    for (auto& row : img)
    {
        row.resize(width);
    }

    std::default_random_engine generator;
    std::uniform_int_distribution<int>  distribution(0, 255);

    for (size_t y = 0; y < height; ++y)
    {
        for (size_t x = 0; x < width; ++x)
        {
            img[y][x] = distribution(generator);
        }
    }

    return img;
}


//def decompress(transformations, source_size, destination_size, step, nb_iter = 8) :
//    factor = source_size // destination_size
//    height = len(transformations) * destination_size
//    width = len(transformations[0]) * destination_size
//    iterations = [np.random.randint(0, 256, (height, width))]
//    cur_img = np.zeros((height, width))
//    for i_iter in range(nb_iter) :
//        print(i_iter)
//        for i in range(len(transformations)) :
//            for j in range(len(transformations[i])) :
//                # Apply transform
//                k, l, flip, angle, contrast, brightness = transformations[i][j]
//                S = reduce(iterations[-1][k * step:k * step + source_size, l * step : l * step + source_size], factor)
//                D = apply_transformation(S, flip, angle, contrast, brightness)
//                cur_img[i * destination_size:(i + 1) * destination_size, j * destination_size : (j + 1) * destination_size] = D
//                iterations.append(cur_img)
//                cur_img = np.zeros((height, width))
//                return iterations


//std::vector<Matrix> fr::Decompress(const std::vector<std::vector<fr::Transformation>>& transformations, size_t sourceSize, size_t destinationSize, size_t step, size_t nIter)
//{
//    auto factor = sourceSize / destinationSize;
//    auto height = size(transformations) * destinationSize;
//    auto width = size(transformations[0]) * destinationSize;
//    std::vector<Matrix> iterations;
//    iterations.push_back(GetNoiseImg(width, height));
//    Matrix curImg(height);
//    for (auto& row : curImg)
//        row.resize(width);
//    for (size_t iter = 0; iter < nIter; ++iter)
//    {
//        for (size_t i = 0; i < size(transformations); ++i)
//        {
//            for (size_t j = 0; j < size(transformations[0]); ++j)
//            {
//                const auto& tr = transformations[i][j];
//                auto S = fr::Reduce(fr::GetBlock(iterations[size(iterations) - 1], tr.x * step, tr.x * step + sourceSize, tr.y * step, tr.y * step + sourceSize), factor);
//                auto D = fr::ApplyTransformation(S, tr.direction, tr.angle, tr.contrast, tr.brightness);
//                for (size_t ii = i * destinationSize; ii < (i + 1) * destinationSize; ++ii)
//                {
//                    for (size_t jj = j * destinationSize; jj < (j + 1) * destinationSize; ++jj)
//                    {
//                        curImg[ii][jj] = D[ii - i * destinationSize][jj - j * destinationSize];
//                    }
//                }                
//            }
//        }
//        iterations.push_back(curImg);
//        for (auto& row : curImg)
//        {
//            for (auto& pixel : row)
//            {
//                pixel = 0;
//            }
//        }
//    }
//    return iterations;
//}

Matrix fr::Decompress(const std::vector<std::vector<fr::Transformation>>& transformations, size_t sourceSize, size_t destinationSize, size_t step, size_t nIter)
{

    auto factor = sourceSize / destinationSize;
    auto height = size(transformations) * destinationSize;
    auto width = size(transformations[0]) * destinationSize;
    auto img = GetNoiseImg(width, height);
    for (size_t iter = 0; iter < nIter; ++iter)
    {
        for (size_t i = 0; i < size(transformations); ++i)
        {
            for (size_t j = 0; j < size(transformations[0]); ++j)
            {
                const auto& tr = transformations[i][j];
                auto S = fr::Reduce(fr::GetBlock(img, tr.x * step, tr.x * step + sourceSize, tr.y * step, tr.y * step + sourceSize), factor);
                auto D = fr::ApplyTransformation(S, tr.direction, tr.angle, tr.contrast, tr.brightness);
                for (size_t ii = i * destinationSize; ii < (i + 1) * destinationSize; ++ii)
                {
                    for (size_t jj = j * destinationSize; jj < (j + 1) * destinationSize; ++jj)
                    {
                        img[ii][jj] = D[ii - i * destinationSize][jj - j * destinationSize];
                    }
                }
            }
        }
    }
    return img;
}


bool fr::Compress(const std::string& fileName, size_t domenBlockSize, size_t rangeBlockSize)
{
    auto rgb = rgbDoubleFromBmp(fileName);
    std::vector<std::vector<std::vector<fr::Transformation>>> trans(3);
    trans[0] = std::move(fr::Compress(rgb[0], domenBlockSize, rangeBlockSize, domenBlockSize));
    trans[1] = std::move(fr::Compress(rgb[1], domenBlockSize, rangeBlockSize, domenBlockSize));
    trans[2] = std::move(fr::Compress(rgb[2], domenBlockSize, rangeBlockSize, domenBlockSize));

    auto pureName = fileName.substr(0, fileName.find('.'));
    std::ofstream file(pureName + ".fr", std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        file.close();
        return false;
    }

    auto width = (uint32_t)size(rgb[0][0]);
    auto height = (uint32_t)size(rgb[0]);
    auto dSize = (uint32_t)domenBlockSize;
    auto rSize = (uint32_t)rangeBlockSize;
    file.write((char*)&width, sizeof(width));
    file.write((char*)&height, sizeof(height));
    file.write((char*)&dSize, sizeof(dSize));
    file.write((char*)&rSize, sizeof(rSize));
    
   for (const auto& mat : trans)
   {
        for (const auto& row : mat)
        {
            for (const auto& tran : row)
            {
                auto x = (uint16_t)tran.x;
                auto y = (uint16_t)tran.y;
                auto direction = (uint8_t)tran.direction;
                auto angle = (uint8_t)tran.angle;
                auto contrast = (float)tran.contrast;
                auto brightness = (float)tran.brightness;
                file.write((char*)&x, sizeof(x));
                file.write((char*)&y, sizeof(y));
                file.write((char*)&direction, sizeof(direction));
                file.write((char*)&angle, sizeof(angle));
                file.write((char*)&contrast, sizeof(contrast));
                file.write((char*)&brightness, sizeof(brightness));
            }
        }
   }
    
   file.close();
   if (!file.good())
   {
       return false;
   }
   return true;
}

std::vector<std::vector<std::vector<fr::Transformation>>> fr::Compress(const RGB& rgb, size_t domenBlockSize, size_t rangeBlockSize, size_t step)
{
    std::vector<std::vector<std::vector<fr::Transformation>>> trans(3);
    trans[0] = std::move(fr::Compress(rgb[0], domenBlockSize, rangeBlockSize, step));
    trans[1] = std::move(fr::Compress(rgb[1], domenBlockSize, rangeBlockSize, step));
    trans[2] = std::move(fr::Compress(rgb[2], domenBlockSize, rangeBlockSize, step));
    return trans;
}

bool fr::Save(size_t _width, size_t _height, size_t domenSize, size_t rangeSize, size_t _step, const std::vector<std::vector<std::vector<fr::Transformation>>>& transformations, const std::string& newFileName)
{
    auto newFullPath = SplitFullFileName(newFileName);
    std::ofstream file(newFullPath[0] + newFullPath[1] + ".fr", std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        file.close();
        return false;
    }

    auto width = (uint32_t)_width;//size(rgb[0][0]);
    auto height = (uint32_t)_height;//size(rgb[0]);
    auto dSize = (uint32_t)domenSize;//domenBlockSize;
    auto rSize = (uint32_t)rangeSize;//rangeBlockSize;
    auto step = (uint32_t)_step;
    file.write((char*)&width, sizeof(width));
    file.write((char*)&height, sizeof(height));
    file.write((char*)&dSize, sizeof(dSize));
    file.write((char*)&rSize, sizeof(rSize));
    file.write((char*)&step, sizeof(step));

    for (const auto& mat : transformations)
    {
        for (const auto& row : mat)
        {
            for (const auto& tran : row)
            {
                auto x = (uint16_t)tran.x;
                auto y = (uint16_t)tran.y;
                auto direction = (uint8_t)tran.direction;
                auto angle = (uint8_t)tran.angle;
                auto contrast = (float)tran.contrast;
                auto brightness = (float)tran.brightness;
                file.write((char*)&x, sizeof(x));
                file.write((char*)&y, sizeof(y));
                file.write((char*)&direction, sizeof(direction));
                file.write((char*)&angle, sizeof(angle));
                file.write((char*)&contrast, sizeof(contrast));
                file.write((char*)&brightness, sizeof(brightness));
            }
        }
    }

    file.close();

    return true;
}


bool fr::Compress(const std::string& fileName, const std::string& newFileName, size_t domenBlockSize, size_t rangeBlockSize)
{
    auto rgb = rgbDoubleFromBmp(fileName);
    std::vector<std::vector<std::vector<fr::Transformation>>> trans(3);
    trans[0] = std::move(fr::Compress(rgb[0], domenBlockSize, rangeBlockSize, domenBlockSize));
    trans[1] = std::move(fr::Compress(rgb[1], domenBlockSize, rangeBlockSize, domenBlockSize));
    trans[2] = std::move(fr::Compress(rgb[2], domenBlockSize, rangeBlockSize, domenBlockSize));

    auto fullPath = SplitFullFileName(fileName);
    auto newFullPath = SplitFullFileName(newFileName);
    if (newFullPath[0].empty())
    {
        newFullPath[0] = fullPath[0];
    }
    std::ofstream file(newFullPath[0] + newFullPath[1] + ".fr", std::ios::out | std::ios::binary);
    if (!file.is_open())
    {
        file.close();
        return false;
    }

    auto width = (uint32_t)size(rgb[0][0]);
    auto height = (uint32_t)size(rgb[0]);
    auto dSize = (uint32_t)domenBlockSize;
    auto rSize = (uint32_t)rangeBlockSize;
    file.write((char*)&width, sizeof(width));
    file.write((char*)&height, sizeof(height));
    file.write((char*)&dSize, sizeof(dSize));
    file.write((char*)&rSize, sizeof(rSize));

    for (const auto& mat : trans)
    {
        for (const auto& row : mat)
        {
            for (const auto& tran : row)
            {
                auto x = (uint16_t)tran.x;
                auto y = (uint16_t)tran.y;
                auto direction = (uint8_t)tran.direction;
                auto angle = (uint8_t)tran.angle;
                auto contrast = (float)tran.contrast;
                auto brightness = (float)tran.brightness;
                file.write((char*)&x, sizeof(x));
                file.write((char*)&y, sizeof(y));
                file.write((char*)&direction, sizeof(direction));
                file.write((char*)&angle, sizeof(angle));
                file.write((char*)&contrast, sizeof(contrast));
                file.write((char*)&brightness, sizeof(brightness));
            }
        }
    }

    file.close();
    if (!file.good())
    {
        return false;
    }
    return true;
}

bool fr::Decompress(const std::string& fileName, size_t nIter)
{
    if (!fileName.ends_with(".fr"))
    {
        return false;
    }
    std::ifstream file(fileName, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }
    uint32_t width;
    uint32_t height;
    uint32_t dSize;
    uint32_t rSize;
    file.read((char*)&width, sizeof(width));
    file.read((char*)&height, sizeof(height));
    file.read((char*)&dSize, sizeof(dSize));
    file.read((char*)&rSize, sizeof(rSize));

    std::vector<std::vector<std::vector<fr::Transformation>>> trans(3);
    for (auto& mat : trans)
    {
        mat.resize(height / rSize);
        for (auto& row : mat)
        {
            row.resize(width / rSize);
        }
    }

    for (auto& mat : trans)
    {
        for (auto& row : mat)
        {
            for (auto& tran : row)
            {
                uint16_t x;
                uint16_t y;
                uint8_t direction;
                uint8_t angle;
                float contrast;
                float brightness;
                file.read((char*)&x, sizeof(x));
                file.read((char*)&y, sizeof(y));
                file.read((char*)&direction, sizeof(direction));
                file.read((char*)&angle, sizeof(angle));
                file.read((char*)&contrast, sizeof(contrast));
                file.read((char*)&brightness, sizeof(brightness));
                tran.x = x;
                tran.y = y;
                tran.direction = direction;
                tran.angle = angle;
                tran.contrast = contrast;
                tran.brightness = brightness;
            }
        }
    }
    file.close();
    /*if (!file.good())
    {
        return false;
    }*/

    auto R = fr::Decompress(trans[0], dSize, rSize, dSize, nIter);
    auto G = fr::Decompress(trans[1], dSize, rSize, dSize, nIter);
    auto B = fr::Decompress(trans[2], dSize, rSize, dSize, nIter);
    auto name = fileName.substr(0, fileName.find('.'));
    MatricesToBmp(R, G, B, name);
    return true;
}

RGB fr::Decompress(const std::vector<std::vector<std::vector<fr::Transformation>>>& transformations, size_t dSize, size_t rSize, size_t step, size_t nIter)
{
    RGB rgb(3);
    rgb[0] = fr::Decompress(transformations[0], dSize, rSize, step, nIter);
    rgb[1] = fr::Decompress(transformations[1], dSize, rSize, step, nIter);
    rgb[2] = fr::Decompress(transformations[2], dSize, rSize, step, nIter);
    return rgb;
}

fr::ImageProps fr::Open(const std::string& fileName)
{
    fr::ImageProps props;
    std::ifstream file(fileName, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        return {};
    }
    file.read((char*)&props.width, sizeof(props.width));
    file.read((char*)&props.height, sizeof(props.height));
    file.read((char*)&props.dSize, sizeof(props.dSize));
    file.read((char*)&props.rSize, sizeof(props.rSize));
    file.read((char*)&props.step, sizeof(props.step));

    props.transformations.resize(3);
    for (auto& mat : props.transformations)
    {
        mat.resize(props.height / props.rSize);
        for (auto& row : mat)
        {
            row.resize(props.width / props.rSize);
        }
    }

    for (auto& mat : props.transformations)
    {
        for (auto& row : mat)
        {
            for (auto& tran : row)
            {
                uint16_t x;
                uint16_t y;
                uint8_t direction;
                uint8_t angle;
                float contrast;
                float brightness;
                file.read((char*)&x, sizeof(x));
                file.read((char*)&y, sizeof(y));
                file.read((char*)&direction, sizeof(direction));
                file.read((char*)&angle, sizeof(angle));
                file.read((char*)&contrast, sizeof(contrast));
                file.read((char*)&brightness, sizeof(brightness));
                tran.x = x;
                tran.y = y;
                tran.direction = direction;
                tran.angle = angle;
                tran.contrast = contrast;
                tran.brightness = brightness;
            }
        }
    }
    file.close();

    return props;
}

bool fr::Decompress(const std::string& fileName, const std::string& newFileName, size_t nIter)
{
    auto fullPath = SplitFullFileName(fileName);
    auto newFullPath = SplitFullFileName(newFileName);
    if (newFullPath[0].empty())
    {
        newFullPath[0] = fullPath[0];
    }
    std::ifstream file(fileName, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        return false;
    }
    uint32_t width;
    uint32_t height;
    uint32_t dSize;
    uint32_t rSize;
    uint32_t step;
    file.read((char*)&width, sizeof(width));
    file.read((char*)&height, sizeof(height));
    file.read((char*)&dSize, sizeof(dSize));
    file.read((char*)&rSize, sizeof(rSize));
    file.read((char*)&step, sizeof(step));

    std::vector<std::vector<std::vector<fr::Transformation>>> trans(3);
    for (auto& mat : trans)
    {
        mat.resize(height / rSize);
        for (auto& row : mat)
        {
            row.resize(width / rSize);
        }
    }

    for (auto& mat : trans)
    {
        for (auto& row : mat)
        {
            for (auto& tran : row)
            {
                uint16_t x;
                uint16_t y;
                uint8_t direction;
                uint8_t angle;
                float contrast;
                float brightness;
                file.read((char*)&x, sizeof(x));
                file.read((char*)&y, sizeof(y));
                file.read((char*)&direction, sizeof(direction));
                file.read((char*)&angle, sizeof(angle));
                file.read((char*)&contrast, sizeof(contrast));
                file.read((char*)&brightness, sizeof(brightness));
                tran.x = x;
                tran.y = y;
                tran.direction = direction;
                tran.angle = angle;
                tran.contrast = contrast;
                tran.brightness = brightness;
            }
        }
    }
    file.close();
    /*if (!file.good())
    {
        return false;
    }*/

    auto R = fr::Decompress(trans[0], dSize, rSize, step, nIter);
    auto G = fr::Decompress(trans[1], dSize, rSize, step, nIter);
    auto B = fr::Decompress(trans[2], dSize, rSize, step, nIter);
    MatricesToBmp(R, G, B, newFullPath[0] + newFullPath[1]);
    return true;
}


