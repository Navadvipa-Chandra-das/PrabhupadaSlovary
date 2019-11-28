#include "SanskritSlovary.h"
#include "SanskritSlovaryConsts.h"

GUI_APP_MAIN
{
  StdLogSetup( LOG_FILE | LOG_COUT | LOG_TIMESTAMP );
  SetLanguage( SetLNGCharset( GetSystemLNG(), CHARSET_UTF8 ) );
	
  SanskritSlovaryWindow WindowSanskritSlovary;
  
  Event< Stream& > Serial;
  Serial = [&] ( Stream& S ) { WindowSanskritSlovary.SerializeApp( S ); };
  
  LoadFromFile( Serial, SanskritSlovaryIniFile, 0 );
  WindowSanskritSlovary.Run();
  StoreToFile(  Serial, SanskritSlovaryIniFile, 0 );
}
