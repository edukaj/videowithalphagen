#if 1
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

		for( const auto& f : listFrames )
			cout << f << endl;

		vc.generateVideo();
	}
	catch( const exception& exc )
	{
		cerr << exc.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
#else

#include <opencv2/highgui.hpp>
#include <boost/filesystem.hpp>
#include <iostream>

using namespace std;
using namespace boost;
using namespace boost::filesystem;


int main(int argc, char* argv[])
{
  path p (".");   // p reads clearer than argv[1] in the following code

  auto bmp = cv::imread("./Poker_winning_hand_001.bmp");
  auto bmpSize = bmp.size();
  cout << "opened ./Poker_winning_hand_001.bmp " << bmpSize.width << "x" << bmpSize.height << endl;

  auto tga = cv::imread("./Poker_winning_hand_001.tga");
  auto tgaSize = tga.size();
  cout << "opened ./Poker_winning_hand_001.tga " << tgaSize.width << "x" << tgaSize.height << endl;

  auto png = cv::imread("./Poker_winning_hand_001.png");
  auto pngSize = png.size();
  cout << "opened ./Poker_winning_hand_001.png " << pngSize.width << "x" << pngSize.height << endl;


  try
  {
	if (exists(p))    // does p actually exist?
	{
	  if (is_regular_file(p))        // is p a regular file?
		cout << p << " size is " << file_size(p) << '\n';

	  else if (is_directory(p))      // is p a directory?
	  {
		cout << p << " is a directory containing:\n";

		copy(directory_iterator(p), directory_iterator(), // directory_iterator::value_type
		  ostream_iterator<directory_entry>(cout, "\n")); // is directory_entry, which is
														  // converted to a path by the
														  // path stream inserter
	  }

	  else
		cout << p << " exists, but is neither a regular file nor a directory\n";
	}
	else
	  cout << p << " does not exist\n";
  }
  catch (const filesystem_error& ex)
  {
	cout << ex.what() << '\n';
  }

  return 0;
}
#endif
