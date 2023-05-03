//Components.h

//Has the structs for all component types to be used by ECSElement
//Included in ECSElement.

#pragma once

#include <string>
#include "resource/ResourceElement.h"

struct SpriteComponent {
    std::shared_ptr<TextureResource> textureResource;
    float x;
    float y;
    float width;
    float height;
    float rotation;

    SpriteComponent(const std::shared_ptr<TextureResource>& textureResource, float x, float y, float width, float height, float rotation = 0)
        : textureResource(textureResource), x(x), y(y), width(width), height(height), rotation(rotation) {}
};