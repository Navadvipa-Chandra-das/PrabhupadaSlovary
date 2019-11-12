#include "SanskritSlovary.h"

#define IMAGECLASS SanskritSlovaryImg
#define IMAGEFILE <SanskritSlovary/SanskritSlovary.iml>
//#include <Draw/iml_source.h>
#include <Draw/iml.h>

struct NumberToSanskrit : public Convert {
	virtual Value  Format( const Value& q ) const {
		int n = q;
		return n == 0 ? String( "zero" ) : FormatInt( n );
	}
};

struct NumberToPerevod : public Convert {
	virtual Value  Format( const Value& q ) const {
		int n = q;
		return n == 0 ? String( "zero" ) : FormatInt( n );
	}
};

SanskritSlovaryPanel::SanskritSlovaryPanel()
{
  CtrlLayout( *this );
  SplitterSearch.Horz( SansritEdit, PerevodEdit );
  SplitterSearch.SetMin( 0, 1000 );
  SplitterSearch.SetMin( 1, 2000 );
  ArraySanskrit.AddRowNumColumn( "Санскрит", 50 ).SetConvert( Single< NumberToSanskrit >() );
  ArraySanskrit.AddRowNumColumn( "Перевод", 50 ).SetConvert( Single< NumberToPerevod >() );
  ArraySanskrit.SetVirtualCount( 900000 );
  ToolBarSanskrit.ButtonMinSize( Size( 24, 24 ) );

  ToolBarSanskrit.Add("New", SanskritSlovaryImg::Dobavity(), THISBACK( Dobavity ))
       .Key( K_CTRL_N )
       .Help( "Open new window" );

  ToolBarSanskrit.Add("Open..", SanskritSlovaryImg::Udality(), THISBACK( Udality ))
       .Key(K_CTRL_O)
       .Help("Open existing document");
}

void SanskritSlovaryPanel::Dobavity()
{

}

void SanskritSlovaryPanel::Udality()
{

}

SanskritSlovaryWindow::SanskritSlovaryWindow()
{
  Add( PanelSanskritSlovary );
  PanelSanskritSlovary.SizePos();
  Rect( 0, 0, 600, 600 );
  Title( "Словарь санскрита Шрилы Прабхупады" ).Sizeable().Zoomable();
}
