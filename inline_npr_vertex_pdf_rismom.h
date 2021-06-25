// -*- C++ -*-
/*! \file
 * \brief Inline construction of NPR vertices for quasi PDF in RI/SMOM scheme
 *
 * NPR vertices on  props
 */

#ifndef __inline_npr_vertex_pdf_h__
#define __inline_npr_vertex_pdf_h__

#include "chromabase.h"
#include "meas/inline/abs_inline_measurement.h"
#include "io/qprop_io.h"

namespace Chroma 
{ 
  /*! \ingroup inlinehadron */
  namespace InlineNprVertexPDFRISMOMEnv
  {
    extern const std::string name;
    bool registerAll();
  }

  //! Parameter structure
  /*! \ingroup inlinehadron */
  struct InlineNprVertexPDFRISMOMParams
  {
    InlineNprVertexPDFRISMOMParams();
    InlineNprVertexPDFRISMOMParams(XMLReader& xml_in, const std::string& path);
    void write(XMLWriter& xml_out, const std::string& path);

    unsigned long frequency;

    //! Parameters
    struct Param_t
    {
      int          links_max;	/*!< maximum number of links */
      bool	   save_prop;
      std::string  file_name;          /*!< bb output file name pattern */
      int          cfg_serial;
      GroupXML_t   cfs;                /*!< Fermion state */
    } param;

    //! Propagators
    struct NamedObject_t
    {
      std::string       gauge_id;        /*!< Input Gauge id */
      std::string       prop_id_fwd;         /*!< Input forward prop */
		std::string       prop_id_bwd;         /*!< Input backward prop */
    } named_obj;

    std::string xml_file;  // Alternate XML file pattern
  };
void write_FT_prop(const LatticePropagator& prop,
//                     const multi1d<int> mom,
				   std::string name,int serial);
DPropagator FTpropagator2(const LatticePropagator& prop,
			 const multi1d<int> mom,
						  const multi1d<int> t_src);
void read(XMLReader& xml, const std::string& path, InlineNprVertexPDFRISMOMParams::Param_t& input);
void write(XMLWriter& xml, const std::string& path, const InlineNprVertexPDFRISMOMParams::Param_t& input);
void read(XMLReader& xml, const std::string& path, InlineNprVertexPDFRISMOMParams::NamedObject_t& input);
void write(XMLWriter& xml, const std::string& path, const InlineNprVertexPDFRISMOMParams::NamedObject_t& input);
  //! Inline measurement of NPR vertices
  /*! \ingroup inlinehadron */
  class InlineNprVertexPDFRISMOM : public AbsInlineMeasurement
  {
  public:
    ~InlineNprVertexPDFRISMOM() {}
    InlineNprVertexPDFRISMOM(const InlineNprVertexPDFRISMOMParams& p) : params(p) {}
    InlineNprVertexPDFRISMOM(const InlineNprVertexPDFRISMOM& p) : params(p.params) {}

    unsigned long getFrequency(void) const {return params.frequency;}

    //! Do the measurement
    void operator()(const unsigned long update_no,
		    XMLWriter& xml_out); 

  protected:
    //! Do the measurement
    void func(const unsigned long update_no,
	      XMLWriter& xml_out); 

  private:
    InlineNprVertexPDFRISMOMParams params;
  };

};

#endif
