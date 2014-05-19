// Little Polygon SDK
// Copyright (C) 2013 Max Kaufmann
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

// THIS IS STRICTLY A CONVENIENCE MODULE :P

#include "assets.h"
#include "events.h"
#include "graphics.h"
#include "sprites.h"
#include "splines.h"
#include "utils.h"

class GlobalContext : public Singleton<GlobalContext> {
public:

	SDL_Window *window;
	AssetBundle assets;
	Viewport view;
	Timer timer;
	TimerQueue queue;
	BasicPlotter plotter;
	LinePlotter lines;
	SpritePlotter sprites;
	SpriteBatch batch;
	SplinePlotter splines;
	

public:
	GlobalContext(const char *caption, int w, int h, const char *assetPath=0, int plotterCap=1024, int linesCap=128, int spriteLayers=8);
	~GlobalContext();
};

#define LPInit(...) (new GlobalContext(__VA_ARGS__))
#define LPDestroy() (delete GlobalContext::getInstancePtr())

#define gWindow  (GlobalContext::getInstance().window)
#define gAssets  (GlobalContext::getInstance().assets)
#define gView    (GlobalContext::getInstance().view)
#define gTimer   (GlobalContext::getInstance().timer)
#define gQueue   (GlobalContext::getInstance().queue)
#define gPlotter (GlobalContext::getInstance().plotter)
#define gLines   (GlobalContext::getInstance().lines)
#define gSprites (GlobalContext::getInstance().sprites)
#define gBatch   (GlobalContext::getInstance().batch)

