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
#include "sprites.h"
#include "collections.h"
#include "utils.h"

#define kTimelineTranslation   1
#define kTimelineRotation      2
#define kTimelineScale         3

//------------------------------------------------------------------------------
// ASSETS

struct RigBoneAsset
{
	uint32_t parentIndex;
	uint32_t hash;
	
	Vec2 translation;
	Vec2 scale;
	lpFloat radians;

	AffineMatrix concatenatedMatrix() const {
		auto uv = unitVector(radians);
		return AffineMatrix(scale.x * uv, scale.y * uv.anticlockwise(), translation);
	}
};

struct RigSlotAsset
{
	uint32_t boneIndex;
	uint32_t defaultAttachment;
	Color defaultColor;
};

struct RigAttachmentAsset
{
	RigSlotAsset *slot;
	ImageAsset *image;
	uint32_t hash;
	uint32_t layerHash;
	AffineMatrix xform;
};

struct RigAnimationAsset
{
	uint32_t hash;
	lpFloat duration;
};

struct RigTimelineAsset
{
	lpFloat *times;
	union {
		lpFloat *rotationValues;
		Vec2 *translationValues;
		Vec2 *scaleValues;
		int *attachmentValues;
	};
	uint32_t nkeyframes;
	uint32_t animHash;
	union {
		uint32_t boneIndex;
		uint32_t slotIndex;
	};
	uint32_t kind;
};

struct RigAsset
{
	uint32_t defaultLayer;
	uint32_t nbones;
	uint32_t nslots;
	uint32_t nattachments;
	uint32_t nanims;
	uint32_t ntimeslines;
	RigBoneAsset *bones;
	RigSlotAsset *slots;
	RigAttachmentAsset *attachments;
	RigAnimationAsset *anims;
	RigTimelineAsset *timelines;
};

//------------------------------------------------------------------------------
// RUNTIME CONTROLLER

class Rig {
private:
	const RigAsset* data;

	struct Attitude {
		lpFloat radians;
		Vec2 scale;
		
		void applyTo(AffineMatrix& mat) {
			auto u = unitVector(radians);
			mat.u = scale.x * u;
			mat.v = scale.y * u.anticlockwise();
		}
	};
	
	// (indexed by bone)
	Array<Attitude> localAttitudes;
	Array<AffineMatrix> localTransforms;
	Array<AffineMatrix> worldTransforms;
	
	// (indexed by timeline)
	BitArray timelineMask;
	Array<unsigned> currentKeyframes;
	
	RigAnimationAsset* currentAnimation;
	uint32_t currentLayer;
	lpFloat currentTime;
	
	bool xformDirty;
	
public:
	Rig(const RigAsset* asset);
	
	// GETTERS
	
	bool playing() const { return currentAnimation != 0; }
	lpFloat time() const { return currentTime; }
	bool showingLayer(const char* name) const { return currentLayer == fnv1a(name); }
	bool showingAnimation(const char* name) const { return currentAnimation->hash == fnv1a(name); }
	const AffineMatrix& rootTransform() const { return worldTransforms[0]; }
	const AffineMatrix* findTransform(const char* boneName) const;
	
	// SETTERS
	
	void setRootTransform(const AffineMatrix& mat);
	void setLayer(const char *layerName);
	void setAnimation(const char *animName);
	
	// METHODS
	
	void resetPose();
	void resetTime();
	void refreshTransforms();
	void tick(lpFloat dt);
	void draw(SpritePlotter* plotter, Color c=rgba(0));
	
private:
	
	void setDefaultPose();
	void computeWorldTransforms();
	void updateTimeline(int i);
	void applyTimeline(int i);
	
};

