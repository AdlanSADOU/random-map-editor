#pragma once
#include <iostream>

#include "Animation.hpp"
#include "Map.hpp"
#include "SFML/Graphics.hpp"
#include "Utils.hpp"

class Character : public sf::Sprite
{
    enum AnimState
    {
        IDLE,
        WALK_UP,
        WALK_LEFT,
        WALK_DOWN,
        WALK_RIGHT,
        RUN
    };

    sf::Texture texture;

    Animation _animWalkUp;
    Animation _animWalkLeft;
    Animation _animWalkDown;
    Animation _animWalkRight;
    AnimState animState;

    sf::Vector2f topPointPosL = {};
    sf::Vector2f topPointPosR = {};
    sf::Vector2f leftPointPosU = {};
    sf::Vector2f leftPointPosD = {};
    sf::Vector2f downPointPosL = {};
    sf::Vector2f downPointPosR = {};
    sf::Vector2f rightPointPosU = {};
    sf::Vector2f rightPointPosD = {};

  public:
  public:
    void create();
    void update(zz::Controls c, Map &map);
    void renderDebugInfo(sf::RenderWindow &w);
};

/**
 * Character implementation
 */
inline void Character::create()
{
    this->texture.loadFromFile("./res/characters/skeleton.png");
    this->setTexture(this->texture);
    this->setTextureRect({0, 64 * 18, 64, 64});
    this->setScale({0.6f, 0.6f});

    Animation::AnimData walkUpAnimData{
        ._sprite = (this),
        ._texture = &(this->texture),
        ._playRate = 50,
        .frameData{
            .firstFrameY = 64 * 8,
            .firstFrameX = 64,
            .stride = 64,
            .nbOfFrames = 9,
            .width = 64,
            .height = 64},
    };

    Animation::AnimData walkLeftAnimData{
        ._sprite = (this),
        ._texture = &(this->texture),
        ._playRate = 50,
        .frameData{
            .firstFrameY = 64 * 9,
            .firstFrameX = 64,
            .stride = 64,
            .nbOfFrames = 9,
            .width = 64,
            .height = 64},
    };

    Animation::AnimData walkDownAnimData{
        ._sprite = (this),
        ._texture = &(this->texture),
        ._playRate = 50,
        .frameData{
            .firstFrameY = 64 * 10,
            .firstFrameX = 64,
            .stride = 64,
            .nbOfFrames = 9,
            .width = 64,
            .height = 64},
    };

    Animation::AnimData walkRightAnimData{
        ._sprite = (this),
        ._texture = &(this->texture),
        ._playRate = 50,
        .frameData{
            .firstFrameY = 64 * 11,
            .firstFrameX = 64,
            .stride = 64,
            .nbOfFrames = 9,
            .width = 64,
            .height = 64},
    };

    this->_animWalkUp.create("walk_up", walkUpAnimData);
    this->_animWalkLeft.create("walk_left", walkLeftAnimData);
    this->_animWalkDown.create("walk_down", walkDownAnimData);
    this->_animWalkRight.create("walk_right", walkRightAnimData);
}

inline void Character::update(zz::Controls c, Map &map)
{
    static AnimState lastState = IDLE;
    switch (animState) {
    case IDLE:
        if (lastState == WALK_UP)
            this->setTextureRect({0, 64 * 8, 64, 64});
        if (lastState == WALK_LEFT)
            this->setTextureRect({0, 64 * 9, 64, 64});
        if (lastState == WALK_DOWN)
            this->setTextureRect({0, 64 * 10, 64, 64});
        if (lastState == WALK_RIGHT)
            this->setTextureRect({0, 64 * 11, 64, 64});

        break;
    case WALK_UP:
        lastState = WALK_UP;
        _animWalkUp.play();
        break;
    case WALK_LEFT:
        lastState = WALK_LEFT;
        _animWalkLeft.play();
        break;
    case WALK_DOWN:
        lastState = WALK_DOWN;
        _animWalkDown.play();
        break;
    case WALK_RIGHT:
        lastState = WALK_RIGHT;
        _animWalkRight.play();
        break;
    case RUN:
        break;

    default:
        break;
    }

    static sf::Vector2f upSpeed = {0, -4};
    static sf::Vector2f leftSpeed = {-4, 0};
    static sf::Vector2f downSpeed = {0, +4};
    static sf::Vector2f rightSpeed = {+4, 0};
    static sf::Vector2f zero = {0, 0};
    static float speed = 1.f;

    auto bounds = this->getGlobalBounds();
    topPointPosL = {bounds.left - 6 + bounds.width / 2, bounds.top + 18};
    topPointPosR = {bounds.left + 4 + bounds.width / 2, bounds.top + 18};

    leftPointPosU = {bounds.left + 8, bounds.top + bounds.height - 12};
    leftPointPosD = {bounds.left + 8, bounds.top + bounds.height - 4};

    downPointPosL = {bounds.left - 6 + bounds.width / 2, bounds.top + bounds.height + 2};
    downPointPosR = {bounds.left + 4 + bounds.width / 2, bounds.top + bounds.height + 2};

    rightPointPosU = {bounds.left + bounds.width - 8, bounds.top + bounds.height - 12};
    rightPointPosD = {bounds.left + bounds.width - 8, bounds.top + bounds.height - 4};

    // for (auto &&w : map.wallTiles) {
    //     if (w.contains(topPointPosL) ||
    //         w.contains(topPointPosR))
    //         upSpeed = zero;
    //     if (w.contains(leftPointPosU) ||
    //         w.contains(leftPointPosD))
    //         leftSpeed = zero;
    //     if (w.contains(downPointPosL) ||
    //         w.contains(downPointPosR))
    //         downSpeed = zero;
    //     if (w.contains(rightPointPosU) ||
    //         w.contains(rightPointPosD))
    //         rightSpeed = zero;
    // }

    animState = IDLE;
    if (c.up) {
        animState = WALK_UP;
        this->move(upSpeed);
    }
    if (c.left) {
        animState = WALK_LEFT;
        this->move(leftSpeed);
    }
    if (c.down) {
        animState = WALK_DOWN;
        this->move(downSpeed);
    }
    if (c.right) {
        animState = WALK_RIGHT;
        this->move(rightSpeed);
    }

    // Reset our speeds to defauts
    upSpeed = {0, -4};
    leftSpeed = {-4, 0};
    downSpeed = {0, +4};
    rightSpeed = {+4, 0};
}

inline void Character::renderDebugInfo(sf::RenderWindow &w)
{

    sf::CircleShape topPointL = sf::CircleShape(2, 4);
    sf::CircleShape topPointR = sf::CircleShape(2, 4);
    sf::CircleShape leftPointU = sf::CircleShape(2, 4);
    sf::CircleShape leftPointD = sf::CircleShape(2, 4);
    sf::CircleShape downPointL = sf::CircleShape(2, 4);
    sf::CircleShape downPointR = sf::CircleShape(2, 4);
    sf::CircleShape rightPointU = sf::CircleShape(2, 4);
    sf::CircleShape rightPointD = sf::CircleShape(2, 4);

    topPointL.setFillColor(sf::Color::Red);
    topPointL.setPosition(topPointPosL);
    w.draw(topPointL);
    topPointR.setFillColor(sf::Color::Red);
    topPointR.setPosition(topPointPosR);
    w.draw(topPointR);

    leftPointU.setFillColor(sf::Color::Red);
    leftPointU.setPosition(leftPointPosU);
    w.draw(leftPointU);
    leftPointD.setFillColor(sf::Color::Red);
    leftPointD.setPosition(leftPointPosD);
    w.draw(leftPointD);

    rightPointU.setFillColor(sf::Color::Red);
    rightPointU.setPosition(rightPointPosU);
    w.draw(rightPointU);
    rightPointD.setFillColor(sf::Color::Red);
    rightPointD.setPosition(rightPointPosD);
    w.draw(rightPointD);

    downPointL.setFillColor(sf::Color::Red);
    downPointL.setPosition(downPointPosL);
    w.draw(downPointL);
    downPointR.setFillColor(sf::Color::Red);
    downPointR.setPosition(downPointPosR);
    w.draw(downPointR);
}