#include "PrabhupadaSlovary.h"
#include "PrabhupadaSlovaryConsts.h"

GUI_APP_MAIN
{
  StdLogSetup( LOG_FILE | LOG_COUT | LOG_TIMESTAMP );
  SetLanguage( SetLNGCharset( GetSystemLNG(), CHARSET_UTF8 ) );
	
  PrabhupadaSlovaryWindow WindowPrabhupadaSlovary;
  
  Function< void( Stream& ) > Serial;
  Serial = [&] ( Stream& S ) { WindowPrabhupadaSlovary.Serialize( S ); };
  
  const Vector< String >& cl = CommandLine();
  DDUMPC( cl );
  int i = FindMatch( cl, "Lang=" );
  if ( i == -1 )
    if ( !LoadFromFile( Serial, PrabhupadaSlovaryIniFile, 0 ) )
      WindowPrabhupadaSlovary.PanelPrabhupadaSlovary.SetYazyk( RussianYazyk );
  WindowPrabhupadaSlovary.Run();
  StoreToFile(  Serial, PrabhupadaSlovaryIniFile, 0 );
}
