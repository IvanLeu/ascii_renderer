#include "Application.h"
#include <iostream>

int main() {

	try {
		Application app;
		return app.Run();
	}
	catch (std::exception& e) {
		std::cerr << "Exception occured: " << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Unknown exception" << std::endl;
	}

	return 0;
}