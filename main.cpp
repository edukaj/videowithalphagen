#include "ProgramOptions.h"
#include "VideoConverter.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	try
	{
		ProgramOptions po{argc, argv};

		if( po.shouldDisplayOnlyHelp())
			return EXIT_SUCCESS;

		VideoConverter vc{po};

		if (vc.frames().empty())
			cout << "no files filtered" << endl;

		vc.generateVideo();
	}
	catch( const exception& exc )
	{
		cerr << exc.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
