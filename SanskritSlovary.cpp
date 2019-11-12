#include "SanskritSlovary.h"

SanskritSlovaryPanel::SanskritSlovaryPanel()
{
  CtrlLayout( *this );
  SplitterSearch.Horz( SansritEdit, PerevodEdit );
}

SanskritSlovaryWindow::SanskritSlovaryWindow()
{
  Add( PanelSanskritSlovary );
  PanelSanskritSlovary.SizePos();
  Rect( 0, 0, 600, 600 );
  Title( "Словарь санскрита Шрилы Прабхупады" ).Sizeable().Zoomable();
}
