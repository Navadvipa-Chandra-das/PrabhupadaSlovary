#include "PrabhupadaSlovaryAlgorithm.h"

namespace Prabhupada {

void CommandMap::Prepare( const Upp::Vector< Upp::String >& cl )
{
  int r, len_key, len_command;
  Upp::String Key;

  for ( int i = 0; i < GetCount(); ++i ) {
    Key = GetKey( i );
    r = Upp::FindMatch( cl, [&] ( Upp::String S ) { return S.Find( Key ) != -1; } );
    if ( r >= 0 ) {
      bool FindFlag = true;
      len_key     = Key.GetLength();
      len_command = cl[ r ].GetLength();
      if ( len_command > len_key ) {
        if ( cl[ r ][ len_key ] == '=' )
          ++len_key;
        else
          FindFlag = false;
      }
      if ( FindFlag ) {
        CommandInfo& ci = GetValues()[ i ];
        ci.Value   = cl[ r ].Right( len_command - len_key );
        ci.Present = true;
      }
    }
  }
}

} // namespace Prabhupada
