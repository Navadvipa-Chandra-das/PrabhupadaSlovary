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

  int& StrongYazyk = WindowPrabhupadaSlovary.PanelPrabhupadaSlovary.StrongYazyk;
  int i = FindMatch( cl, []( String S ) { return S.Find( "Lang=" ) != -1; } );

  if ( i >= 0 ) {
    String y = cl[ i ].Right( cl[ i ].GetLength() - 5 );
    i = WindowPrabhupadaSlovary.PanelPrabhupadaSlovary.VectorYazyk.FindYazyk( y );
    StrongYazyk = WindowPrabhupadaSlovary.PanelPrabhupadaSlovary.VectorYazyk[ i ].ID;
  }
  
  i = FindIndex( cl, "NoLoadINI" );

  if ( i == -1 ) {
    if ( !LoadFromFile( Serial, PrabhupadaSlovaryIniFile, 0 ) && StrongYazyk == -1 )
      StrongYazyk = RussianYazyk;
  }

  if ( StrongYazyk != -1 )
    WindowPrabhupadaSlovary.PanelPrabhupadaSlovary.SetYazyk( StrongYazyk );
  WindowPrabhupadaSlovary.Run();
  StoreToFile(  Serial, PrabhupadaSlovaryIniFile, 0 );
}
