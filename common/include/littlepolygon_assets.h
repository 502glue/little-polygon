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
#include "littlepolygon_base.h"

//------------------------------------------------------------------------------
// CONSTANTS
//------------------------------------------------------------------------------

#define ASCII_BEGIN          32
#define ASCII_END            127

#define TEXTURE_FLAG_FILTER  0x1
#define TEXTURE_FLAG_REPEAT  0x2
#define TEXTURE_FLAG_LUM     0x4
#define TEXTURE_FLAG_RGB     0x8

#define ASSET_TYPE_UNDEFINED 0
#define ASSET_TYPE_TEXTURE   1
#define ASSET_TYPE_IMAGE     2
#define ASSET_TYPE_FONT      3
#define ASSET_TYPE_SAMPLE    4
#define ASSET_TYPE_TILEMAP   5
#define ASSET_TYPE_USERDATA  6

//------------------------------------------------------------------------------
// ASSET RECORDS
// These records are directly mapped into memory from the asset binary.
//------------------------------------------------------------------------------

struct TextureAsset {
	void *compressedData; 	// zlib compressed texture data
	int32_t w, h; 	        // size of the texture (guarenteed to be POT)
	uint32_t 
		compressedSize,     // size of the compressed buffer, in bytes
		textureHandle,      // handle to the initialized texture resource
		flags;              // extra information (wrapping, format, etc)
	
	inline bool initialized() const { return textureHandle != 0; }
	inline int format() const { return GL_RGBA; }
};

struct FrameAsset {
	float u0, v0,  // UV coordinates of the corners (necessary since the image
		u1, v1,    // might have been transposed during packing)
		u2, v2, 
		u3, v3;

	int32_t px, py, // post-trim pivot of the frame
		w, h;	    // post-trim size of the frame
};

struct ImageAsset {
	TextureAsset *texture; // the texture onto which this image is packed
	int32_t w, h,          // the logical size of the image
		px, py,            // the logical pivot of the image
		nframes;           // number of rendered frames

	inline FrameAsset* frame(int i) {
		// frames are stored immediately after the image (correctly aligned)
		ASSERT(i >= 0 && i < nframes);
		return reinterpret_cast<FrameAsset*>(this+1) + i;
	}	
};

struct uint8_pair_t {
	uint8_t x;
	uint8_t y;
};

struct TilemapAsset {
	uint8_pair_t *data;
	void *compressedData;   // zlib compressed tilemap buffer
	uint32_t tw, th,        // the size of the individual tiles
		mw, mh,             // the size of the tilemap
		compressedSize;     // the byte-length of the compressed buffer
	TextureAsset tileAtlas; // a texture-atlas of all the tiles

	inline bool intialized() const { return data != 0; }

	inline uint8_pair_t tileAt(int x, int y) const {
		ASSERT(intialized());
		ASSERT(x >= 0 && x < mw);
		ASSERT(y >= 0 && y < mh);
		return data[y * mw + x];
	}
};

struct GlyphAsset {
	int32_t x, y, // texel position of this glyph
		advance;  // pixel offset to the next character
};

struct FontAsset {
	int32_t height;                           // line-height of the font
	GlyphAsset glyphs[ASCII_END-ASCII_BEGIN]; // individual glyph metrics
	TextureAsset texture;                     // character texture-atlas
	
	GlyphAsset getGlyph(const char c) {
		// glyphs are stored in ASCII-order
		ASSERT(c >= ASCII_BEGIN && c < ASCII_END);
		return glyphs[c-ASCII_BEGIN];
	}

	const char* measureLine(const char *msg, int *outLength) {
		// measure the given line's total advance without rendering
		*outLength = 0;
		while(*msg && *msg != '\n') {
			*outLength += getGlyph(*msg).advance;
			++msg;
		}
		return msg;
	}	
};

struct SampleAsset {
	Mix_Chunk *chunk;              // initialized as a sdl mixer "chunk"
	void *compressedData;          // compressed PCM buffer
	int32_t channelCount,          // PCM stereo or mono?
		sampleWidth,               // PCM bits per sample
		frequency;                 // PCM samples per second
	uint32_t size, compressedSize; // byte-length of the compressed data

	inline bool initialized() const { return chunk != 0; }
};

struct UserdataAsset {
	// A slice of plain old data that can represent anything - structured game assets,
	// UTF8 strings, etc.  Data immediately follows the initial size term.

	// TODO: Add support for optionally-compressed userdata ("always add value"!).

	size_t size; // length of the data, in bytes

	inline void *data() const { return (void*)(this+1); }

	template<typename T> 
	T& as() { ASSERT(size == sizeof(T)); return *((T*)(this+1)); }
};

//------------------------------------------------------------------------------
// MAIN INTERFACE
//------------------------------------------------------------------------------

struct AssetBundle {
	int assetCount;

	// Assets are keyed by name-hashes (fnv-1a)
	// inlined so that the compiler can constant-fold over string literals :)
	inline static uint32_t hash(const char* name) {
	    uint32_t hval = 0x811c9dc5;
	    while(*name) {
	        hval ^= (*name);
	        hval *= 0x01000193;
	        ++name;
	    }
	    return hval;
	}

	struct Header {
		uint32_t hash, type;
		void* data;
	};

	Header *headers;

	// headers are sorted by hash, so lookup is LOG(N)
	void* findHeader(uint32_t hash, uint32_t assetType);

	// lookup assets by name
	inline TextureAsset *texture(const char * name) { return texture(hash(name)); }
	inline ImageAsset *image(const char * name) { return image(hash(name)); }
	inline TilemapAsset *tilemap(const char * name) { return tilemap(hash(name)); }
	inline FontAsset *font(const char * name) { return font(hash(name)); }
	inline SampleAsset *sample(const char * name) { return sample(hash(name)); }
	inline UserdataAsset *userdata(const char *name) { return userdata(hash(name)); }

	// lookup assets by hash
	inline TextureAsset *texture(uint32_t hash) { return (TextureAsset*) findHeader(hash, ASSET_TYPE_TEXTURE); }
	inline ImageAsset *image(uint32_t hash) { return (ImageAsset*) findHeader(hash, ASSET_TYPE_IMAGE); }
	inline TilemapAsset *tilemap(uint32_t hash) { return (TilemapAsset*) findHeader(hash, ASSET_TYPE_TILEMAP); }
	inline FontAsset *font(uint32_t hash) { return (FontAsset*) findHeader(hash, ASSET_TYPE_FONT); }
	inline SampleAsset *sample(uint32_t hash) { return (SampleAsset*) findHeader(hash, ASSET_TYPE_SAMPLE); }
	inline UserdataAsset *userdata(uint32_t hash) { return (UserdataAsset*) findHeader(hash, ASSET_TYPE_USERDATA); }

};

// Initialize an asset bundle by memory-mapping the binary at the given
// SDL path and then performing pointer-fixup.
void initialize(AssetBundle *bundle, const char* path, uint32_t crc=0);
void release(AssetBundle *bundle);

// By default, resource handles for assets are initializes lazily, however
// you can use these functions to initialize them eagerly.
void intializeContents(AssetBundle *bundle);
void releaseContents(AssetBundle *bundle);

// Methods for decompressing textures and binding to opengl
void initialize(TextureAsset *asset);
void bind(TextureAsset *asset);
void release(TextureAsset *asset);

// Methods for decompressing tilemaps and initializing its
// texture-atlas
void initialize(TilemapAsset *asset);
void release(TilemapAsset *asset);

// Methods for decompressing audio samples bind to mixer
void initialize(SampleAsset *asset);
void play(SampleAsset *asset);
void release(SampleAsset *asset);

