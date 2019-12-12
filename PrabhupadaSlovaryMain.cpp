#include "PrabhupadaSlovary.h"
#include "PrabhupadaSlovaryConsts.h"
#include "PrabhupadaSlovaryAlgorithm.h"

GUI_APP_MAIN
{
  Upp::StdLogSetup( Upp::LOG_FILE | Upp::LOG_COUT | Upp::LOG_TIMESTAMP );
  Upp::SetLanguage( Upp::SetLNGCharset( Upp::GetSystemLNG(), CHARSET_UTF8 ) );

  Prabhupada::PrabhupadaSlovaryWindow WindowPrabhupadaSlovary;
  
  Upp::Function< void( Upp::Stream& ) > Serial;
  Serial = [&] ( Upp::Stream& S ) { WindowPrabhupadaSlovary.Serialize( S ); };
  
  const Upp::Vector< Upp::String >& cl = Upp::CommandLine();

  Prabhupada::CommandMap cm;
  cm.Add( "NoLoadINI", Prabhupada::CommandInfo() );
  cm.Add( "Lang",      Prabhupada::CommandInfo() );
  cm.Add( "INI_Place", Prabhupada::CommandInfo() );
  cm.Prepare( cl );
  DDUMPM( cm );

  int i, &StrongYazyk = WindowPrabhupadaSlovary.PanelPrabhupadaSlovary.StrongYazyk;
  Prabhupada::PrabhupadaSlovaryPanel::VidSortirovka InitSortirovka = Prabhupada::PrabhupadaSlovaryPanel::VidSortirovka::Reset;
  Prabhupada::CommandInfo& ci = cm.GetPut( "Lang" );

  if ( ci.Present ) {
    i = WindowPrabhupadaSlovary.PanelPrabhupadaSlovary.VectorYazyk.FindYazyk( ci.Value );
    StrongYazyk = i > 0 ? WindowPrabhupadaSlovary.PanelPrabhupadaSlovary.VectorYazyk[ i ].ID : Prabhupada::RussianYazyk;
  }
  
  ci = cm.GetPut( "NoLoadINI" );

  if ( !ci.Present ) {
    if ( !Upp::LoadFromFile( Serial, Prabhupada::PrabhupadaSlovaryIniFile, 0 ) && StrongYazyk == -1 ) {
      StrongYazyk = Prabhupada::RussianYazyk;
      InitSortirovka = Prabhupada::PrabhupadaSlovaryPanel::VidSortirovka::SanskritVozrastanie;
    }
  } else
      InitSortirovka = Prabhupada::PrabhupadaSlovaryPanel::VidSortirovka::SanskritVozrastanie;

  if ( StrongYazyk != -1 )
    WindowPrabhupadaSlovary.PanelPrabhupadaSlovary.SetYazyk( StrongYazyk );
  if ( InitSortirovka != Prabhupada::PrabhupadaSlovaryPanel::VidSortirovka::Reset )
    WindowPrabhupadaSlovary.PanelPrabhupadaSlovary.SetSortirovka( InitSortirovka );
  
  Prabhupada::FilterSlovary F = WindowPrabhupadaSlovary.PanelPrabhupadaSlovary.Filter;
  WindowPrabhupadaSlovary.PanelPrabhupadaSlovary.Filter.Clear();
  WindowPrabhupadaSlovary.PanelPrabhupadaSlovary.SetFilter( F );
  
  WindowPrabhupadaSlovary.Run();
  Upp::StoreToFile(  Serial, Prabhupada::PrabhupadaSlovaryIniFile, 0 );
}
