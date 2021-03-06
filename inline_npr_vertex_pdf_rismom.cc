/*! \file
 * \brief Inline construction of NPR vertices
 *
 * NPR vertices on  props
 */

#include "inline_npr_vertex_pdf_rismom.h"
#include "meas/inline/abs_inline_measurement_factory.h"
#include "meas/glue/mesplq.h"
#include "util/ft/sftmom.h"
// #include "meas/hadron/BuildingBlocks_w.h"
#include "util/info/proginfo.h"
#include "meas/inline/make_xml_file.h"

#include "npr_vertex_pdf_rismom.h"

#include "meas/inline/io/named_objmap.h"

#include "actions/ferm/fermstates/ferm_createstate_factory_w.h"
#include "actions/ferm/fermstates/ferm_createstate_aggregate_w.h"
#include <string>


namespace Chroma 
{ 
  namespace InlineNprVertexPDFRISMOMEnv
  { 
    namespace
    {
      AbsInlineMeasurement* createMeasurement(XMLReader& xml_in, 
					      const std::string& path) 
      {
	return new InlineNprVertexPDFRISMOM(InlineNprVertexPDFRISMOMParams(xml_in, path));
      }

      //! Local registration flag
      bool registered = false;
    }

    const std::string name = "NPR_VERTEX_PDF_RISMOM";

    //! Register all the factories
    bool registerAll() 
    {
      bool success = true; 
      if (! registered)
      {
	success &= CreateFermStateEnv::registerAll();
	success &= TheInlineMeasurementFactory::Instance().registerObject(name, createMeasurement);
	registered = true;
      }
      return success;
    }
  }



  //! Param input
  void read(XMLReader& xml, const std::string& path, InlineNprVertexPDFRISMOMParams::Param_t& input)
  {
    XMLReader paramtop(xml, path);

    int version;
    read(paramtop, "version", version);

    if (paramtop.count("FermState") != 0)
      input.cfs = readXMLGroup(paramtop, "FermState", "Name");
    else
      input.cfs = CreateFermStateEnv::nullXMLGroup();

    switch (version) 
    {
    case 1:
      break;

    default :
      QDPIO::cerr << InlineNprVertexPDFRISMOMEnv::name << ": input parameter version "
		  << version << " unsupported." << std::endl;
      QDP_abort(1);
    }
    
    read(paramtop, "links_max", input.links_max);
    read(paramtop, "file_name", input.file_name);
    read(paramtop, "save_prop", input.save_prop);
    read(paramtop, "cfg_serial", input.cfg_serial);
  }


  //! Param write
  void write(XMLWriter& xml, const std::string& path, const InlineNprVertexPDFRISMOMParams::Param_t& input)
  {
    push(xml, path);

    int version = 1;
    write(xml, "version", version);
    write(xml, "links_max", input.links_max);
    write(xml, "file_name", input.file_name);    
    write(xml, "save_prop", input.save_prop);
    write(xml, "cfg_serial", input.cfg_serial);  
    xml << input.cfs.xml;

    pop(xml);
  }

  //! Propagator input
  void read(XMLReader& xml, const std::string& path, InlineNprVertexPDFRISMOMParams::NamedObject_t& input)
  {
    XMLReader inputtop(xml, path);

    read(inputtop, "gauge_id", input.gauge_id);
    read(inputtop, "prop_id_fwd", input.prop_id_fwd);
	  read(inputtop, "prop_id_bwd", input.prop_id_bwd);
  }

  //! Propagator output
  void write(XMLWriter& xml, const std::string& path, const InlineNprVertexPDFRISMOMParams::NamedObject_t& input)
  {
    push(xml, path);

    write(xml, "gauge_id", input.gauge_id);
    write(xml, "prop_id_fwd", input.prop_id_fwd);
	  write(xml, "prop_id_bwd", input.prop_id_bwd);

    pop(xml);
  }


  // Param stuff
  InlineNprVertexPDFRISMOMParams::InlineNprVertexPDFRISMOMParams() {frequency = 0;}

  InlineNprVertexPDFRISMOMParams::InlineNprVertexPDFRISMOMParams(XMLReader& xml_in, const std::string& path)
  {
    try 
    {
      XMLReader paramtop(xml_in, path);

      if (paramtop.count("Frequency") == 1)
	read(paramtop, "Frequency", frequency);
      else
	frequency = 1;

      // Read program parameters
      read(paramtop, "Param", param);

      // Read in the output propagator/source configuration info
      read(paramtop, "NamedObject", named_obj);

      // Possible alternate XML file pattern
      if (paramtop.count("xml_file") != 0) 
      {
	read(paramtop, "xml_file", xml_file);
      }
    }
    catch(const std::string& e) 
    {
      QDPIO::cerr << __func__ << ": Caught Exception reading XML: " << e << std::endl;
      QDP_abort(1);
    }
  }


  void
  InlineNprVertexPDFRISMOMParams::write(XMLWriter& xml_out, const std::string& path)
  {
    push(xml_out, path);
    
    Chroma::write(xml_out, "Param", param); 
    Chroma::write(xml_out, "NamedObject", named_obj);
    QDP::write(xml_out, "xml_file", xml_file);

    pop(xml_out);
  }


  //###################################################################################//
  // Accept All Link Patterns                                                          //
  //###################################################################################//

  void AllLinkPatterns( bool &                          DoThisPattern,
			bool &                          DoFurtherPatterns,
			multi1d< int > & LinkPattern );
/*  {
    DoThisPattern     = true;
    DoFurtherPatterns = true;
    
    return;
  }*/


  // Function call
  void 
  InlineNprVertexPDFRISMOM::operator()(unsigned long update_no,
				   XMLWriter& xml_out) 
  {
    // If xml file not empty, then use alternate
    if (params.xml_file != "")
    {
      std::string xml_file = makeXMLFileName(params.xml_file, update_no);

      push(xml_out, "NprVertexPDFRISMOM");
      write(xml_out, "update_no", update_no);
      write(xml_out, "xml_file", xml_file);
      pop(xml_out);

      XMLFileWriter xml(xml_file);
      func(update_no, xml);
    }
    else
    {
      func(update_no, xml_out);
    }
  }

                     

  // Function call
  void 
  InlineNprVertexPDFRISMOM::func(unsigned long update_no,
			     XMLWriter& XmlOut) 
  {
    START_CODE();

    StopWatch snoop;
    snoop.reset();
    snoop.start();

    push(XmlOut, "NprVertexPDFRISMOM");
    write(XmlOut, "update_no", update_no);

    QDPIO::cout << " ExampleNprVertexPDFRISMOM" << std::endl;
    QDPIO::cout << "     volume: " << QDP::Layout::lattSize()[0];
    for (int i=1; i<Nd; ++i) {
      QDPIO::cout << " x " << QDP::Layout::lattSize()[i];
    }
    QDPIO::cout << std::endl;

    //#################################################################################//
    // XML output
    //#################################################################################//

    proginfo(XmlOut);    // Print out basic program info

    push(XmlOut, "Output_version");
    write(XmlOut, "out_version", 2);
    pop(XmlOut);

    //###############################################################################//
    // Read Gauge Field                                                              //
    //###############################################################################//

    QDPIO::cout << "Attempt to initialize the gauge field" << std::endl << std::flush ;

    // Grab the gauge field
    multi1d<LatticeColorMatrix> U;
    XMLBufferWriter gauge_xml;

    try
    {
      U = TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(params.named_obj.gauge_id);
      TheNamedObjMap::Instance().get(params.named_obj.gauge_id).getRecordXML(gauge_xml);

      // Set the construct state and modify the fields
      {
	QDPIO::cout << "cfs=XX" << params.param.cfs.xml << "XX" << std::endl;
	std::istringstream  xml_s(params.param.cfs.xml);
	XMLReader  fermtop(xml_s);

	Handle<CreateFermState< LatticeFermion, multi1d<LatticeColorMatrix>, multi1d<LatticeColorMatrix> > > 
	  cfs(TheCreateFermStateFactory::Instance().createObject(params.param.cfs.id,
								 fermtop,
								 params.param.cfs.path));

	Handle<FermState< LatticeFermion, multi1d<LatticeColorMatrix>, multi1d<LatticeColorMatrix> > > 
	  state((*cfs)(U));

	// Pull the u fields back out from the state since they might have been
	// munged with fermBC's
	U = state->getLinks();
      }
    
    }
    catch( std::bad_cast ) 
    {
      QDPIO::cerr << InlineNprVertexPDFRISMOMEnv::name << ": caught dynamic cast error"
		  << std::endl;
      QDP_abort(1);
    }
    catch (const std::string& e) 
    {
      QDPIO::cerr << InlineNprVertexPDFRISMOMEnv::name << ": std::map call failed: " << e
		  << std::endl;
      QDP_abort(1);
    }
    catch( ... )
    {
      QDPIO::cerr << InlineNprVertexPDFRISMOMEnv::name << ": caught generic exception "
		  << std::endl;
      QDP_abort(1);
    }

    // Write out the input
    params.write(XmlOut, "Input");

    // Write out the config info
    write(XmlOut, "Config_info", gauge_xml);

    // Calculate some gauge invariant observables just for info.
    MesPlq(XmlOut, "Observables", U);

    //#################################################################################//
    // Read Forward Propagator                                                         //
    //#################################################################################//

    SftMom phases_nomom( 0, true, Nd-1 );  // used to check props. Fix to Nd-1 direction.

    LatticePropagator F,B, FB; // FB for S(p,p')
    ChromaProp_t prop_header_fwd, prop_header_bwd;
    PropSourceConst_t source_header_fwd, source_header_bwd;
    QDPIO::cout << "Attempt to parse forward propagator" << std::endl;
    QDPIO::cout << "parsing forward propagator " << params.named_obj.prop_id_fwd << " ... " << std::endl << std::flush;
	  QDPIO::cout << "Attempt to parse backward propagator" << std::endl;
   QDPIO::cout << "parsing backward propagator " << params.named_obj.prop_id_bwd << " ... " << std::endl << std::flush;
    

    try
    {
      // Snarf a copy
      F = TheNamedObjMap::Instance().getData<LatticePropagator>(params.named_obj.prop_id_fwd);
		B = TheNamedObjMap::Instance().getData<LatticePropagator>(params.named_obj.prop_id_bwd);
	
      // Snarf the frwd prop info. This is will throw if the frwd prop id is not there
      XMLReader PropXML, PropRecordXML;
      

      // Try to invert this record XML into a ChromaProp struct
      {
  TheNamedObjMap::Instance().get(params.named_obj.prop_id_fwd).getFileXML(PropXML);
  TheNamedObjMap::Instance().get(params.named_obj.prop_id_fwd).getRecordXML(PropRecordXML);
	read(PropRecordXML, "/Propagator/ForwardProp", prop_header_fwd);
	read(PropRecordXML, "/Propagator/PropSource", source_header_fwd);

		  
	TheNamedObjMap::Instance().get(params.named_obj.prop_id_bwd).getFileXML(PropXML);
	  TheNamedObjMap::Instance().get(params.named_obj.prop_id_bwd).getRecordXML(PropRecordXML);
	  read(PropRecordXML, "/Propagator/ForwardProp", prop_header_bwd);
	  read(PropRecordXML, "/Propagator/PropSource", source_header_bwd);
      }

      // Sanity check - write out the norm2 of the forward prop in the j_decay direction
      // Use this for any possible verification
      {
	multi1d<Double> PropCheckFWD =
	  sumMulti( localNorm2( F ), phases_nomom.getSet() );
	  multi1d<Double> PropCheckBWD =
		sumMulti( localNorm2( B ), phases_nomom.getSet() );

	QDPIO::cout << "forward propagator check = " << PropCheckFWD[0] << std::endl;
	  QDPIO::cout << "backward propagator check = " << PropCheckBWD[0] << std::endl;

	// Write out the forward propagator header
	push(XmlOut, "ForwardProp");
	write(XmlOut, "PropFWDXML", PropXML);
	write(XmlOut, "PropFWDRecordXML", PropRecordXML);
	write(XmlOut, "PropFWDCheck", PropCheckFWD);
	pop(XmlOut);
	
	  push(XmlOut, "BackwardProp");
	  write(XmlOut, "PropBWDXML", PropXML);
	  write(XmlOut, "PropBWDRecordXML", PropRecordXML);
	  write(XmlOut, "PropBWDCheck", PropCheckBWD);
	  pop(XmlOut);
      }
    }
    catch( std::bad_cast ) 
    {
      QDPIO::cerr << InlineNprVertexPDFRISMOMEnv::name << ": caught dynamic cast error"
		  << std::endl;
      QDP_abort(1);
    }
    catch (const std::string& e) 
    {
      QDPIO::cerr << InlineNprVertexPDFRISMOMEnv::name << ": forward prop: error message: " << e
		  << std::endl;
      QDP_abort(1);
    }

    QDPIO::cout << "Forward propagator successfully parsed" << std::endl;


    // Get the momentum from the header

    multi1d<int> mom_fwd, mom_bwd  ;
    multi1d<int> t_src_fwd, t_src_bwd;
    int       t_dir_fwd = source_header_fwd.j_decay ;
	  int       t_dir_bwd = source_header_fwd.j_decay ;

    try{
      mom_fwd = source_header_fwd.getMom() ;
      t_src_fwd = source_header_fwd.getTSrce() ;
		mom_bwd = source_header_bwd.getMom() ;
  t_src_bwd = source_header_bwd.getTSrce() ;
    }
    catch (const std::string& e){
      QDPIO::cerr << InlineNprVertexPDFRISMOMEnv::name << ": propagator does not have a momentum source or t_src not present: error message: " << e << std::endl;
      QDP_abort(1);
    }

    QDP::StopWatch swatch;
    swatch.reset();
  if(params.param.save_prop==true)    
  {
    //Fourier transform the propagator 
    QDPIO::cout << "Fourier Transforming propagator" << std::endl;

    swatch.start();
    std::string name_prop_fwd=params.param.file_name+"_fwd.ky_prop";
    writeKYUQprop2(F,name_prop_fwd);
	  std::string name_prop_bwd=params.param.file_name+"_bwd.ky_prop";
   writeKYUQprop2(B,name_prop_bwd);
    swatch.stop();
    QDPIO::cout << "finished write KY propagator"
                << "  time= "
                << swatch.getTimeInSeconds()
                << " secs" << std::endl;
  }

    LatticeReal theta_fwd=zero, theta_bwd=zero;
	  for(int id=0;id<4;id++) {
        theta_fwd+=twopi*Real(mom_fwd[id])/Real(Layout::lattSize()[id])* QDP::Layout::latticeCoordinate(id);
	  theta_bwd+=twopi*Real(mom_bwd[id])/Real(Layout::lattSize()[id])* QDP::Layout::latticeCoordinate(id);
	  }
    //need the oposit momentum on the sink
    LatticeComplex phase_fwd=cmplx(cos(theta_fwd),-sin(theta_fwd)), phase_bwd=cmplx(cos(theta_bwd),-sin(theta_bwd));
    F=F*phase_fwd;//project propagator to the given momentum.
	  B=B*phase_bwd;//project propagator to the given momentum.
	  FB=F*phase_bwd;//project S(p,z)to S(p,z)e^{-ip'z}
    
	  std::string name_prop_fwd=params.param.file_name+"_fwd.ftp", name_prop_bwd=params.param.file_name+"_bwd.ftp"; name_prop_mix=params.param.file_name+"_mix.ftp";
    write_FT_prop(F,name_prop_fwd,params.param.cfg_serial);
	  write_FT_prop(B,name_prop_bwd,params.param.cfg_serial);
	  write_FT_prop(FB,name_prop_mix,params.param.cfg_serial);

//    LatticeComplex phase2=cmplx(cos(theta),sin(theta));
//    F=F*phase;

/*
    sprintf(name,"../gs8t8iwa165_052.pdf.wall.%03d%03d%03d%03d.m0.010000",mom[0],mom[1],mom[2],mom[3]);
    readKYUQprop2(F,name);
    std::string ky_prop_name=std::string(name)+".test";
    writeKYUQprop2(F,ky_prop_name);
    
    name_chroma=params.param.file_name+".g";
    write_FT_prop(F,neg_mom,name_chroma);
*/

    //#################################################################################//
    // Construct Building Blocks                                                       //
    //#################################################################################//
    
    XMLBufferWriter file_xml;
    push(file_xml, "NprVertexPDF");
    write(file_xml, "Param", params.param);
    write(file_xml, "ForwardProp", prop_header_fwd);
    write(file_xml, "PropFWDSource", source_header_fwd);
	  write(file_xml, "BackwardProp", prop_header_bwd);
   write(file_xml, "PropBWDSource", source_header_bwd);
    write(file_xml, "Config", gauge_xml);
    pop(file_xml);


    XMLBufferWriter prop_fwd_xml;
    push(prop_fwd_xml,"QuarkPropagatorFWD");
    write(prop_fwd_xml,"mom_fwd",mom_fwd);
    write(prop_fwd_xml,"origin_fwd",t_src_fwd);
	  write(prop_fwd_xml,"mom_fwd",mom_fwd);
   write(prop_fwd_xml,"origin_fwd",t_src_fwd);
    write(prop_fwd_xml,"t_dir_fwd",t_dir_fwd);
    pop(prop_fwd_xml) ;
	  
	
	  XMLBufferWriter prop_bwd_xml;
   push(prop_bwd_xml,"QuarkPropagatorBWD");
   write(prop_bwd_xml,"mom_bwd",mom_bwd);
   write(prop_bwd_xml,"origin_bwd",t_src_bwd);
	 write(prop_bwd_xml,"mom_bwd",mom_bwd);
  write(prop_bwd_xml,"origin_bwd",t_src_bwd);
   write(prop_bwd_xml,"t_dir_bwd",t_dir_bwd);
   pop(prop_bwd_xml) ;

    QDPIO::cout << "Calculating building blocks" << std::endl;
    swatch.reset();
    swatch.start();
	std::string str=params.param.file_name+ ".io";

    NprVertexPDFRISMOM(F, B, U, params.param.links_max, AllLinkPatterns, str.c_str(),params.param.cfg_serial);
    swatch.stop();
      
    QDPIO::cout << "finished calculating NprVertexPDFRISMOM"
		<< "  time= "
		<< swatch.getTimeInSeconds() 
		<< " secs" << std::endl;

    pop(XmlOut);   // NprVertexPDF

    snoop.stop();
    QDPIO::cout << InlineNprVertexPDFRISMOMEnv::name << ": total time = "
		<< snoop.getTimeInSeconds() 
		<< " secs" << std::endl;

    QDPIO::cout << InlineNprVertexPDFRISMOMEnv::name << ": ran successfully" << std::endl;

    END_CODE();
  } 

}
