#include <iostream>
#include <chrono>
#include <filesystem>

#include <imgui.h>
#include <imgui-sfml.h>
#include <imfilebrowser.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "../WaveletCompress/wavelet.h"
#include "../FractalCompress/fractal.h"

namespace im = ImGui;

constexpr size_t FILE_PATH_LEN = 256;
const char* METHODS[] = { "Wavelet", "Fractal" };

size_t GetFileSize(const std::string& fileName);

std::string convert_wchar_to_string(const wchar_t* wchar_str) {
    int len = WideCharToMultiByte(CP_UTF8, 0, wchar_str, -1, NULL, 0, NULL, NULL);
    std::vector<char> buf(len);
    WideCharToMultiByte(CP_UTF8, 0, wchar_str, -1, buf.data(), len, NULL, NULL);
    std::string str(buf.begin(), buf.end());
    if (str[str.size() - 1] == '\0')
        str.resize(str.size() - 1);
    return str;
}

class UI
{
public:
    UI(const char* title, sf::Vector2u windowSize);
    void Run();

private:
    bool OpenAndShowImage(const std::string& filePath);
    void AvgPixelMist();

private:
    sf::Vector2u _windowSize;
    ImVec2 _imgSize;
    sf::RenderWindow _window;
    sf::Color _color;
    sf::Image _noImageBG;
    sf::Texture _img;
    sf::Texture _compImg;
    std::string _filePath;
    std::string _newFilePath;
    im::FileBrowser _fileDialog;
    std::string _originalFileName;
    std::string _compressedFileName;
    sf::Clock _deltaClock;
    std::chrono::milliseconds _compressingTime = std::chrono::milliseconds(0);
    std::chrono::milliseconds _decompressingTime = std::chrono::milliseconds(0);
    size_t _originalFileSize = 0;
    size_t _compressedFileSize = 0;
    int _selectedItem = 0;
    int _nIterWv = 4;
    int _nIterFr = 8;
    int _dSize = 16;
    int _rSize = 8;
    int _step = 8;
    int _fdMode = 0;
    float _buttonWidth = 70.0;
    float quantCoef = 99.999;
    float _avgPixelMist = 0.0;
    bool _isImgSet = false;
    bool _isCompImgSet = false;
};

int main()
{    
    auto windowSize = sf::Vector2u(1200, 800);
    auto title = "ImgCompress";
    UI ui(title, windowSize);
    ui.Run();
    return 0;
}



UI::UI(const char* title, sf::Vector2u windowSize) : _windowSize(windowSize), _imgSize((float)_windowSize.x / 2 - 10, (float)_windowSize.x / 2 - 20)
{
    _noImageBG.create((unsigned)_imgSize.x - 10, (unsigned)_imgSize.y - 10, { 0, 0, 0 });
    _filePath = std::string(FILE_PATH_LEN, '\0');
    _newFilePath = std::string(FILE_PATH_LEN, '\0');
    _fileDialog.SetTitle("Select an image");
    _fileDialog.SetTypeFilters({ ".bmp", ".fr", ".wv" });
    //im::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowBorderSize, 0.);
    //im::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
    _window.create(sf::VideoMode(windowSize.x, windowSize.y), title);
    _window.setVerticalSyncEnabled(true);
    im::SFML::Init(_window);
}


void UI::Run()
{
    while (_window.isOpen()) {
        sf::Event event;
        while (_window.pollEvent(event)) {
            im::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                _window.close();
            }
        }

        im::SFML::Update(_window, _deltaClock.restart());
        // Draw UI here

        // The main widget
        im::Begin("MainWidget", (bool*)0, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_::ImGuiWindowFlags_NoSavedSettings/*| ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding*/);
        im::SetWindowPos({ 0, 0 });
        im::SetWindowSize(im::GetMainViewport()->Size);
        //

        // Image area
        im::BeginChild("Original image", _imgSize, true);
        if (_isImgSet)
        {
            im::Image(_img, { _imgSize.x - 10, _imgSize.y - 10 });
        }
        im::EndChild(); im::SameLine();

        im::BeginChild("Compressed image", _imgSize, true);
        if (_isCompImgSet)
        {
            im::Image(_compImg, { _imgSize.x - 10, _imgSize.y - 10 });
        }
        im::EndChild();
        // End of image area


        im::BeginChild("OriginalImageProperties", { (float)_window.getSize().x / 2 - 10, 50.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove);
        im::Text(_originalFileName.c_str());
        im::SameLine(0, 20);
        im::Text((std::to_string(_originalFileSize) + " bytes").c_str());
        im::Text((std::to_string(_compressingTime.count()) + " ms").c_str());        
        im::EndChild();
        im::SameLine();

        im::BeginChild("CompressedImageProperties", { (float)_window.getSize().x / 2 - 10, 50.f }, true, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove);
        im::Text(_compressedFileName.c_str());
        im::SameLine(0, 20);
        im::Text((std::to_string(_compressedFileSize) + " bytes").c_str());
        im::Text((std::to_string(_decompressingTime.count()) + " ms").c_str());
        im::SameLine(0, 20);
        im::Text((std::to_string(_avgPixelMist) + " % mist").c_str());
        im::EndChild();


        // Input area
        im::BeginChild("", { (float)_window.getSize().x - 12, (float)_window.getSize().y - _imgSize.y - 70 }, true, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove);

        im::InputText("selected image", _filePath.data(), FILE_PATH_LEN);

        if (_fileDialog.HasSelected())
        {            
            _filePath = convert_wchar_to_string(_fileDialog.GetSelected().c_str());
            auto fullPath = SplitFullFileName(_filePath);
            _newFilePath = fullPath[1];
            _fileDialog.ClearSelected();
        }

        if (im::Button("Browse", { _buttonWidth, im::GetTextLineHeightWithSpacing() }))
        {
            _fdMode = 1;
            _fileDialog.Open();
        } im::SameLine();

        if (im::Button("Open", { _buttonWidth, im::GetTextLineHeightWithSpacing() }))
        {
            OpenAndShowImage(_filePath);
        }

        im::InputText("new image name", _newFilePath.data(), FILE_PATH_LEN);

        std::vector<std::string> fullFilePath;
        std::vector<std::string> fullNewFilePath;
        try
        {
            fullFilePath = SplitFullFileName(_filePath);
            fullNewFilePath = SplitFullFileName(_newFilePath);
            if (fullNewFilePath[0].empty())
                fullNewFilePath[0] = fullFilePath[0];
        }
        catch (std::exception&) {}

        if (_filePath.ends_with(".bmp"))
        {
            im::Combo("Compression method", &_selectedItem, METHODS, 2);
            if (_selectedItem == 0)
            {
                im::InputInt("number of iterations", &_nIterWv);
                im::InputFloat("Quantizing threshold", &quantCoef);

                if (im::Button("Compress", { _buttonWidth, im::GetTextLineHeightWithSpacing() }))
                {
                    //read data
                    auto ycbcr = rgbToYcbcr2(rgbDoubleFromBmp(_filePath));
                    //process data
                    auto start = std::chrono::high_resolution_clock::now();
                    wv::Compress(ycbcr, quantCoef, _nIterWv);
                    _compressingTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
                    //write data
                    wv::Save(ycbcr, _nIterWv, fullNewFilePath[0] + fullNewFilePath[1] + ".wv");

                    OpenAndShowImage(fullNewFilePath[0] + fullNewFilePath[1] + ".wv");
                    AvgPixelMist();
                }
            }
            if (_selectedItem == 1)
            {
                im::InputInt("Domen block size", &_dSize);
                im::InputInt("Range block size", &_rSize);
                im::InputInt("Step", &_step);

                if (im::Button("Compress", { _buttonWidth, im::GetTextLineHeightWithSpacing() }))
                {
                    //read data
                    auto rgb = rgbDoubleFromBmp(_filePath);
                    //process data
                    auto start = std::chrono::high_resolution_clock::now();
                    auto trans = fr::Compress(rgb, _dSize, _rSize, _step);
                    _compressingTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
                    //write data
                    fr::Save(size(rgb[0][0]), size(rgb[0]), _dSize, _rSize, _step, trans, fullNewFilePath[0] + fullNewFilePath[1] + ".fr");

                    OpenAndShowImage(fullNewFilePath[0] + fullNewFilePath[1] + ".fr");
                    AvgPixelMist();
                }
            }
        }
        if (_filePath.ends_with(".fr"))
        {
            im::InputInt("number of iterations", &_nIterFr);

            if (im::Button("Decompress", { _buttonWidth, im::GetTextLineHeightWithSpacing() }))
            {
                //read data
                auto props = fr::Open(_filePath);
                //process data
                auto start = std::chrono::high_resolution_clock::now();
                auto rgb = fr::Decompress(props.transformations, props.dSize, props.rSize, _step, _nIterFr);
                _decompressingTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
                //write data
                rgbToBmp(rgb, fullNewFilePath[0] + fullNewFilePath[1] + ".bmp");
            }
        }
        if (_filePath.ends_with(".wv"))
        {
            if (im::Button("Decompress", { _buttonWidth, im::GetTextLineHeightWithSpacing() }))
            {
                //read data
                auto data = std::move(wv::Open(_filePath));
                //process data
                auto start = std::chrono::high_resolution_clock::now();
                wv::Decompress(data.first, data.second);
                _decompressingTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
                //write data
                rgbToBmp(ycbcrToRgb2(data.first), fullNewFilePath[0] + fullNewFilePath[1] + ".bmp");
            }
        }
        im::EndChild(); // End of the input area

        im::End(); // End of the main widget

        // End of UI drawing
        _fileDialog.Display();
        _window.clear(sf::Color::Black); // заполняем окно заданным цветом
        im::SFML::Render(_window);
        _window.display();
    }
    im::SFML::Shutdown();
}


bool UI::OpenAndShowImage(const std::string& filePath)
{
    bool result = false;
    auto fullFilePath = SplitFullFileName(filePath);


    if (filePath.ends_with(".bmp"))
    {
        if (_img.loadFromFile(filePath))
        {
            _originalFileName = _compressedFileName = fullFilePath[1] + fullFilePath[2];
            _originalFileSize = GetFileSize(filePath);
            _isImgSet = true;
        }
        else
        {
            _originalFileName = "Unable to open file!";
            _originalFileSize = 0;
            _isImgSet = false;
            result = false;
        }
    }
    if (filePath.ends_with(".wv"))
    {
        auto start = std::chrono::high_resolution_clock::now();
        if (wv::Decompress(filePath, fullFilePath[0] + "tmp_" + fullFilePath[1]))
        {
            _decompressingTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
            if (_compImg.loadFromFile(fullFilePath[0] + "tmp_" + fullFilePath[1] + ".bmp"))
            {
                _compressedFileName = fullFilePath[1] + fullFilePath[2];
                _compressedFileSize = GetFileSize(filePath);
                _isCompImgSet = true;
            }
            else
            {
                _compressedFileName = "Unable to open decompressed file!";
                _compressedFileSize = 0;
                _isCompImgSet = false;
                result = false;
            }
            std::system(("del " + fullFilePath[0] + "tmp_" + fullFilePath[1] + ".bmp").c_str());
        }
        else
        {
            _decompressingTime = std::chrono::milliseconds(0);
            _compressedFileSize = 0;
            _compressedFileName = "Decompression error!";
            result = false;
        }
        
    }
    if (filePath.ends_with(".fr"))
    {
        auto start = std::chrono::high_resolution_clock::now();
        if (fr::Decompress(filePath, fullFilePath[0] + "tmp_" + fullFilePath[1], _nIterFr))
        {
            _decompressingTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
            if (_compImg.loadFromFile(fullFilePath[0] + "tmp_" + fullFilePath[1] + ".bmp"))
            {
                _compressedFileName = fullFilePath[1] + fullFilePath[2];
                _compressedFileSize = GetFileSize(filePath);
                _isCompImgSet = true;
            }
            else
            {
                _compressedFileName = fullFilePath[1] + fullFilePath[2];
                _compressedFileSize = 0;
                _isCompImgSet = false;
                result = false;
            }
            std::system(("del " + fullFilePath[0] + "tmp_" + fullFilePath[1] + ".bmp").c_str());
        }
        else
        {
            _decompressingTime = std::chrono::milliseconds(0);
            _compressedFileSize = 0;
            _compressedFileName = "Decompression error!";
            result = false;
        }
    }
    return result;
}

void UI::AvgPixelMist()
{
    auto img = _img.copyToImage();
    auto compImg = _compImg.copyToImage();
    auto pImg = img.getPixelsPtr();
    auto pCompImg = compImg.getPixelsPtr();
    auto byteSize = img.getSize().x * img.getSize().y * 3;
    int64_t sum = 0;
    for (size_t i = 0; i < byteSize; i += 4)
    {
        sum += abs((int)pCompImg[i] - (int)pImg[i]);
        sum += abs((int)pCompImg[i+1] - (int)pImg[i + 1]);
        sum += abs((int)pCompImg[i+2] - (int)pImg[i + 2]);
    }
    _avgPixelMist = (double)sum / byteSize / 255 * 100;
}

size_t GetFileSize(const std::string& fileName)
{
    std::filesystem::path p(fileName);
    return std::filesystem::file_size(p);
}

