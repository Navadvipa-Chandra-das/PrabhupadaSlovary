#ifndef _PrabhupadaSlovary_PrabhupadaSlovaryAlgorithm_h_
#define _PrabhupadaSlovary_PrabhupadaSlovaryAlgorithm_h_

#include <CtrlLib/CtrlLib.h>

namespace Prabhupada {

class CommandInfo : Upp::Moveable< CommandInfo > {
public:
  bool Present = false;
  Upp::String Value;
  Upp::String ToString() const { return Upp::AsString( Present ) + " : " + Value; }
};

class CommandMap : public Upp::VectorMap< Upp::String, CommandInfo > {
public:
  void Prepare( const Upp::Vector< Upp::String >& cl );
};

} // namespace Prabhupada

#endif
