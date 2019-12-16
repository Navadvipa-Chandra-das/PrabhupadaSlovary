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
    SanskritPair& sp = VectorSanskrit[ i ];
    at = sp.Sanskrit;
    if ( sp.DeleteCandidat && !ArraySanskrit.IsSelected( n ) ) {
      at.ink   = ColorInkDelete;
      at.paper = ColorPaperDelete;
    }
  } else
    at = Upp::FormatInt( n );
  at.SetFont( GauraFont );
  return at;
}

Upp::Value NumberToPerevod::Format( const Upp::Value& q ) const {
  int n = q, i;
  Upp::AttrText at;
  if ( n < VectorSanskrit.DlinaVector ) {
    i  = VectorSanskrit[ n ].Index;
    SanskritPair& sp = VectorSanskrit[ i ];
    at = sp.Perevod;
    if ( sp.DeleteCandidat && !ArraySanskrit.IsSelected( n ) ) {
      at.ink   = ColorInkDelete;
      at.paper = ColorPaperDelete;
    }
  } else
    at = Upp::FormatInt( n );
  at.SetFont( GauraFont );
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
  GauraFont = gf;
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
  int ID;
  Upp::String YAZYK;
  Upp::String YAZYK_SLOVO;
    
  Upp::Sql sql( Session );
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
  sql.Clear();
  for ( YazykVector::iterator i = VectorYazyk.begin(); i != VectorYazyk.end(); ++i ) {
    YazykDropList.Add( (*i).YazykSlovo );
  }
}

void PrabhupadaSlovaryPanel::PrepareVectorSanskrit()
{
  if ( Yazyk == -1 ) return;
  int ID;
  Upp::String IZNACHALYNO;
  Upp::String PEREVOD;
  
  Upp::Sql sql( Session );
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
    RefPairSanskrit.ReservIndex = RefPairSanskrit.Index;
    RefPairSanskrit.ID          = ID;
    RefPairSanskrit.Sanskrit    = IZNACHALYNO;
    RefPairSanskrit.Perevod     = PEREVOD;
    
    if ( VectorSanskrit.LastID < ID )
      VectorSanskrit.LastID = ID;
  }
  sql.Clear();
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
  Upp::Event<> AddSlovo_; // Gate<> для функторов, возвращающих логический тип
  Upp::Function< void( void ) > RemoveSlovo_, Edit_, SmenaYazyka_, SortirovkaUstanovka_, Filter_, RemoveDubli_, CopyToClipboard_, DeleteSlova_;

  AddSlovo_        =  [&] { AddSlovo(); };
  RemoveSlovo_     << [&] { RemoveSlovo(); };
  Edit_            << [&] { Edit(); };
  Filter_          << [&] { FilterUstanovka(); };
  RemoveDubli_     << [&] { RemoveDubli(); };
  CopyToClipboard_ << [&] { CopyToClipboard(); };
  DeleteSlova_     << [&] { DeleteSlova(); };
  
  bar.Add( Upp::t_( "Добавить" ),      PrabhupadaSlovaryImg::AddSlovo(), AddSlovo_ ).Key( Upp::K_CTRL_INSERT ).Help( Upp::t_( "Добавить новое слово и перевод" ) );
  bar.Add( Upp::t_( "Пометить для удаления" ), PrabhupadaSlovaryImg::RemoveSlovo(),  RemoveSlovo_  ).Key( Upp::K_CTRL_DELETE ).Help( Upp::t_( "Пометить слова для удаления" ) );
  bar.Add( Upp::t_( "Редактировать" ), PrabhupadaSlovaryImg::Edit(),     Edit_     ).Key( Upp::K_CTRL_E      ).Help( Upp::t_( "Редактировать" ) );
  bar.Add( Upp::t_( "Поиск" ),         PrabhupadaSlovaryImg::Filter(),   Filter_   ).Key( Upp::K_ENTER       ).Help( Upp::t_( "Применить введенные слова для поиска" ) );
  bar.Add( Upp::t_( "Удалить дубли" ), PrabhupadaSlovaryImg::RemoveDubli(),
           RemoveDubli_ ).Key( Upp::K_ALT | Upp::K_CTRL_D  ).Help( Upp::t_( "Удалить дубли" ) );
  bar.Add( Upp::t_( "Копировать в буфер обмена" ), PrabhupadaSlovaryImg::CopyToClipboard(), CopyToClipboard_ ).Key( Upp::K_CTRL_C ).Help( Upp::t_( "Копировать в буфер обмена" ) );
  bar.Add( Upp::t_( "Удалить помеченные слова" ), PrabhupadaSlovaryImg::DeleteSlova(), DeleteSlova_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_DELETE ).Help( Upp::t_( "Удалить помеченные слова" ) );

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
  if ( Sortirovka != s || Sortirovka == VidSortirovka::Reset ) {
    Sortirovka = s;

    switch ( Sortirovka ) {
    case VidSortirovka::Reset :
			return;
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
}

void PrabhupadaSlovaryPanel::SmenaYazyka()
{
  SetYazyk( YazykDropList.GetIndex() );
}

inline bool operator < ( const SanskritPair& a, const SanskritPair& b )
{
  return a.Sanskrit == b.Sanskrit ? a.Perevod  <  b.Perevod : a.Sanskrit <  b.Sanskrit;
}

inline bool operator > ( const SanskritPair& a, const SanskritPair& b )
{
  return a.Sanskrit == b.Sanskrit ? a.Perevod  >  b.Perevod : a.Sanskrit >  b.Sanskrit;
}

void PrabhupadaSlovaryPanel::RemoveDubli()
{
  // Сортируем по простому без выкрутасов!
  Upp::Sort( VectorSanskrit, std::less< SanskritPair >() );
  Upp::RemoveDubli< SanskritVector >( VectorSanskrit, [&] ( int i ) { DeleteSlovo( i ); } );

  BigRefresh();
}

void PrabhupadaSlovaryPanel::DeleteSlovo( int i )
{
  Upp::Sql sql( Session );
  sql.SetStatement( "delete from SANSKRIT where ID = ?" );
  sql.SetParam( 0, VectorSanskrit[ VectorSanskrit[ i ].Index ].ID );
  DLOG( Upp::AsString( i ) + " : " + Upp::AsString( VectorSanskrit[ i ].Index ) + " : " + VectorSanskrit[ VectorSanskrit[ i ].Index ].Sanskrit + " : " + VectorSanskrit[ VectorSanskrit[ i ].Index ].Perevod );
  sql.Run();
  sql.Clear();
}

void PrabhupadaSlovaryPanel::AddSlovo()
{
  Upp::Vector< int > v = { 1, 1, 2, 3, 3, 3, 4, 5, 6, 6, 7, 8, 9, 10, 10 };
  DDUMP( v );
  Upp::RemoveDubli< Upp::Vector< int > >( v );
  DDUMP( v );
  Upp::PromptOK( "AddSlovo()!" );
}

//void ArrayCtrlGetSelIndexes( Upp::ArrayCtrl& A, Upp::Vector< int >& R )
//{
//	for( int i = 0; i < A.GetCount(); ++i )
//		if( A.IsSel( i ) )
//			R.Add( i );
//}

void PrabhupadaSlovaryPanel::RemoveSlovo()
{
  if ( Upp::PromptOKCancel( Upp::t_( "Удалять слово?" ) ) ) {
    Upp::Vector< int > rs;
    ArraySanskrit.GetSelIndexes( rs );
    for ( int i = 0; i < rs.GetCount(); ++i )
      VectorSanskrit[ VectorSanskrit[ rs[ i ] ].Index ].DeleteCandidat = !VectorSanskrit[ VectorSanskrit[ rs[ i ] ].Index ].DeleteCandidat;
 }
}

void PrabhupadaSlovaryPanel::DeleteSlova()
{
  Upp::Sql sql( Session );
  sql.SetStatement( "delete from SANSKRIT where ID = ?" );
  if ( Upp::PromptOKCancel( Upp::t_( "Удалять помеченные слова?" ) ) ) {
    bool UdalenieBylo = false;
    for ( int i = 0; i < VectorSanskrit.GetCount(); ++i ) {
      SanskritPair& sp = VectorSanskrit[ i ];
      if ( sp.DeleteCandidat ) {
        sql.SetParam( 0, sp.ID );
        DLOG( Upp::AsString( sp.ID ) + " : " + sp.Sanskrit + " : " + sp.Perevod );
        sql.Run();
        UdalenieBylo = true;
      }
    }
    if ( UdalenieBylo ) {
      VectorSanskrit.RemoveIf( [&] ( int i ) { return VectorSanskrit[ i ].DeleteCandidat; } );
      BigRefresh();
    }
  }
}

void PrabhupadaSlovaryPanel::BigRefresh()
{
  VidSortirovka Sortirovka_ = Sortirovka;
  Sortirovka = VidSortirovka::Reset;
  Filter.Reset = true;

  VectorSanskrit.ResetIndex();

  SetFilter( Filter );
  SetSortirovka( Sortirovka_ );
}

void PrabhupadaSlovaryPanel::Edit()
{
  Upp::PromptOK( "Редактирование!" );
}

PrabhupadaSlovaryWindow::PrabhupadaSlovaryWindow( CommandMap& cm )
{
  Add( PanelPrabhupadaSlovary );
  PanelPrabhupadaSlovary.SizePos();
  Upp::Rect( 0, 0, 600, 600 );
  Title( Upp::t_( "Санскритский словарь Шрилы Прабхупады" ) ).Sizeable().Zoomable();

  Upp::Function< void( Upp::Stream& ) > Serial;
  Serial = [&] ( Upp::Stream& S ) { Serialize( S ); };
  
  int i, &StrongYazyk = PanelPrabhupadaSlovary.StrongYazyk;
  PrabhupadaSlovaryPanel::VidSortirovka InitSortirovka = PrabhupadaSlovaryPanel::VidSortirovka::Reset;
  
  CommandInfo& ci = cm.GetPut( "Lang" );
  if ( ci.Present ) {
    i = PanelPrabhupadaSlovary.VectorYazyk.FindYazyk( ci.Value );
    StrongYazyk = i > 0 ? PanelPrabhupadaSlovary.VectorYazyk[ i ].ID : RussianYazyk;
  }
  
  ci = cm.GetPut( "NoLoadINI" );
  bool LoadINI = false;
  if ( !ci.Present )
    LoadINI = Upp::LoadFromFile( Serial, PrabhupadaSlovaryIniFile, 0 );

  if ( !LoadINI ) {
    if ( StrongYazyk == -1 )
      StrongYazyk = RussianYazyk;
    InitSortirovka = PrabhupadaSlovaryPanel::VidSortirovka::SanskritVozrastanie;
  }

  if ( StrongYazyk != -1 )
    PanelPrabhupadaSlovary.SetYazyk( StrongYazyk );
  if ( InitSortirovka != PrabhupadaSlovaryPanel::VidSortirovka::Reset )
    PanelPrabhupadaSlovary.SetSortirovka( InitSortirovka );
  
  FilterSlovary F = PanelPrabhupadaSlovary.Filter;
  PanelPrabhupadaSlovary.Filter.Clear();
  PanelPrabhupadaSlovary.SetFilter( F );
}

PrabhupadaSlovaryWindow::~PrabhupadaSlovaryWindow()
{
  Upp::Function< void( Upp::Stream& ) > Serial;
  Serial = [&] ( Upp::Stream& S ) { Serialize( S ); };
  Upp::StoreToFile(  Serial, Prabhupada::PrabhupadaSlovaryIniFile, 0 );
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
    (*this)[ i ].ReservIndex = (*this)[ i ].Index;
}

void SanskritVector::LoadIndexFromReserv()
{
  for ( int i = 0; i < GetCount(); ++i )
    (*this)[ i ].Index = (*this)[ i ].ReservIndex;
}

void SanskritVector::ResetIndex()
{
  for ( int i = 0; i < GetCount(); ++i ) {
    (*this)[ i ].Index       = i;
    (*this)[ i ].ReservIndex = i;
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
    Filter.Reset = true;
    VidSortirovka Sortirovka_ = Sortirovka;
    Sortirovka = VidSortirovka::Reset;
    
    int c = ArraySanskrit.GetCursor();
    Yazyk = y;
    PrepareVectorSanskrit();
    YazykDropList.SetIndex( y );

    SetSortirovka( Sortirovka_ );
    SetFilter( Filter );

    if ( c < 0 )
      ArraySanskrit.GoBegin();
    else
      ArraySanskrit.SetCursor( c );
  }
}

} // namespace Prabhupada
