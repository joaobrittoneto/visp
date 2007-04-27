/****************************************************************************
 *
 * $Id: trackDot2WithAutoDetection.cpp,v 1.6 2007-04-27 16:40:14 fspindle Exp $
 *
 * Copyright (C) 1998-2006 Inria. All rights reserved.
 *
 * This software was developed at:
 * IRISA/INRIA Rennes
 * Projet Lagadic
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * http://www.irisa.fr/lagadic
 *
 * This file is part of the ViSP toolkit
 *
 * This file may be distributed under the terms of the Q Public License
 * as defined by Trolltech AS of Norway and appearing in the file
 * LICENSE included in the packaging of this file.
 *
 * Licensees holding valid ViSP Professional Edition licenses may
 * use this file in accordance with the ViSP Commercial License
 * Agreement provided with the Software.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Contact visp@irisa.fr if any conditions of this licensing are
 * not clear to you.
 *
 * Description:
 * Test auto detection of dots.
 *
 * Authors:
 * Fabien Spindler
 *
 *****************************************************************************/
/*!
  \file trackDot2WithAutoDetection.cpp

  \brief Example of auto detection of dots using vpDot2.
*/

/*!
  \example trackDot2WithAutoDetection.cpp

  Example of auto detection of dots using vpDot2.
*/

#include <stdio.h>
#include <sstream>
#include <iomanip>

#include <visp/vpDebug.h>
#include <visp/vpConfig.h>

#if (defined (VISP_HAVE_X11) || defined(VISP_HAVE_GTK) || defined(WIN32))

#include <stdio.h>

#include <visp/vpImage.h>
#include <visp/vpImageIo.h>
#include <visp/vpDisplayX.h>
#include <visp/vpDisplayGTK.h>
#include <visp/vpDisplayGDI.h>
#include <visp/vpList.h>
#include <visp/vpDot2.h>
#include <visp/vpParseArgv.h>
#include <visp/vpIoTools.h>

// List of allowed command line options
#define GETOPTARGS	"cdi:h"

/*!

  Print the program options.

  \param name : Program name.
  \param badparam : Bad parameter name.
  \param ipath: Input image path.

*/
void usage(char *name, char *badparam, std::string ipath)
{
  fprintf(stdout, "\n\
Test auto detection of dots using vpDot2.\n\
\n\
SYNOPSIS\n\
  %s [-i <input image path>] [-c] [-d] [-h]\n", name);

  fprintf(stdout, "\n\
OPTIONS:                                               Default\n\
  -i <input image path>                                %s\n\
     Set image input path.\n\
     From this path read images \n\
     \"ViSP-images/mire-2/image.%%04d.pgm\"\n\
     Setting the VISP_INPUT_IMAGE_PATH environment\n\
     variable produces the same behaviour than using\n\
     this option.\n\
\n\
  -c\n\
     Disable the mouse click. Usefull to automaze the \n\
     execution of this program without humain intervention.\n\
\n\
  -d \n\
     Turn off the display.\n\
\n\
  -h\n\
     Print the help.\n",
	  ipath.c_str());

  if (badparam)
    fprintf(stdout, "\nERROR: Bad parameter [%s]\n", badparam);
}
/*!

  Set the program options.

  \param argc : Command line number of parameters.
  \param argv : Array of command line parameters.
  \param ipath : Input image path.
  \param click_allowed : Mouse click activation.
  \param display : Display activation.

  \return false if the program has to be stopped, true otherwise.

*/
bool getOptions(int argc, char **argv, std::string &ipath,
		bool &click_allowed, bool &display)
{
  char *optarg;
  int	c;
  while ((c = vpParseArgv::parse(argc, argv, GETOPTARGS, &optarg)) > 1) {

    switch (c) {
    case 'c': click_allowed = false; break;
    case 'd': display = false; break;
    case 'i': ipath = optarg; break;
    case 'h': usage(argv[0], NULL, ipath); return false; break;

    default:
      usage(argv[0], optarg, ipath);
      return false; break;
    }
  }

  if ((c == 1) || (c == -1)) {
    // standalone param or error
    usage(argv[0], NULL, ipath);
    std::cerr << "ERROR: " << std::endl;
    std::cerr << "  Bad argument " << optarg << std::endl << std::endl;
    return false;
  }

  return true;
}


int
main(int argc, char ** argv)
{
  std::string env_ipath;
  std::string opt_ipath;
  std::string ipath;
  std::string dirname;
  std::string filename;
  bool opt_click_allowed = true;
  bool opt_display = true;

  // Get the VISP_IMAGE_PATH environment variable value
  char *ptenv = getenv("VISP_INPUT_IMAGE_PATH");
  if (ptenv != NULL)
    env_ipath = ptenv;

  // Set the default input path
  if (! env_ipath.empty())
    ipath = env_ipath;


  // Read the command line options
  if (getOptions(argc, argv, opt_ipath, opt_click_allowed,
		 opt_display) == false) {
    exit (-1);
  }

  // Get the option values
  if (!opt_ipath.empty())
    ipath = opt_ipath;

  // Compare ipath and env_ipath. If they differ, we take into account
  // the input path comming from the command line option
  if (opt_ipath.empty()) {
    if (ipath != env_ipath) {
      std::cout << std::endl
	   << "WARNING: " << std::endl;
      std::cout << "  Since -i <visp image path=" << ipath << "> "
	   << "  is different from VISP_IMAGE_PATH=" << env_ipath << std::endl
	   << "  we skip the environment variable." << std::endl;
    }
  }

  // Test if an input path is set
  if (opt_ipath.empty() && env_ipath.empty()){
    usage(argv[0], NULL, ipath);
    std::cerr << std::endl
	 << "ERROR:" << std::endl;
    std::cerr << "  Use -i <visp image path> option or set VISP_INPUT_IMAGE_PATH "
	 << std::endl
	 << "  environment variable to specify the location of the " << std::endl
	 << "  image path where test images are located." << std::endl << std::endl;
    exit(-1);
  }


  // Declare an image, this is a gray level image (unsigned char)
  // it size is not defined yet, it will be defined when the image will
  // read on the disk
  vpImage<unsigned char> I ;

  // Set the path location of the image sequence
  dirname = ipath + vpIoTools::path("/ViSP-images/mire-2/");

  // Build the name of the image file
  unsigned iter = 1; // Image number
  std::ostringstream s;
  s.setf(std::ios::right, std::ios::adjustfield);
  s << "image." << std::setw(4) << std::setfill('0') << iter << ".pgm";
  filename = dirname + s.str();

  // Read the PGM image named "filename" on the disk, and put the
  // bitmap into the image structure I.  I is initialized to the
  // correct size
  //
  // exception readPGM may throw various exception if, for example,
  // the file does not exist, or if the memory cannot be allocated
  try{
    vpCTRACE << "Load: " << filename << std::endl;

    vpImageIo::readPGM(I, filename) ;
  }
  catch(...)
  {
    // an exception is throwned if an exception from readPGM has been catched
    // here this will result in the end of the program
    // Note that another error message has been printed from readPGM
    // to give more information about the error
    std::cerr << std::endl
	 << "ERROR:" << std::endl;
    std::cerr << "  Cannot read " << filename << std::endl;
    std::cerr << "  Check your -i " << ipath << " option " << std::endl
	 << "  or VISP_INPUT_IMAGE_PATH environment variable."
	 << std::endl;
    exit(-1);
  }


  // We open a window using either X11, GTK or GDI.
#if defined VISP_HAVE_X11
  vpDisplayX display;
#elif defined VISP_HAVE_GTK
  vpDisplayGTK display;
#elif defined WIN32
  vpDisplayGDI display;
#endif

  if (opt_display) {
    try{
      // Display size is automatically defined by the image (I) size
      display.init(I, 100, 100,"Display...") ;
      // Display the image
      // The image class has a member that specify a pointer toward
      // the display that has been initialized in the display declaration
      // therefore is is no longuer necessary to make a reference to the
      // display variable.
      vpDisplay::display(I) ;
    }
    catch(...)
    {
      vpERROR_TRACE("Error while displaying the image") ;
      exit(-1);
    }
  }

  // Dot declaration
  vpDot2 d ;
  if (0) {
    try{
      d.initTracking(I) ;
      printf("Dot characteristics: \n");
      printf("  width : %lf\n", d.getWidth());
      printf("  height: %lf\n", d.getHeight());
      printf("  surface: %lf\n", d.getSurface());
      printf("  gray level min: %d\n", d.getGrayLevelMin());
      printf("  gray level max: %d\n", d.getGrayLevelMax());
      printf("  accuracy: %lf\n", d.getAccuracy());
    }
    catch(...)
      {
	std::cerr << "Cannot initialise the tracking and get default dot features..." << std::endl;
	exit(-1);
      }
  }

  // Set dot characteristics for the auto detection
  d.setGraphics(true);
  d.setWidth(15.0);
  d.setHeight(12.0);
  d.setSurface(124);
  d.setGrayLevelMin(164);
  d.setGrayLevelMax(255);
  d.setAccuracy(0.65);

  while (iter < 10)
  {

    // set the new image name
    s.str("");
    s << "image." << std::setw(4) << std::setfill('0') << iter << ".pgm";
    filename = dirname + s.str();
    // read the image
    vpImageIo::readPGM(I, filename);

    if (opt_display) {
      // Display the image
      vpDisplay::display(I) ;
    }

    std::cout << "Search dots in image" << filename << std::endl;
    vpList<vpDot2> * list_d;
    list_d = d.searchDotsInArea(I, 0, 0, I.getWidth(), I.getHeight()) ;

    if( list_d->nbElement() == 0 ) {
      std::cout << "Dot auto detection did not work, "
	   << "Please click on a dot to perform a manual detection"
	   << std::endl;

      d.initTracking( I );
      if (opt_display) {
	vpDisplay::displayCross_uv(I,(int)d.get_u(), (int)d.get_v(),
				   10,vpColor::green) ;
	vpDisplay::flush(I) ;
      }
    }
    else {
      std::cout << std::endl << list_d->nbElement() << " dots are detected" << std::endl;

      if (opt_display) {
	// Parse all founded dots for display
	list_d->front();
	while (!list_d->outside()) {
	  vpDot2 tmp_d;
	  tmp_d = list_d->value() ;
	  list_d->next() ;
	  vpDisplay::displayCross_uv(I,(int)tmp_d.get_u(), (int)tmp_d.get_v(),
				     10, vpColor::red) ;
	}
	vpDisplay::flush(I) ;
      }
    }

    // free memory allocated for the list of dots found in d.searchDotsInArea()
    list_d->kill();
    delete list_d;

    // If click is allowed, wait for a mouse click to launch the next iteration
    if (opt_display && opt_click_allowed) {
      std::cout << "\nA click to continue..." << std::endl;
      // Wait for a blocking mouse click
      vpDisplay::getClick(I) ;
    }

    iter ++;
  }
  if (opt_display && opt_click_allowed) {
    std::cout << "\nA click to exit..." << std::endl;
    // Wait for a blocking mouse click
    vpDisplay::getClick(I) ;
  }
  vpTRACE("End...");
}
#else
int
main()
{
  vpERROR_TRACE("You do not have X11, GTK or GDI display functionalities...");
}

#endif


/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
