#include "SanskritSlovary.h"
#include "SanskritSlovaryConsts.h"

GUI_APP_MAIN
{
  StdLogSetup( LOG_FILE | LOG_COUT | LOG_TIMESTAMP );
  SetLanguage( SetLNGCharset( GetSystemLNG(), CHARSET_UTF8 ) );
	
  SanskritSlovaryWindow WindowSanskritSlovary;
  
  Function< void( Stream& ) > Serial;
  Serial = [&] ( Stream& S ) { WindowSanskritSlovary.Serialize( S ); };
  
  if ( !LoadFromFile( Serial, SanskritSlovaryIniFile, 0 ) )
    WindowSanskritSlovary.PanelSanskritSlovary.SetYazyk( RussianYazyk );
  WindowSanskritSlovary.Run();
  StoreToFile(  Serial, SanskritSlovaryIniFile, 0 );
}
