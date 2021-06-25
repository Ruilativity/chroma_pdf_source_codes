//  $Id: npr_vertex_pdf.cc,v 1.0 2017-8-14 R. Zhang & Y. Yang, Exp $
/*! \file
 *  \brief NPR vertex calculations
 */

#include "util/ft/sftmom.h"
#include "npr_vertex_pdf_rismom.h"
#include <string>
using std::string;

namespace Chroma 
{

  QDP::StandardOutputStream& operator<<(QDP::StandardOutputStream& s, const multi1d<int>& d);
/*  {
    if (d.size() > 0)
    {
      s << d[0];
      for(int i=1; i < d.size(); ++i)
	s << " " << d[i];
    }

    return s;
  }*/

  //! NPR vertices in RI/SMOM scheme
  void NprVertexPDFRISMOM(const LatticePropagator &             F,
						  const LatticePropagator &             B,
		 const multi1d< LatticeColorMatrix > & U,
		 const unsigned short int              MaxNLinks,
		 const BBLinkPattern                   LinkPattern,
		 const char* name,
		 int  cfg_serial)
  {
    StopWatch TotalTime;
    TotalTime.reset();
    TotalTime.start();
	
    StopWatch Timer;



    general_data_base output(name);

	output.add_dimension(dim_conf, 1, &cfg_serial);
	output.add_dimension(dim_direction, 4);
	output.add_dimension(dim_displacement, 1+MaxNLinks);
	output.add_dimension(dim_operator, 16);
	output.add_dimension(dim_temporary, 144);
	output.add_dimension(dim_complex, 2);
	if(Layout::primaryNode()) output.initialize();

    int GBB_NLinkPatterns=0;

    //#################################################################################//
    // open building blocks data files                                                 //
    //#################################################################################//

    Timer.reset();
    Timer.start();

    //#################################################################################//
    // calculate building blocks                                                       //
    //#################################################################################//

  for(int idir=0;idir<4;idir++)
  {
    QDPIO::cout << __func__ << ": start BkwdFrwd" << std::endl;

    const int NLinks = 0;
    multi1d< int > LinkDirs( 0 );

    LatticePropagator B_reverse = Gamma(15)*adj(B)*Gamma(15);
    BkwdFrwd(B_reverse, F, output, 0, GBB_NLinkPatterns, LinkDirs);
    

    Timer.stop();
    QDPIO::cout << __func__ << ": total time for 0 links (single BkwdFrwdTr call) = "
		<< Timer.getTimeInSeconds() 
		<< " seconds" << std::endl;

    Timer.reset();
    Timer.start();

    QDPIO::cout << __func__ << ": start AddLinks" << std::endl;

    AddLinks(B_reverse, F, U,
	     LinkDirs, MaxNLinks, idir, LinkPattern, 
	      output, GBB_NLinkPatterns);

    Timer.stop();
    QDPIO::cout << __func__ << ": total time for remaining links (outermost AddLinks call) = "
		<< Timer.getTimeInSeconds() 
		<< " seconds" << std::endl;

    QDPIO::cout << __func__ << "saving io_general output" << std::endl;
  }
	if(output.data!=NULL) if(Layout::primaryNode()) output.save();

    TotalTime.stop();
    QDPIO::cout << __func__ << ": total time = "
		<< TotalTime.getTimeInSeconds() 
		<< " seconds" << std::endl;


    return;
  }
}  // end namespace Chroma
