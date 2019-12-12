#include "PrabhupadaSlovary.h"
#include "PrabhupadaSlovaryConsts.h"

#define IMAGECLASS PrabhupadaSlovaryImg
#define IMAGEFILE <PrabhupadaSlovary/PrabhupadaSlovary.iml>
//#include <Draw/iml_source.h>
#include <Draw/iml.h>

namespace Prabhupada {

static Upp::Font GetGauraFont()
{
  static Upp::Font AGauraFont;
  static bool AGauraFontInit = false;
  if ( !AGauraFontInit ) {
    AGauraFont.FaceName( "Gaura Times" );
    AGauraFont.Height( 14 );
    AGauraFontInit = true;
  }
  return AGauraFont;
}

Upp::Value NumberToSanskrit::Format( const Upp::Value& q ) const {
  int n = q, i;
  Upp::AttrText at;
  if ( n < VectorSanskrit.DlinaVector ) {
    i  = VectorSanskrit[ n ].Index;
    at = VectorSanskrit[ i ].Sanskrit;
  } else
    at = Upp::FormatInt( n );
  at.SetFont( GetGauraFont() );
  return at;
}

Upp::Value NumberToPerevod::Format( const Upp::Value& q ) const {
  int n = q, i;
  Upp::AttrText at;
  if ( n < VectorSanskrit.DlinaVector ) {
    i  = VectorSanskrit[ n ].Index;
    at = VectorSanskrit[ i ].Perevod;
  } else
    at = Upp::FormatInt( n );
  at.SetFont( GetGauraFont() );
  return at;
}

PrabhupadaSlovaryPanel::PrabhupadaSlovaryPanel()
{
  CtrlLayout( *this );
  SanskritPoiskEdit.Tip( Upp::t_( "Поиск санскрита" ) );
  PerevodPoiskEdit.Tip(  Upp::t_( "Поиск перевода" ) );
  SplitterSearch.Horz() << SanskritPoiskEdit << PerevodPoiskEdit;
  SplitterSearch.SetMin( 0, 1000 );
  SplitterSearch.SetMin( 1, 1000 );
  EditIndicatorRow.NoWantFocus();

  ArraySanskrit.AddRowNumColumn( Upp::t_( "Санскрит" ), 50 ).SetConvert( FNumberToSanskrit ).Edit( EditSanskrit );
  ArraySanskrit.AddRowNumColumn( Upp::t_( "Перевод" ) , 50 ).SetConvert( FNumberToPerevod  ).Edit( EditPerevod  );
  const Upp::Font& gf = GetGauraFont();
  ArraySanskrit.SetLineCy( gf.GetCy() );
  ArraySanskrit.MultiSelect();

  Upp::Function< void () > ur;
  ur = [&] () { IndicatorRow(); };
  ArraySanskrit.WhenSel << ur;
  
  ToolBarSanskrit.ButtonMinSize( Upp::Size( 24, 24 ) );
  PrepareBar( ToolBarSanskrit );
       
  Upp::String db( Upp::ConfigFile( "Sanskrit.db" ) );
  if( !Session.Open( db ) ) {
    Upp::PromptOK( Upp::t_( "Не удалось открыть базу данных санскитского словаря Шрилы Прабхупады!" ) );
  } else {
    Session.LogErrors( true );
  }
  PrepareVectorYazyk();
}

void PrabhupadaSlovaryPanel::IndicatorRow()
{
  int r = ArraySanskrit.GetCursor() + 1;
  EditIndicatorRow.SetText( Upp::AsString( r ) + IndicatorSeparator + Upp::AsString( VectorSanskrit.DlinaVector ) );
}

void PrabhupadaSlovaryPanel::PrepareVectorYazyk()
{
  Upp::Sql sql( Session );
  int ID;
  Upp::String YAZYK;
  Upp::String YAZYK_SLOVO;
    
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
  Upp::Sql sql( Session );
  int ID;
  Upp::String IZNACHALYNO;
  Upp::String PEREVOD;
    
  sql.SetStatement( "select a.ID, a.IZNACHALYNO, a.PEREVOD from SANSKRIT a where a.YAZYK = ?" );
  sql.SetParam( 0, VectorYazyk[ Yazyk ].Yazyk );
  sql.Run();
  
  VectorSanskrit.Clear();

  int i = 0;
  SanskritPair PairSanskrit;
  VectorSanskrit.LastID = 0;
  
  while ( sql.Fetch() ) {
    ID          = sql[ 0 ];
    IZNACHALYNO = sql[ 1 ];
    PEREVOD     = sql[ 2 ];
  
    SanskritPair& RefPairSanskrit = VectorSanskrit.At( i, PairSanskrit );
  
    RefPairSanskrit.Index       = i++;
    RefPairSanskrit.IndexReserv = RefPairSanskrit.Index;
    RefPairSanskrit.Sanskrit    = IZNACHALYNO;
    RefPairSanskrit.Perevod     = PEREVOD;
    
    if ( VectorSanskrit.LastID < ID )
      VectorSanskrit.LastID = ID;
  }
  SetVectorSanskritDlinaVector( i );
}

void PrabhupadaSlovaryPanel::SetVectorSanskritDlinaVector( int d )
{
  VectorSanskrit.DlinaVector = d;
  ArraySanskrit.SetVirtualCount( VectorSanskrit.DlinaVector );
  ArraySanskrit.WhenSel();
}

void PrabhupadaSlovaryPanel::PrepareBar( Upp::Bar& bar )
{
  Upp::Event<> Dobavity_; // Gate<> для функторов, возвращающих логический тип
  Upp::Function< void( void ) > Udality_, Edit_, SmenaYazyka_, SortirovkaUstanovka_, Filter_, UdalityDubli_, CopyToClipboard_;

  Dobavity_        =  [&] { Dobavity(); };
  Udality_         << [&] { Udality();  };
  Edit_            << [&] { Edit(); };
  Filter_          << [&] { FilterUstanovka(); };
  UdalityDubli_    << [&] { UdalityDubli(); };
  CopyToClipboard_ << [&] { CopyToClipboard(); };
  
  bar.Add( Upp::t_( "Добавить" ),      PrabhupadaSlovaryImg::Dobavity(), Dobavity_ ).Key( Upp::K_CTRL_INSERT ).Help( Upp::t_( "Добавить новое слово и перевод" ) );
  bar.Add( Upp::t_( "Удалить" ),       PrabhupadaSlovaryImg::Udality(),  Udality_  ).Key( Upp::K_CTRL_DELETE ).Help( Upp::t_( "Удалить слово" ) );
  bar.Add( Upp::t_( "Редактировать" ), PrabhupadaSlovaryImg::Edit(),     Edit_     ).Key( Upp::K_CTRL_E      ).Help( Upp::t_( "Редактировать" ) );
  bar.Add( Upp::t_( "Поиск" ),         PrabhupadaSlovaryImg::Filter(),   Filter_   ).Key( Upp::K_ENTER       ).Help( Upp::t_( "Применить введенные слова для поиска" ) );
  bar.Add( Upp::t_( "Удалить дубли" ), PrabhupadaSlovaryImg::UdalityDubli(),
           UdalityDubli_ ).Key( Upp::K_ALT | Upp::K_CTRL_D  ).Help( Upp::t_( "Удалить дубли" ) );
  bar.Add( Upp::t_( "Копировать в буфер обмена" ), PrabhupadaSlovaryImg::CopyToClipboard(), CopyToClipboard_ ).Key( Upp::K_CTRL_C ).Help( Upp::t_( "Копировать в буфер обмена" ) );

  YazykDropList.Tip( Upp::t_( "Язык" ) );
  YazykDropList.DropWidthZ( 148 );
  YazykDropList.NoWantFocus();
  
  SmenaYazyka_          << [&] { SmenaYazyka(); };
  SortirovkaUstanovka_  << [&] { SortirovkaUstanovka(); };
  // WhenPush в отличие от WhenAction даёт существенно другое поведение DropList!
  // Когда WhenPush - похоже на кнопку и событие не возникает при выборе из выпадающего списка
  // когда WhenAction - на кнопку не похоже, сразу выпадает список и событие возникает при
  // выборе из списка
  YazykDropList.WhenAction = SmenaYazyka_;
  
  SortDropList.Add( Upp::t_( "Без сортировки" ) );
  SortDropList.Add( Upp::t_( "Санскрит по возрастанию" ) );
  SortDropList.Add( Upp::t_( "Санскрит по убыванию" ) );
  SortDropList.Add( Upp::t_( "Перевод по возрастанию" ) );
  SortDropList.Add( Upp::t_( "Перевод по убыванию" ) );
  SortDropList.Tip( Upp::t_( "Сортировка" ) );
  SortDropList.DropWidthZ( 180 );
  SortDropList.NoWantFocus();
  SortDropList.WhenPush = SortirovkaUstanovka_;
  
  bar.Add( YazykDropList, 138 );
  bar.Add( SortDropList, 170 );
}

void PrabhupadaSlovaryPanel::CopyToClipboard()
{
  Upp::PromptOK( "CopyToClipboard()!" );
}

void PrabhupadaSlovaryPanel::FilterUstanovka()
{
  FilterSlovary F;
  F.FilterSanskrit = ~SanskritPoiskEdit;
  F.FilterPerevod  = ~PerevodPoiskEdit;
  SetFilter( F );
}

void PrabhupadaSlovaryPanel::SetSortirovka( VidSortirovka s )
{
  if ( Sortirovka != s ) {
    Sortirovka = s;

    switch ( Sortirovka ) {
    case VidSortirovka::Reset :
			throw Upp::t_( "Сортировка VidSortirovka::Reset только для служебного пользования" );
      break;
    case VidSortirovka::NotSorted :
      VectorSanskrit.ResetIndex();
      break;
    case VidSortirovka::SanskritVozrastanie :
      Upp::SortIterSwap( VectorSanskrit
                       , [&] ( SanskritPair& a, SanskritPair& b ) { return VectorSanskrit[ a.Index ].Sanskrit == VectorSanskrit[ b.Index ].Sanskrit ?
                                                                           VectorSanskrit[ a.Index ].Perevod  <  VectorSanskrit[ b.Index ].Perevod :
                                                                           VectorSanskrit[ a.Index ].Sanskrit <  VectorSanskrit[ b.Index ].Sanskrit; }
                       , [] ( SanskritVector::iterator a, SanskritVector::iterator b ) { Upp::Swap( (*a).Index, (*b).Index ); } );
      break;
    case VidSortirovka::SanskritUbyvanie :
      Upp::SortIterSwap( VectorSanskrit
                       , [&] ( SanskritPair& a, SanskritPair& b ) { return VectorSanskrit[ a.Index ].Sanskrit == VectorSanskrit[ b.Index ].Sanskrit ?
                                                                           VectorSanskrit[ a.Index ].Perevod  >  VectorSanskrit[ b.Index ].Perevod :
                                                                           VectorSanskrit[ a.Index ].Sanskrit >  VectorSanskrit[ b.Index ].Sanskrit; }
                       , [] ( SanskritVector::iterator a, SanskritVector::iterator b ) { Upp::Swap( (*a).Index, (*b).Index ); } );
      break;
    case VidSortirovka::PerevodVozrastanie :
      Upp::SortIterSwap( VectorSanskrit
                       , [&] ( SanskritPair& a, SanskritPair& b ) { return VectorSanskrit[ a.Index ].Perevod  == VectorSanskrit[ b.Index ].Perevod ?
                                                                           VectorSanskrit[ a.Index ].Sanskrit <  VectorSanskrit[ b.Index ].Sanskrit :
                                                                           VectorSanskrit[ a.Index ].Perevod  <  VectorSanskrit[ b.Index ].Perevod; }
                       , [] ( SanskritVector::iterator a, SanskritVector::iterator b ) { Upp::Swap( (*a).Index, (*b).Index ); } );
      break;
    case VidSortirovka::PerevodUbyvanie :
      Upp::SortIterSwap( VectorSanskrit
                       , [&] ( SanskritPair& a, SanskritPair& b ) { return VectorSanskrit[ a.Index ].Perevod  == VectorSanskrit[ b.Index ].Perevod ?
                                                                           VectorSanskrit[ a.Index ].Sanskrit >  VectorSanskrit[ b.Index ].Sanskrit :
                                                                           VectorSanskrit[ a.Index ].Perevod  >  VectorSanskrit[ b.Index ].Perevod; }
                       , [] ( SanskritVector::iterator a, SanskritVector::iterator b ) { Upp::Swap( (*a).Index, (*b).Index ); } );
      break;
    }
    SortDropList.SetIndex( static_cast< int >( s ) );
    ArraySanskritRefresh();
  }
}

void PrabhupadaSlovaryPanel::SortirovkaUstanovka()
{
  VidSortirovka s = static_cast< VidSortirovka >( SortDropList.GetIndex() );
  SetSortirovka( s );
}

void PrabhupadaSlovaryPanel::ArraySanskritRefresh()
{
  ArraySanskrit.Refresh();
  ArraySanskrit.WhenSel();
  Upp::BeepInformation();
}

void PrabhupadaSlovaryPanel::SmenaYazyka()
{
  SetYazyk( YazykDropList.GetIndex() );
}

void PrabhupadaSlovaryPanel::UdalityDubli()
{
  if ( VectorSanskrit.IsEmpty() )
    return;

  FilterSlovary SaveFilter_ = Filter;
  Filter.Reset = true;

  // Сортируем по простому без выкрутасов!
  Upp::Sort( VectorSanskrit, [&] ( SanskritPair& a, SanskritPair& b ) { return a.Sanskrit == b.Sanskrit ? a.Perevod  <  b.Perevod : a.Sanskrit <  b.Sanskrit; } );
  
  SanskritPair& sp = VectorSanskrit[ 0 ];
  int i = 1;
  while ( i < VectorSanskrit.GetCount() ) {
    do {
      if ( i >= VectorSanskrit.GetCount() )
        goto STOP_REMOVE_DUBLI;
      if ( VectorSanskrit[ i ] == sp )
        VectorSanskrit.Remove( i );
      else
        break;
    } while ( true );
    sp = VectorSanskrit[ i ];
    ++i;
  }
  STOP_REMOVE_DUBLI:
  VectorSanskrit.ResetIndex();
  SetFilter( SaveFilter_ );
}

void PrabhupadaSlovaryPanel::Dobavity()
{
  Upp::PromptOK( "Dobavity()!" );
}

void PrabhupadaSlovaryPanel::Udality()
{
  Upp::PromptOK( "Udality()!" );
}

void PrabhupadaSlovaryPanel::Edit()
{
  Upp::PromptOK( "Редактирование!" );
}

PrabhupadaSlovaryWindow::PrabhupadaSlovaryWindow()
{
  Add( PanelPrabhupadaSlovary );
  PanelPrabhupadaSlovary.SizePos();
  Upp::Rect( 0, 0, 600, 600 );
  Title( Upp::t_( "Санскритский словарь Шрилы Прабхупады" ) ).Sizeable().Zoomable();
}

void PrabhupadaSlovaryWindow::Serialize( Upp::Stream& s )
{
  Upp::GuiLock __;
  int version = 0;
  s / version; // номер версии, чтобы при чтении можно разветвиться на разные версии параметров
  s.Magic( 346156 ); // запишите магическое число для проверки правильности данных
  SerializePlacement( s );
  PanelPrabhupadaSlovary.Serialize( s );
}

void PrabhupadaSlovaryPanel::Serialize( Upp::Stream& s )
{
  int version = 0;
  s / version;
  s.Magic( 954756 );

  ArraySanskrit.SerializeHeader( s );
  ArraySanskrit.SerializeSettings( s );
  SplitterSearch.Serialize( s );
  Filter.Serialize( s );

  int LastYazyk, LastCursor, LastSortirovka;
  if ( s.IsStoring() ) {
    LastCursor     = ArraySanskrit.GetCursor();
    LastYazyk      = Yazyk;
    LastSortirovka = static_cast< int >( Sortirovka );
  }
  s % LastCursor % LastYazyk % LastSortirovka;
  if ( s.IsLoading() ) {
    if ( StrongYazyk != -1 ) {
      if ( StrongYazyk != LastYazyk )
        LastCursor = 0;
      LastYazyk = StrongYazyk;
    }
    SetYazyk( LastYazyk );
    SetSortirovka( static_cast< VidSortirovka >( LastSortirovka ) );
    ArraySanskrit.SetCursor( LastCursor );
    SetFilter( Filter );
  }
}

void SanskritVector::SaveIndexToReserv()
{
  for ( int i = 0; i < GetCount(); ++i )
    (*this)[ i ].IndexReserv = (*this)[ i ].Index;
}

void SanskritVector::LoadIndexFromReserv()
{
  for ( int i = 0; i < GetCount(); ++i )
    (*this)[ i ].Index = (*this)[ i ].IndexReserv;
}

void SanskritVector::ResetIndex()
{
  for ( int i = 0; i < GetCount(); ++i ) {
    (*this)[ i ].Index       = i;
    (*this)[ i ].IndexReserv = i;
  }
}

void PrabhupadaSlovaryPanel::FilterVectorSanskrit()
{
  int uspeh = -1;
  for ( int i = 0; i < VectorSanskrit.GetCount(); ++i ) {
    //if (  )
  }
  SetVectorSanskritDlinaVector( VectorSanskrit.GetCount() );
}

void PrabhupadaSlovaryPanel::SetFilter( const FilterSlovary& F )
{
  if ( Filter != F || Filter.Reset == true  ) {
    Filter.Reset = false;
    if ( Filter.IsEmpty() )
      VectorSanskrit.SaveIndexToReserv();

    Filter = F;
    if ( Filter.IsEmpty() ) {
      VectorSanskrit.LoadIndexFromReserv();
      SetVectorSanskritDlinaVector( VectorSanskrit.GetCount() );
    } else
      FilterVectorSanskrit();
    
    SanskritPoiskEdit.SetText( Filter.FilterSanskrit );
    PerevodPoiskEdit.SetText( Filter.FilterPerevod );
  }
}

int YazykVector::FindYazyk( const Upp::String& S )
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
    ArraySanskritRefresh();
  }
}

} // namespace Prabhupada
