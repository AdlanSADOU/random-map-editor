#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

class Animation
{
public:
    struct FrameData
    {
        int firstFrameY = 0;
        int firstFrameX = 0;
        int stride = 0;
        int nbOfFrames = 0;
        int width = 0;
        int height = 0;
    };

    struct AnimData
    {
        sf::Sprite *_sprite;
        sf::Texture *_texture;
        float _playRate = 0;
        FrameData frameData = {0};
    };

private:
    std::vector<sf::IntRect> rects;
    sf::Clock _clock;
    AnimData _animData;
    std::string _name;

public:
    Animation(std::string name, AnimData data)
    {
        create(name, data);
    }
    Animation(){};
    ~Animation(){};

    void create(std::string name, AnimData data);
    void play();
};

/**
 * Animation implementation
 */

inline void Animation::create(std::string name, AnimData data)
{
    this->_name = name;
    this->_animData = data;
    this->_clock.restart();

    const FrameData f = this->_animData.frameData;
    for (int i = 0; i <= f.nbOfFrames; i++)
    {
        sf::IntRect rect;
        if (f.firstFrameX == 0)
        {
            rect = {
                {(f.firstFrameX + i) * f.stride, f.firstFrameY},
                {f.width, f.height}};
        }
        else
        {
            rect = {
                {(f.firstFrameX) + f.stride * i, f.firstFrameY},
                {f.width, f.height}};
        }
        this->rects.push_back(rect);
    }
}

inline void Animation::play()
{
    static int frame = 0;
    static bool forward = true;
    static sf::IntRect &rect = this->rects.back();

    if (this->_clock.getElapsedTime().asMilliseconds() >= this->_animData._playRate)
    {
        this->_clock.restart();

        if (forward)
        {
            this->_animData._sprite->setTextureRect(this->rects[frame]);
            ++frame;
        }

        else if (!forward)
        {
            --frame;
            this->_animData._sprite->setTextureRect(this->rects[frame]);
        }

        // if playing backward, frame should not be assigned to 0 here
        if ((frame = frame % (this->_animData.frameData.nbOfFrames - 1)) == 0)
        {
            // forward = !forward;
        }
    }
}