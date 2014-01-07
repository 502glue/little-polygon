```
 __         __     ______   ______   __         ______    
/\ \       /\ \   /\__  _\ /\__  _\ /\ \       /\  ___\   
\ \ \____  \ \ \  \/_/\ \/ \/_/\ \/ \ \ \____  \ \  __\   
 \ \_____\  \ \_\    \ \_\    \ \_\  \ \_____\  \ \_____\ 
  \/_____/   \/_/     \/_/     \/_/   \/_____/   \/_____/ 
                                                          
 ______   ______     __         __  __     ______     ______     __   __    
/\  == \ /\  __ \   /\ \       /\ \_\ \   /\  ___\   /\  __ \   /\ "-.\ \   
\ \  _-/ \ \ \/\ \  \ \ \____  \ \____ \  \ \ \__ \  \ \ \/\ \  \ \ \-.  \  
 \ \_\    \ \_____\  \ \_____\  \/\_____\  \ \_____\  \ \_____\  \ \_\\"\_\ 
  \/_/     \/_____/   \/_____/   \/_____/   \/_____/   \/_____/   \/_/ \/_/ 
                                                                            
```

DESCRIPTION
-----------

A collection of useful little scripts for making native mobile and desktop games.  Features
are added as I need them for my personal work, but I'm always game for contributions.

Build Dependencies (python2)
* lxml - For parsing TMX Files
* Pillow - For image processing and compositting sprite atlasses
* pyyaml - For asset-script parsing
* psd_tools - For Photoshop import Support

Runtime Dependencies (native)
* sdl2 - platform abstraction
* sdl2_mixer - music and sound effects
* zlib - asset decompression

Modules
* littlepolygon_assets - a simple system for packaging and loading compressed game assets
* littlepolygon_graphics - fast sprite-batching renderer for pixel games
* littlepolygon_templates - a collection of slim-and-handy C++ collection templates
* littlepolygon_utils - small math and utility helpers

CODING STANDARDS
----------------

Python scripts are organized into "tasks" - scripts that encapsulate a single method which
can be invoked directly (checking for "__main__"), or imported and composited with other
tasks in high-level scripts.

Native code is written in C++ with a "C with Objects" mentality.  Each object is expressed
as a plain-old-data structure with just a few convenience getter methods.  Initialization and
non-idempotent methods are top-level functions which take the structure as a "context" argument.
This is a lot more flexible for controlling memory allocation, initialization order, multithreading, 
binding to scripting environments, keeping private methods out of headers, and allowing the use of
completely opaque pointers that are just forward-declared.

The example game is written with a modern C++11 mentality.  Different tools for different levels
of abstraction ;)

Wishlist
--------

* automatic lua binding (using python scripts, not C++ template madness)
* lightweight entity-component system for in-game "hit F7" level editting
* asynchronous sprite-batch (for multithreading)
* more import asset types (SVG, Flash)
* non-native export targets (e.g. unity, webgl)

LICENSE
-------

Little Polygon SDK
Copyright (C) 2014 Max Kaufmann

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

(commercial license and support available upon request)
