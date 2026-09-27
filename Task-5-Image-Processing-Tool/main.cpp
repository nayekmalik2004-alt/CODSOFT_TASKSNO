#include <SFML/Graphics.hpp>
#include <windows.h>
#include <algorithm>
#include <cmath>
#include <string>
#include <cstdint>
#include <filesystem>

std::filesystem::path openImageDialog()
{
    wchar_t fileName[MAX_PATH] = L"";

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;

    ofn.lpstrFilter =
        L"Image Files\0*.png;*.jpg;*.jpeg;*.bmp;*.tga\0"
        L"All Files\0*.*\0";

    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn))
        return std::filesystem::path(fileName);

    return {};
}

std::filesystem::path saveImageDialog()
{
    wchar_t fileName[MAX_PATH] = L"processed.png";

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;

    ofn.lpstrFilter =
        L"PNG Image\0*.png\0"
        L"JPEG Image\0*.jpg\0"
        L"BMP Image\0*.bmp\0"
        L"All Files\0*.*\0";

    ofn.nFilterIndex = 1;
    ofn.lpstrDefExt = L"png";
    ofn.Flags = OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameW(&ofn))
        return std::filesystem::path(fileName);

    return {};
}

std::uint8_t clampColor(int value)
{
    return static_cast<std::uint8_t>(
        std::clamp(value, 0, 255)
    );
}

void applyGrayscale(sf::Image& image)
{
    sf::Vector2u size = image.getSize();

    for (unsigned int y = 0; y < size.y; ++y)
    {
        for (unsigned int x = 0; x < size.x; ++x)
        {
            sf::Color pixel = image.getPixel({x, y});

            int gray = static_cast<int>(
                0.299 * pixel.r +
                0.587 * pixel.g +
                0.114 * pixel.b
            );

            pixel.r = static_cast<std::uint8_t>(gray);
            pixel.g = static_cast<std::uint8_t>(gray);
            pixel.b = static_cast<std::uint8_t>(gray);

            image.setPixel({x, y}, pixel);
        }
    }
}

void adjustBrightness(sf::Image& image, int amount)
{
    sf::Vector2u size = image.getSize();

    for (unsigned int y = 0; y < size.y; ++y)
    {
        for (unsigned int x = 0; x < size.x; ++x)
        {
            sf::Color pixel = image.getPixel({x, y});

            pixel.r = clampColor(pixel.r + amount);
            pixel.g = clampColor(pixel.g + amount);
            pixel.b = clampColor(pixel.b + amount);

            image.setPixel({x, y}, pixel);
        }
    }
}

void adjustContrast(sf::Image& image, float amount)
{
    sf::Vector2u size = image.getSize();

    float factor =
        (259.0f * (amount + 255.0f)) /
        (255.0f * (259.0f - amount));

    for (unsigned int y = 0; y < size.y; ++y)
    {
        for (unsigned int x = 0; x < size.x; ++x)
        {
            sf::Color pixel = image.getPixel({x, y});

            int r = static_cast<int>(
                factor * (pixel.r - 128) + 128
            );

            int g = static_cast<int>(
                factor * (pixel.g - 128) + 128
            );

            int b = static_cast<int>(
                factor * (pixel.b - 128) + 128
            );

            pixel.r = clampColor(r);
            pixel.g = clampColor(g);
            pixel.b = clampColor(b);

            image.setPixel({x, y}, pixel);
        }
    }
}

void applyBlur(sf::Image& image)
{
    sf::Vector2u size = image.getSize();

    if (size.x < 3 || size.y < 3)
        return;

    sf::Image original = image;

    for (unsigned int y = 1; y < size.y - 1; ++y)
    {
        for (unsigned int x = 1; x < size.x - 1; ++x)
        {
            int red = 0;
            int green = 0;
            int blue = 0;

            for (int dy = -1; dy <= 1; ++dy)
            {
                for (int dx = -1; dx <= 1; ++dx)
                {
                    sf::Color p = original.getPixel({
                        static_cast<unsigned int>(
                            static_cast<int>(x) + dx
                        ),
                        static_cast<unsigned int>(
                            static_cast<int>(y) + dy
                        )
                    });

                    red += p.r;
                    green += p.g;
                    blue += p.b;
                }
            }

            sf::Color result(
                static_cast<std::uint8_t>(red / 9),
                static_cast<std::uint8_t>(green / 9),
                static_cast<std::uint8_t>(blue / 9),
                original.getPixel({x, y}).a
            );

            image.setPixel({x, y}, result);
        }
    }
}

void applySharpen(sf::Image& image)
{
    sf::Vector2u size = image.getSize();

    if (size.x < 3 || size.y < 3)
        return;

    sf::Image original = image;

    for (unsigned int y = 1; y < size.y - 1; ++y)
    {
        for (unsigned int x = 1; x < size.x - 1; ++x)
        {
            sf::Color center = original.getPixel({x, y});
            sf::Color top = original.getPixel({x, y - 1});
            sf::Color bottom = original.getPixel({x, y + 1});
            sf::Color left = original.getPixel({x - 1, y});
            sf::Color right = original.getPixel({x + 1, y});

            int red =
                5 * center.r -
                top.r -
                bottom.r -
                left.r -
                right.r;

            int green =
                5 * center.g -
                top.g -
                bottom.g -
                left.g -
                right.g;

            int blue =
                5 * center.b -
                top.b -
                bottom.b -
                left.b -
                right.b;

            sf::Color result(
                clampColor(red),
                clampColor(green),
                clampColor(blue),
                center.a
            );

            image.setPixel({x, y}, result);
        }
    }
}

void cropCenter(sf::Image& image)
{
    sf::Vector2u size = image.getSize();

    if (size.x < 20 || size.y < 20)
        return;

    unsigned int newWidth =
        static_cast<unsigned int>(size.x * 0.8f);

    unsigned int newHeight =
        static_cast<unsigned int>(size.y * 0.8f);

    unsigned int startX = (size.x - newWidth) / 2;
    unsigned int startY = (size.y - newHeight) / 2;

    sf::Image cropped(
        {newWidth, newHeight},
        sf::Color::Black
    );

    for (unsigned int y = 0; y < newHeight; ++y)
    {
        for (unsigned int x = 0; x < newWidth; ++x)
        {
            cropped.setPixel(
                {x, y},
                image.getPixel({
                    startX + x,
                    startY + y
                })
            );
        }
    }

    image = cropped;
}

void resizeHalf(sf::Image& image)
{
    sf::Vector2u size = image.getSize();

    if (size.x < 2 || size.y < 2)
        return;

    unsigned int newWidth = std::max(1u, size.x / 2);
    unsigned int newHeight = std::max(1u, size.y / 2);

    sf::Image resized(
        {newWidth, newHeight},
        sf::Color::Black
    );

    for (unsigned int y = 0; y < newHeight; ++y)
    {
        for (unsigned int x = 0; x < newWidth; ++x)
        {
            unsigned int sourceX =
                std::min(size.x - 1, x * 2);

            unsigned int sourceY =
                std::min(size.y - 1, y * 2);

            resized.setPixel(
                {x, y},
                image.getPixel({
                    sourceX,
                    sourceY
                })
            );
        }
    }

    image = resized;
}

bool isButtonClicked(
    const sf::Vector2i& mouse,
    const sf::RectangleShape& button
)
{
    return button.getGlobalBounds().contains(
        sf::Vector2f(
            static_cast<float>(mouse.x),
            static_cast<float>(mouse.y)
        )
    );
}

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({1200, 750}),
        "Task 5 - Image Processing Tool"
    );

    window.setFramerateLimit(60);

    sf::Font font;

    if (!font.openFromFile("arial.ttf"))
    {
        MessageBoxA(
            nullptr,
            "Could not load arial.ttf",
            "Error",
            MB_OK | MB_ICONERROR
        );

        return 1;
    }

    sf::Image image;
    sf::Image originalImage;

    sf::Texture texture;
    sf::Sprite sprite(texture);

    bool imageLoaded = false;

    sf::Text title(font, "IMAGE PROCESSING TOOL", 30);
    title.setPosition({25.f, 20.f});

    sf::Text status(
        font,
        "Load an image to begin",
        18
    );

    status.setPosition({25.f, 680.f});

    struct Button
    {
        sf::RectangleShape shape;
        sf::Text text;

        Button(
            sf::Font& font,
            const std::string& label,
            float x,
            float y
        )
            : shape({180.f, 45.f}),
              text(font, label, 18)
        {
            shape.setPosition({x, y});
            shape.setFillColor(sf::Color(60, 60, 70));
            shape.setOutlineColor(sf::Color(120, 120, 130));
            shape.setOutlineThickness(1.f);

            text.setPosition({x + 15.f, y + 10.f});
            text.setFillColor(sf::Color::White);
        }

        void draw(sf::RenderWindow& window)
        {
            window.draw(shape);
            window.draw(text);
        }
    };

    Button loadButton(font, "Load Image", 975.f, 80.f);
    Button saveButton(font, "Save As", 975.f, 135.f);
    Button resetButton(font, "Reset", 975.f, 190.f);
    Button grayButton(font, "Grayscale", 975.f, 245.f);
    Button blurButton(font, "Blur", 975.f, 300.f);
    Button sharpenButton(font, "Sharpen", 975.f, 355.f);
    Button brightPlusButton(font, "Brightness +", 975.f, 410.f);
    Button brightMinusButton(font, "Brightness -", 975.f, 465.f);
    Button contrastPlusButton(font, "Contrast +", 975.f, 520.f);
    Button contrastMinusButton(font, "Contrast -", 975.f, 575.f);
    Button cropButton(font, "Crop Center", 775.f, 635.f);
    Button resizeButton(font, "Resize 50%", 975.f, 635.f);

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (
                const auto* mouse =
                    event->getIf<sf::Event::MouseButtonPressed>()
            )
            {
                if (
                    mouse->button ==
                    sf::Mouse::Button::Left
                )
                {
                    sf::Vector2i mousePosition =
                        mouse->position;

                    if (
                        isButtonClicked(
                            mousePosition,
                            loadButton.shape
                        )
                    )
                    {
                        std::filesystem::path path =
                            openImageDialog();

                        if (!path.empty())
                        {
                            if (image.loadFromFile(path))
                            {
                                originalImage = image;

                                texture.loadFromImage(
                                    image
                                );

                                texture.setSmooth(true);
                                sprite = sf::Sprite(texture);
                                imageLoaded = true;

                                status.setString(
                                    "Image loaded successfully"
                                );
                            }
                            else
                            {
                                status.setString(
                                    "Failed to load image"
                                );
                            }
                        }
                    }
                    else if (
                        isButtonClicked(
                            mousePosition,
                            saveButton.shape
                        )
                    )
                    {
                        if (imageLoaded)
                        {
                            std::filesystem::path path =
                                saveImageDialog();

                            if (!path.empty())
                            {
                                if (image.saveToFile(path))
                                {
                                    status.setString(
                                        "Image saved successfully"
                                    );
                                }
                                else
                                {
                                    status.setString(
                                        "Failed to save image"
                                    );
                                }
                            }
                        }
                        else
                        {
                            status.setString(
                                "Load an image first"
                            );
                        }
                    }
                    else if (
                        isButtonClicked(
                            mousePosition,
                            resetButton.shape
                        )
                    )
                    {
                        if (imageLoaded)
                        {
                            image = originalImage;

                            texture.loadFromImage(
                                image
                            );

                            sprite = sf::Sprite(texture);

                            status.setString(
                                "Image reset to original"
                            );
                        }
                    }
                    else if (
                        isButtonClicked(
                            mousePosition,
                            grayButton.shape
                        )
                    )
                    {
                        if (imageLoaded)
                        {
                            applyGrayscale(image);

                            texture.loadFromImage(
                                image
                            );

                            sprite = sf::Sprite(texture);

                            status.setString(
                                "Grayscale filter applied"
                            );
                        }
                    }
                    else if (
                        isButtonClicked(
                            mousePosition,
                            blurButton.shape
                        )
                    )
                    {
                        if (imageLoaded)
                        {
                            applyBlur(image);

                            texture.loadFromImage(
                                image
                            );

                            sprite = sf::Sprite(texture);

                            status.setString(
                                "Blur filter applied"
                            );
                        }
                    }
                    else if (
                        isButtonClicked(
                            mousePosition,
                            sharpenButton.shape
                        )
                    )
                    {
                        if (imageLoaded)
                        {
                            applySharpen(image);

                            texture.loadFromImage(
                                image
                            );

                            sprite = sf::Sprite(texture);

                            status.setString(
                                "Sharpen filter applied"
                            );
                        }
                    }
                    else if (
                        isButtonClicked(
                            mousePosition,
                            brightPlusButton.shape
                        )
                    )
                    {
                        if (imageLoaded)
                        {
                            adjustBrightness(
                                image,
                                20
                            );

                            texture.loadFromImage(
                                image
                            );

                            sprite = sf::Sprite(texture);

                            status.setString(
                                "Brightness increased"
                            );
                        }
                    }
                    else if (
                        isButtonClicked(
                            mousePosition,
                            brightMinusButton.shape
                        )
                    )
                    {
                        if (imageLoaded)
                        {
                            adjustBrightness(
                                image,
                                -20
                            );

                            texture.loadFromImage(
                                image
                            );

                            sprite = sf::Sprite(texture);

                            status.setString(
                                "Brightness decreased"
                            );
                        }
                    }
                    else if (
                        isButtonClicked(
                            mousePosition,
                            contrastPlusButton.shape
                        )
                    )
                    {
                        if (imageLoaded)
                        {
                            adjustContrast(
                                image,
                                30.0f
                            );

                            texture.loadFromImage(
                                image
                            );

                            sprite = sf::Sprite(texture);

                            status.setString(
                                "Contrast increased"
                            );
                        }
                    }
                    else if (
                        isButtonClicked(
                            mousePosition,
                            contrastMinusButton.shape
                        )
                    )
                    {
                        if (imageLoaded)
                        {
                            adjustContrast(
                                image,
                                -30.0f
                            );

                            texture.loadFromImage(
                                image
                            );

                            sprite = sf::Sprite(texture);

                            status.setString(
                                "Contrast decreased"
                            );
                        }
                    }
                    else if (
                        isButtonClicked(
                            mousePosition,
                            cropButton.shape
                        )
                    )
                    {
                        if (imageLoaded)
                        {
                            cropCenter(image);

                            texture.loadFromImage(
                                image
                            );

                            sprite = sf::Sprite(texture);

                            status.setString(
                                "Center crop applied"
                            );
                        }
                    }
                    else if (
                        isButtonClicked(
                            mousePosition,
                            resizeButton.shape
                        )
                    )
                    {
                        if (imageLoaded)
                        {
                            resizeHalf(image);

                            texture.loadFromImage(
                                image
                            );

                            sprite = sf::Sprite(texture);

                            status.setString(
                                "Image resized to 50%"
                            );
                        }
                    }
                }
            }
        }

        window.clear(sf::Color(30, 30, 35));

        window.draw(title);

        sf::RectangleShape previewArea({920.f, 590.f});
        previewArea.setPosition({25.f, 75.f});
        previewArea.setFillColor(sf::Color(45, 45, 50));
        previewArea.setOutlineColor(sf::Color(100, 100, 105));
        previewArea.setOutlineThickness(2.f);

        window.draw(previewArea);

        if (imageLoaded)
        {
            sf::Vector2u imageSize = image.getSize();

            float scaleX =
                880.f /
                static_cast<float>(imageSize.x);

            float scaleY =
                540.f /
                static_cast<float>(imageSize.y);

            float scale =
                std::min(scaleX, scaleY);

            sprite.setScale({scale, scale});

            float displayedWidth =
                static_cast<float>(imageSize.x) *
                scale;

            float displayedHeight =
                static_cast<float>(imageSize.y) *
                scale;

            float posX =
                25.f +
                (920.f - displayedWidth) / 2.f;

            float posY =
                75.f +
                (590.f - displayedHeight) / 2.f;

            sprite.setPosition({posX, posY});

            window.draw(sprite);
        }

        loadButton.draw(window);
        saveButton.draw(window);
        resetButton.draw(window);
        grayButton.draw(window);
        blurButton.draw(window);
        sharpenButton.draw(window);
        brightPlusButton.draw(window);
        brightMinusButton.draw(window);
        contrastPlusButton.draw(window);
        contrastMinusButton.draw(window);
        cropButton.draw(window);
        resizeButton.draw(window);

        window.draw(status);

        window.display();
    }

    return 0;
}