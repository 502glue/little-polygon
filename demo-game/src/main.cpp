#include "game.h"

int main(int argc, char *argv[]) {
	lpInitialize("Hello, World", 20 * 16 * 4, 8 * 16 * 4, "assets.bin");
	static World world;
	world.run();
	lpFinalize();
}