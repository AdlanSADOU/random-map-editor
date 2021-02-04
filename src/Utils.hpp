#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

namespace zz
{
        struct Controls
    {
        bool up = false;
        bool down = false;
        bool left = false;
        bool right = false;
        bool shift = false;
        bool space = false;
    };

    class Image
    {
    private:
        sf::Sprite _sprite;
        sf::Texture _texture;
        sf::Clock _clock;
        sf::IntRect _subRect;

    public:
        Image(std::string imageFile, sf::IntRect rect = {})
        {
            setImage(imageFile);
            this->_subRect = rect;
            setSubRect(rect);
        }

        Image() {}
        ~Image() {}

        bool setImage(std::string imageFile)
        {
            if (!this->_texture.loadFromFile(imageFile))
            {
                std::cout << "could not load image" << std::endl;
                return false;
            }

            this->_sprite.setTexture(this->_texture);
            return true;
        }

        void setSubRect(sf::IntRect rect)
        {
            this->_sprite.setTextureRect(rect);
        }

        sf::Sprite &getSprite()
        {
            return this->_sprite;
        }
        sf::Texture &getTexture()
        {
            return this->_texture;
        }
        void setScale(sf::Vector2f factors) {
            this->_sprite.setScale(factors);
        }

        void render(sf::RenderTarget &target)
        {
            target.draw(this->_sprite);
        }
    };

    class Text : sf::Text
    {
    private:
        sf::Font font;

    public:
        Text(std::string fontPath = "./res/Semi-Coder-Regular.otf")
        {
            this->setFont(fontPath);
        }

        ~Text(){};

        void setFont(std::string fontPath)
        {
            if (!this->font.loadFromFile(fontPath))
                ;
            std::cerr << "could not load font: " << fontPath << std::endl;
        }
    };

} // namespace zz
