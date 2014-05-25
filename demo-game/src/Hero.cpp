#include "game.h"



Hero::Hero() :
Entity(
	gAssets.userdata("hero.position")->get<Vec2>() - vec(0, kSlop),
	vec(kHeroWidth, kHeroHeight)
),
img(gAssets.image("hero")),
dir(1),
animTime(0.0f), yScale(1.0f),
grounded(true), holdingKitten(false)
{
}

void Hero::tick() {
	float dt = gTimer.deltaSeconds;
	
	// MOVEMENT
	auto speedTarget = kHeroMoveSpeed * gWorld.input.dirX();
	auto easing = gWorld.input.dirX() == 0 ? 0.2f : 0.333f;
	speed.x = easeTowards(speed.x, speedTarget, easing, dt);
	
	if (gWorld.input.dirX()) { dir = gWorld.input.dirX(); }
	
	// GRAVITY
	speed.y += kGravity * dt;
	
	// JUMPING
	if (grounded && gWorld.input.pressedJump()) {
		gAssets.sample("jump")->play();
		speed.y = jumpImpulse(kHeroJumpHeight);
	}
	
	// PICKUP KITTEN?
	if (gWorld.kitten.canPickUp() && overlaps(&gWorld.kitten)) {
		// PICKUP
		gAssets.sample("pickup")->play();
		gWorld.kitten.pickup();
	}
	
	// ACTION?
	if (gWorld.input.pressedAction()) {
		performAction();
	}
	
	// KINEMATICS
	auto wasGrounded = grounded;
	int hitX, hitY; move(&hitX, &hitY);
	grounded = hitY > 0;
	if (grounded) {
		if (!wasGrounded) {
		
			// LANDING FX
			animTime = 0.0f;
			yScale = 0.8f;
			gAssets.sample("land")->play();

		} else {
			
			// RUNNING FX
			int prevFrame = getFrame();
			animTime += kHeroStepsPerMeter * dt * abs(speed.x);
			int nextFrame = getFrame();
			if (prevFrame == 1 && nextFrame == 0) {
				gAssets.sample("footfall")->play();
			}

			// EASE TO NORMAL SCALE
			yScale = easeTowards(yScale, 1.0f, 0.2f, dt);
			
		}
	} else {
		
		// EASE TO FREEFALL SCALE
		yScale = easeTowards(yScale, 1.025f, 0.2f, dt);
		
	}

}

void Hero::performAction() {
	if (gWorld.kitten.isCarried()) {
		// SHOOT
		gAssets.sample("shoot")->play();
		gWorld.kitten.shoot();
		speed.x -= dir * kHeroShootKickback;
	}
	
	
}

void Hero::draw() {
	gSprites.drawImage(
		img,
		AffineMatrix(vec((2.0f-yScale) * dir,0), vec(0,yScale), pixelPosition()),
		getFrame()
	);
}

bool Hero::isStandingStill() const {
	return speed.x > -0.133f && speed.x < 0.133f;
}

Vec2 Hero::carryAnchor() const {
	return position + vec(dir * halfSize.x, -2.1f * yScale * halfSize.y) - vec(0, kMetersPerPixel * getFrame());
}

int Hero::getFrame() const {
	if (!grounded) {
		return 1;
	} else if (!isStandingStill()) {
		return int(animTime) % 2;
	} else {
		return 0;
	}
}
