#include "audioManagerRenderer.h"

void
AudioManager::DEBUG_Renderer::Draw(Game &g)
{
	const auto width = 10;
	const auto height = 30;

	const auto y = 120 * 1080 / g.Screen.Height();
	const auto x1 = g.Audio._writeHead % bufSize * g.Screen.Width() / bufSize;
	const auto x2 = g.Audio._readHead % bufSize * g.Screen.Width() / bufSize;

	g.Screen.DrawRectangle({ x1, y, width, height }, Colors::Blue);
	g.Screen.DrawRectangle({ x2, y, width, height }, Colors::Red);
}

RenderOrder
AudioManager::DEBUG_Renderer::RequestedDrawOrder()
{
	return RenderOrder{ INT16_MAX };
}