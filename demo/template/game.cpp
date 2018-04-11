/*
 *  Copyright (C) 2016 Simon Boyé
 *
 *  This file is part of lair.
 *
 *  lair is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  lair is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with lair.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <lair/geometry/shape_2d.h>

#include <lair/render_gl3/texture_set.h>

#include "main_state.h"

#include "game.h"


// This function is declared in an auto-generated file
void registerTemplateResource(lair::MemoryFileSystem& fs);


GameConfig::GameConfig()
	: GameConfigBase()
{
}

void GameConfig::setFromArgs(int& argc, char** argv) {
	GameConfigBase::setFromArgs(argc, argv);
}

const PropertyList& GameConfig::properties() const {
	return GameConfig::staticProperties();
}

const PropertyList& GameConfig::staticProperties() {
	return GameConfigBase::staticProperties();
}



Game::Game(int argc, char** argv)
    : GameBase(argc, argv),
      _mainState() {
	serializer().registerType<Shape2D>(
	            static_cast<bool(*)(LdlParser&, Shape2D&)>(ldlRead),
	            static_cast<bool(*)(LdlWriter&, const Shape2D&)>(ldlWrite));
	serializer().registerType<Shape2DVector>(
	            static_cast<bool(*)(LdlParser&, Shape2DVector&)>(ldlRead),
	            static_cast<bool(*)(LdlWriter&, const Shape2DVector&)>(ldlWrite));

	serializer().registerType<TextureSetCSP>(
	    [this](LdlParser& parser, TextureSetCSP& ts) {
		    return ldlRead(parser, ts, _renderer, _loader.get());
	    },
	    static_cast<bool(*)(LdlWriter&, const TextureSetCSP&)>(ldlWrite)
	);
}


Game::~Game() {
}


void Game::initialize() {
	GameBase::initialize(_config);

	registerTemplateResource(*_memoryFs);

	typedef std::pair<const lair::Path*, const MemFile*> File;
	std::vector<File> files;
	for(const auto& pathFile: *_memoryFs) {
		files.emplace_back(&pathFile.first, &pathFile.second);
	}
	std::sort(files.begin(), files.end(), [](const File& pf0, const File& pf1) {
		return *pf0.first < *pf1.first;
	});
	for(const File& pathFile: files) {
		log().info("Builtin file: ", *pathFile.first, " (", pathFile.second->size, "B)");
	}

	window()->setUtf8Title("Lair - template");

	_mainState.reset(new MainState(this));
	_mainState->initialize();
}


void Game::shutdown() {
	_mainState->shutdown();
	_mainState.reset();  // Required to ensure that everything is freed

	GameBase::shutdown();
}


GameConfig& Game::config() {
	return _config;
}


MainState* Game::mainState() {
	return _mainState.get();
}
