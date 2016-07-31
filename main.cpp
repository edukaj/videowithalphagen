#include "ProgramOptions.h"
#include "VideoConverter.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
	try
	{
		ProgramOptions po{argc, argv};

		// print info on program options
		cout << po << endl;

		if( po.shouldDisplayOnlyHelp())
			return EXIT_SUCCESS;

		VideoConverter vc{po};

		const auto& listFrames = vc.frames();

		if (listFrames.empty())
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
