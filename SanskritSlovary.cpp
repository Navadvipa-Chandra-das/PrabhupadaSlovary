#include "SanskritSlovary.h"

#define IMAGECLASS SanskritSlovaryImg
#define IMAGEFILE <SanskritSlovary/SanskritSlovary.iml>
//#include <Draw/iml_source.h>
#include <Draw/iml.h>

Font GetGauraFont()
{
  static Font AGauraFont;
  static bool AGauraFontInit = false;
  if ( !AGauraFontInit ) {
    AGauraFont.FaceName( "Gaura Times" );
    AGauraFont.Height( 14 );
    AGauraFontInit = true;
    DUMP( AGauraFont );
  }
  return AGauraFont;
}

struct NumberToSanskrit : public Convert {
  virtual Value  Format( const Value& q ) const {
    int n = q;
    AttrText at( FormatInt( n ) );
    at.SetFont( GetGauraFont() );
    return at;
  }
};

struct NumberToPerevod : public Convert {
  virtual Value  Format( const Value& q ) const {
    int n = q;
    AttrText at( FormatInt( n + 1 ) );
    at.SetFont( GetGauraFont() );
    return at;
  }
};

SanskritSlovaryPanel::SanskritSlovaryPanel()
{
  CtrlLayout( *this );
  SplitterSearch.Horz( SansritEdit, PerevodEdit );
  SplitterSearch.SetMin( 0, 1000 );
  SplitterSearch.SetMin( 1, 2000 );

  ArraySanskrit.AddRowNumColumn( "Санскрит", 50 ).SetConvert( Single< NumberToSanskrit >() ).Edit( EditSanskrit );
  ArraySanskrit.AddRowNumColumn( "Перевод" , 50 ).SetConvert( Single< NumberToPerevod  >() ).Edit( EditPerevod  );
  ArraySanskrit.SetVirtualCount( 900000 );
  
  ToolBarSanskrit.ButtonMinSize( Size( 24, 24 ) );
  PrepareBar( ToolBarSanskrit );
       
  String db( ConfigFile( "Sanskrit.db" ) );
  if( !sqlite3.Open( db ) ) {
    PromptOK( "Не удалось открыть базу данных санскитского словаря Шрилы Прабхупады!" );
  } else {
    sqlite3.LogErrors( true );
    DLOG( "Всё в порядке с открытием базы данных санскитского словаря Шрилы Прабхупады!" );
  }
  PrepareVectorSanskrit();
}

void SanskritSlovaryPanel::PrepareVectorSanskrit()
{
  Sql sql( sqlite3 );
  SanskritPair PairSanskrit;

	int ID;
	String IZNACHALYNO;
	String PEREVOD;
		
	sql.SetStatement( "select a.ID, a.IZNACHALYNO, a.PEREVOD from SANSKRIT a where a.YAZYK = ? limit 200" );
	sql.SetParam( 0, "ru" );
	sql.Run();

	while ( sql.Fetch() ) {
    ID          = sql[ 0 ];
    IZNACHALYNO = sql[ 1 ];
    PEREVOD     = sql[ 2 ];

    SanskritPair& PairSanskrit = VectorSanskrit.Add();
    
    
	  PairSanskrit.Sanskrit = IZNACHALYNO;
	  PairSanskrit.Perevod  = PEREVOD;
	}
  DDUMPC( VectorSanskrit );
}

void SanskritSlovaryPanel::PrepareBar( Bar& bar )
{
  Event<> dobavity; // Gate<> для функторов, возвращающих логический тип
  Function< void (void) > udality;

  dobavity =  [&] { Dobavity(); };
  udality  << [&] { Udality();  };
  udality  << [&] { Udality2(); };
  udality  << THISFN( Udality3 );
  
  bar.Add( "New", SanskritSlovaryImg::Dobavity(), dobavity ).Key( K_CTRL_N ).Help( "Open new window" );
  bar.Add( "Open..", SanskritSlovaryImg::Udality(), udality ).Key( K_CTRL_O ).Help( "Open existing document" );

  YazykDropList.Add( "RU" );
  YazykDropList.Add( "EN" );
  YazykDropList.Tip( t_( "Язык" ) ).LeftPosZ(0, 64).TopPosZ(0, 19);
  YazykDropList.SetIndex( 0 );
  
  bar.Add( YazykDropList, 68 );
}

void SanskritSlovaryPanel::Dobavity()
{

}

void SanskritSlovaryPanel::Udality()
{
  PromptOK( "Не удалось!" );
}
void SanskritSlovaryPanel::Udality2()
{
  PromptOK( "Не удалось, чеснісеньке піонерське!" );
}
void SanskritSlovaryPanel::Udality3()
{
  PromptOK( "Не удалось! Всё по чесноку!" );
}

SanskritSlovaryWindow::SanskritSlovaryWindow()
{
  Add( PanelSanskritSlovary );
  PanelSanskritSlovary.SizePos();
  Rect( 0, 0, 600, 600 );
  Title( t_( "Санскритский словарь Шрилы Прабхупады" ) ).Sizeable().Zoomable();
}

void SanskritSlovaryWindow::SerializeApp( Stream& s )
{
  int version = 0;
  s / version; // номер версии, что бы в будущем его поменять и не конфликтовать со старыми данными
  s.Magic( 346156 ); // запишите магическое число для проверки правильности данных
  //s % number % color;
  SerializePlacement( s );
}
