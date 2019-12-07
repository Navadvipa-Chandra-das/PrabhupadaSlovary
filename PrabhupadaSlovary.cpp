#include "PrabhupadaSlovary.h"
#include "PrabhupadaSlovaryConsts.h"

#define IMAGECLASS PrabhupadaSlovaryImg
#define IMAGEFILE <PrabhupadaSlovary/PrabhupadaSlovary.iml>
//#include <Draw/iml_source.h>
#include <Draw/iml.h>

static Font GetGauraFont()
{
  static Font AGauraFont;
  static bool AGauraFontInit = false;
  if ( !AGauraFontInit ) {
    AGauraFont.FaceName( "Gaura Times" );
    AGauraFont.Height( 14 );
    AGauraFontInit = true;
  }
  return AGauraFont;
}

Value NumberToSanskrit::Format( const Value& q ) const {
  int n = q, i;
  AttrText at;
  if ( n < VectorSanskrit.DlinaVector ) {
    i  = VectorSanskrit[ n ].Index;
    at = VectorSanskrit[ i ].Sanskrit;
  } else
    at = FormatInt( n );
  at.SetFont( GetGauraFont() );
  return at;
}

Value NumberToPerevod::Format( const Value& q ) const {
  int n = q, i;
  AttrText at;
  if ( n < VectorSanskrit.DlinaVector ) {
    i  = VectorSanskrit[ n ].Index;
    at = VectorSanskrit[ i ].Perevod;
  } else
    at = FormatInt( n );
  at.SetFont( GetGauraFont() );
  return at;
}

PrabhupadaSlovaryPanel::PrabhupadaSlovaryPanel()
{
  CtrlLayout( *this );
  SanskritPoiskEdit.Tip( t_( "Поиск санскрита" ) );
  PerevodPoiskEdit.Tip(  t_( "Поиск перевода" ) );
  SplitterSearch.Horz() << SanskritPoiskEdit << PerevodPoiskEdit;
  SplitterSearch.SetMin( 0, 1000 );
  SplitterSearch.SetMin( 1, 1000 );
  EditIndicatorRow.NoWantFocus();

  ArraySanskrit.AddRowNumColumn( "Санскрит", 50 ).SetConvert( FNumberToSanskrit ).Edit( EditSanskrit );
  ArraySanskrit.AddRowNumColumn( "Перевод" , 50 ).SetConvert( FNumberToPerevod  ).Edit( EditPerevod  );
  const Font& gf = GetGauraFont();
  ArraySanskrit.SetLineCy( gf.GetCy() );

  Function< void () > ur;
  ur = [&] () { IndicatorRow(); };
  ArraySanskrit.WhenSel << ur;
  
  ToolBarSanskrit.ButtonMinSize( Size( 24, 24 ) );
  PrepareBar( ToolBarSanskrit );
       
  String db( ConfigFile( "Sanskrit.db" ) );
  if( !Session.Open( db ) ) {
    PromptOK( "Не удалось открыть базу данных санскитского словаря Шрилы Прабхупады!" );
  } else {
    Session.LogErrors( true );
  }
  PrepareVectorYazyk();
}

void PrabhupadaSlovaryPanel::IndicatorRow()
{
  int r = ArraySanskrit.GetCursor() + 1;
  EditIndicatorRow.SetText( AsString( r ) + IndicatorSeparator + AsString( VectorSanskrit.DlinaVector ) );
}

void PrabhupadaSlovaryPanel::PrepareVectorYazyk()
{
  Sql sql( Session );
  int ID;
  String YAZYK;
  String YAZYK_SLOVO;
    
  sql.SetStatement( "select a.ID, a.YAZYK, a.YAZYK_SLOVO from YAZYK a" );
  sql.Run();

  while ( sql.Fetch() ) {
    ID          = sql[ 0 ];
    YAZYK       = sql[ 1 ];
    YAZYK_SLOVO = sql[ 2 ];
  
    YazykInfo& InfoYazyk = VectorYazyk.Add();
  
    InfoYazyk.ID         = ID;
    InfoYazyk.Yazyk      = YAZYK;
    InfoYazyk.YazykSlovo = YAZYK_SLOVO;
  }
  for ( YazykVector::iterator i = VectorYazyk.begin(); i != VectorYazyk.end(); ++i ) {
    YazykDropList.Add( (*i).YazykSlovo );
  }
}

void PrabhupadaSlovaryPanel::PrepareVectorSanskrit()
{
  if ( Yazyk == -1 ) return;
  Sql sql( Session );
  int ID;
  String IZNACHALYNO;
  String PEREVOD;
    
  sql.SetStatement( "select a.ID, a.IZNACHALYNO, a.PEREVOD from SANSKRIT a where a.YAZYK = ?" );
  sql.SetParam( 0, VectorYazyk[ Yazyk ].Yazyk );
  sql.Run();
  
  VectorSanskrit.Clear();

  int i = 0;
  SanskritPair PairSanskrit;
  
  while ( sql.Fetch() ) {
    ID          = sql[ 0 ];
    IZNACHALYNO = sql[ 1 ];
    PEREVOD     = sql[ 2 ];
  
    SanskritPair& RefPairSanskrit = VectorSanskrit.At( i, PairSanskrit );
  
    RefPairSanskrit.Index    = i++;
    RefPairSanskrit.Sanskrit = IZNACHALYNO;
    RefPairSanskrit.Perevod  = PEREVOD;
  }
  VectorSanskrit.DlinaVector = i;
  ArraySanskrit.SetVirtualCount( VectorSanskrit.DlinaVector );
}

void PrabhupadaSlovaryPanel::PrepareBar( Bar& bar )
{
  Event<> dobavity; // Gate<> для функторов, возвращающих логический тип
  Function< void (void) > udality, smenaYazyka;

  dobavity =  [&] { Dobavity(); };
  udality  << [&] { Udality();  };
  udality  << [&] { Udality2(); };
  udality  << THISFN( Udality3 );
  
  bar.Add( "New", PrabhupadaSlovaryImg::Dobavity(), dobavity ).Key( K_CTRL_N ).Help( "Open new window" );
  bar.Add( "Open..", PrabhupadaSlovaryImg::Udality(), udality ).Key( K_CTRL_O ).Help( "Open existing document" );

  YazykDropList.Tip( t_( "Язык" ) );
  YazykDropList.DropWidthZ( 148 );
  YazykDropList.NoWantFocus();
  smenaYazyka  << [&] { SmenaYazyka(); };
  // WhenPush в отличие от WhenAction даёт существенно другое поведение DropList!
  // Когда WhenPush - похоже на кнопку и событие не возникает при выборе из выпадающего списка
  // когда WhenAction - на кнопку не похоже, сразу выпадает список и событие возникает при
  // выборе из списка
  YazykDropList.WhenAction = smenaYazyka;
  
  bar.Add( YazykDropList, 138 );
}

void PrabhupadaSlovaryPanel::SmenaYazyka()
{
  SetYazyk( YazykDropList.GetIndex() );
}

void PrabhupadaSlovaryPanel::Dobavity()
{

}

void PrabhupadaSlovaryPanel::Udality()
{
  PromptOK( "Не удалось!" );
}
void PrabhupadaSlovaryPanel::Udality2()
{
  PromptOK( "Не удалось, чеснісеньке піонерське!" );
}
void PrabhupadaSlovaryPanel::Udality3()
{
  PromptOK( "Не удалось! Всё по чесноку!" );
}

PrabhupadaSlovaryWindow::PrabhupadaSlovaryWindow()
{
  Add( PanelPrabhupadaSlovary );
  PanelPrabhupadaSlovary.SizePos();
  Rect( 0, 0, 600, 600 );
  Title( t_( "Санскритский словарь Шрилы Прабхупады" ) ).Sizeable().Zoomable();
}

void PrabhupadaSlovaryWindow::Serialize( Stream& s )
{
  GuiLock __;
  int version = 0;
  s / version; // номер версии, чтобы при чтении можно разветвиться на разные версии параметров
  s.Magic( 346156 ); // запишите магическое число для проверки правильности данных
  SerializePlacement( s );
  PanelPrabhupadaSlovary.Serialize( s );
}

void PrabhupadaSlovaryPanel::Serialize( Stream& s )
{
  int version = 0;
  s / version;
  s.Magic( 954756 );

  ArraySanskrit.SerializeHeader( s );
  ArraySanskrit.SerializeSettings( s );
  SplitterSearch.Serialize( s );

  int LastYazyk, LastCursor;
  if ( s.IsStoring() ) {
    LastCursor = ArraySanskrit.GetCursor();
    LastYazyk  = Yazyk;
  }
  s % LastCursor % LastYazyk;
  if ( s.IsLoading() ) {
    if ( StrongYazyk != -1 ) {
      if ( StrongYazyk != LastYazyk )
        LastCursor = 0;
      LastYazyk = StrongYazyk;
    }
    SetYazyk( LastYazyk );
    ArraySanskrit.SetCursor( LastCursor );
  }
}

int YazykVector::FindYazyk( const String& S )
{
	for( int i = 0; i < GetCount(); ++i )
		if ( (*this)[i].Yazyk == S )
		  return i;
	return -1;
}

void PrabhupadaSlovaryPanel::SetYazyk( int y )
{
  if ( Yazyk != y ) {
    int c = ArraySanskrit.GetCursor();
    Yazyk = y;
    PrepareVectorSanskrit();
    YazykDropList.SetIndex( y );
    if ( c < 0 )
      ArraySanskrit.GoBegin();
    else
      ArraySanskrit.SetCursor( c );
    ArraySanskrit.Refresh();
    ArraySanskrit.WhenSel();
    // Ура, победа!
    BeepInformation();
  }
}
