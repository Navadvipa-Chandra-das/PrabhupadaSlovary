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

template < class SortRange >
void RemoveDuplicates( SortRange& V )
{
	int n = 0, i = 1;
  while ( i < V.GetCount() ) {
    do {
      if ( i >= V.GetCount() )
        return;
      if ( V[ i ] == V[ n ] )
        V.Remove( i );
      else
        break;
    } while ( true );
    ++n;
    ++i;
  }
};

template < class SortRange, class OnRemove >
void RemoveDuplicates( SortRange& V, const OnRemove& On_Remove )
{
  int n = 0, i = 1;
  while ( i < V.GetCount() ) {
    do {
      if ( i >= V.GetCount() )
        return;
      if ( V[ i ] == V[ n ] ) {
        On_Remove( i );
        V.Remove( i );
      } else
        break;
    } while ( true );
    ++n;
    ++i;
  }
};

} // namespace Prabhupada

#endif
