// -*- C++ -*-
//  $Id: npr_vertex_pdf.h,v 1.0 2017-8-14 R. Zhang & Y. Yang, Exp $
/*! \file
 *  \brief NPR vertex calculations
 */

#ifndef __npr_vertex_pdf_h__
#define __npr_vertex_pdf_h__

#include "chromabase.h"
#include "io_general_class.h"
#include "npr_vertex_pdf.h"

namespace Chroma 
{

  //! NPR vertices
  /*! \ingroup hadron */
  void NprVertexPDFRISMOM(const LatticePropagator &             F,
					const LatticePropagator &             B,
		 const multi1d< LatticeColorMatrix > & U,
		 const unsigned short int              MaxNLinks,
		 const BBLinkPattern                   LinkPattern,
		 const char* name,
		 const int   cfg_serial);

}  // end namespace Chroma

#endif

//###################################################################################//
//###################################################################################//
