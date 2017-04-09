#pragma once
#ifndef AUDIO_MANAGER_RENDERER_H
#define AUDIO_MANAGER_RENDERER_H

#include "../audio.h"
#include "../game.h"

// Renders debug rectangles along the bottom of the screen showing
// the _readHead and _writeHead positions.
class AudioManager::DEBUG_Renderer 
	: public Renderable
{
	virtual void Draw(Game &g) override;
	virtual RenderOrder RequestedDrawOrder() override;
};

#endif // AUDIO_MANAGER_RENDERER_H
