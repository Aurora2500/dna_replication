#include "assets_manager.hpp"

#include <SDL2/SDL.h>

namespace assets {

AssetsManager::AssetsManager() {
	char *base = SDL_GetBasePath();
	m_base_path = base;
}



}