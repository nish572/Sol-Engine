#include "Core.h"

int main(int argc, char* argv[]) {
	if (Core::initCore()) {
		return 1;
	}
	return true;
}