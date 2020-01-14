#include "PrabhupadaSlovary.h"
#include "PrabhupadaSlovaryConsts.h"
#include "PrabhupadaSlovaryAlgorithm.h"

GUI_APP_MAIN
{
  Upp::StdLogSetup( Upp::LOG_FILE | Upp::LOG_COUT | Upp::LOG_TIMESTAMP );

  const Upp::Vector< Upp::String >& cl = Upp::CommandLine();

  Prabhupada::CommandMap cm;
  cm.Add( "NoLoadINI", Prabhupada::CommandInfo() );
  cm.Add( "Lang",      Prabhupada::CommandInfo() );
  cm.Add( "LangGUI",   Prabhupada::CommandInfo() );
  cm.Add( "INI_Place", Prabhupada::CommandInfo() );
  cm.Prepare( cl );

  Prabhupada::CommandInfo& ci = cm.Get( "LangGUI" );
  if ( ci.Present )
    Upp::SetLanguage( Upp::SetLNGCharset( Upp::LNGFromText( ci.Value ), CHARSET_UTF8 ) );
  else
    Upp::SetLanguage( Upp::SetLNGCharset( Upp::GetSystemLNG(), CHARSET_UTF8 ) );

  Prabhupada::PrabhupadaSlovaryWindow WindowPrabhupadaSlovary( cm );
  Upp::Ctrl::SetAppName( Upp::t_( "Srila Prabhupada's Dictionary!" ) );
  WindowPrabhupadaSlovary.Run();
}
