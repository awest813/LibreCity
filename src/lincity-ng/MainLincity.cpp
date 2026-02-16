/* ---------------------------------------------------------------------- *
 * src/lincity-ng/MainLincity.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      David Kamphausen <david.kamphausen@web.de>
 * Copyright (C) 2025      David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#include "MainLincity.hpp"

#include <stdlib.h>                       // for srand
#include <time.h>                         // for time
#include <cassert>                        // for assert
#include <iostream>                       // for char_traits, basic_ostream
#include <stdexcept>                      // for runtime_error
#include <string>                         // for basic_string, operator+
#include <fmt/format.h>
#include <fmt/std.h> // IWYU pragma: keep

#include "TimerInterface.hpp"             // for reset_start_time
#include "gui/DialogBuilder.hpp"          // for DialogBuilder
#include "lincity/lin-city.hpp"             // for SIM_DELAY_SLOW
#include "lincity/modules/all_modules.hpp"  // for initializeModules
#include "lincity/world.hpp"                // for World
#include "util/gettextutil.hpp"

extern void init_types(void);
extern void initFactories(void);

int simDelay = SIM_DELAY_SLOW;
/******************************************/

void setSimulationDelay( int speed )
{
    simDelay = speed;
}

/*
 * get Data form Lincity NG and Save City
 */
void saveCityNG(const World& world, const std::filesystem::path& filename) {
  std::filesystem::path fullname = filename;

  // Ensure the parent directory exists before trying to save
  // This prevents crashes on Windows when the save directory doesn't exist
  try {
    std::filesystem::path parentDir = fullname.parent_path();
    if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
      std::filesystem::create_directories(parentDir);
    }
  } catch (const std::filesystem::filesystem_error& e) {
    std::cerr << "warning: failed to create save directory: " << e.what() << std::endl;
    // Continue anyway and let world.save() fail with proper error dialog if needed
  }

  try {
    world.save(fullname);
    std::cout << "saved game to '" << fullname << "'" << std::endl;
  } catch(std::runtime_error& err) {
    std::cerr << "error: failed to save game to '" << fullname << "': "
      << err.what() << std::endl;
    // Show error dialog instead of crashing with assert
    // This is much better for Windows users
    DialogBuilder()
      .titleText(_("Error"))
      .messageAddTextBold(_("Error: Failed to save game."))
      .messageAddText(fmt::format(_("Could not save to {}."), fullname))
      .messageAddText(err.what())
      .imageFile("images/gui/dialogs/error.png")
      .buttonSet(DialogBuilder::ButtonSet::OK)
      .build();
  }
}

/*
 * Load City and do setup for Lincity NG.
 */
std::unique_ptr<World> loadCityNG(const std::filesystem::path& filename) {
  std::unique_ptr<World> world;
  try {
    world = World::load(filename);
    std::cout << "loaded game from " << filename << std::endl;
  } catch(std::runtime_error& err) {
    std::cerr << "error: failed to load game from " << filename
      << ": " << err.what() << std::endl;
    DialogBuilder()
      .titleText(_("Error"))
      .messageAddTextBold(_("Error: Failed to load game."))
      .messageAddText(fmt::format(_("Could not load {}."), filename))
      .messageAddText(err.what())
      .imageFile("images/gui/dialogs/error.png")
      .buttonSet(DialogBuilder::ButtonSet::OK)
      .build();
  }

  return world;
}

void initLincity()
{
    /* Set up the paths to certain files and directories */
    // init_path_strings();

    /* Make sure the save directory exists */
    // check_savedir();

    /*initialize Desktop Componenet Factories*/
    initFactories();

    /* Initialize random number generator */
    srand (time (0));

    //mps_init(); //CK no implemented

    // initialize constructions
    initializeModules();

    // animation time
    reset_start_time ();

}


/** @file lincity-ng/MainLincity.cpp */
