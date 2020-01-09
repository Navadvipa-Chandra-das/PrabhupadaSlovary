#include "PrabhupadaSlovary.h"
#include "PrabhupadaSlovaryConsts.h"

#define IMAGECLASS PrabhupadaSlovaryImg
#define IMAGEFILE <PrabhupadaSlovary/PrabhupadaSlovary.iml>
//#include <Draw/iml_source.h>
#include <Draw/iml.h>
#include <plugin/pcre/Pcre.h>
#include <set>

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

Upp::Value PrabhupadaSlovaryPanel::GetSanskrit(const Upp::Value& V )
{
  int n = V, i;
  Upp::AttrText at;
  if ( n < VectorSanskrit.FilterGetCount ) {
    i  = VectorIndex[ n ].Index;
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

Upp::Value PrabhupadaSlovaryPanel::GetPerevod(const Upp::Value& V )
{
  int n = V, i;
  Upp::AttrText at;
  if ( n < VectorSanskrit.FilterGetCount ) {
    i  = VectorIndex[ n ].Index;
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

void PrabhupadaSlovaryPanel::SetSanskrit(const Upp::Value& V, int i )
{
  Upp::String s = V;
  int n = VectorIndex[ i ].Index;
  VectorSanskrit[ n ].Sanskrit = s;
  Upp::Sql sql( Session );
  sql.SetStatement( "update SANSKRIT set IZNACHALYNO = ? where ID = ?" );
  sql.SetParam( 0, s );
  sql.SetParam( 1, VectorSanskrit[ n ].ID );
  sql.Run();
  sql.Clear();
}

void PrabhupadaSlovaryPanel::SetPerevod(const Upp::Value& V, int i )
{
  Upp::String s = V;
  int n = VectorIndex[ i ].Index;
  VectorSanskrit[ n ].Perevod = s;
  Upp::Sql sql( Session );
  sql.SetStatement( "update SANSKRIT set PEREVOD = ? where ID = ?" );
  sql.SetParam( 0, s );
  sql.SetParam( 1, VectorSanskrit[ n ].ID );
  sql.Run();
  sql.Clear();
}

void PrabhupadaSlovaryPanel::ArraySanskritInserter( int I )
{
  ++VectorSanskrit.FilterGetCount;

  SanskritPair PairSanskrit;
  IndexPair    PairIndex;
  
  SanskritPair& RefPairSanskrit = VectorSanskrit.Insert( I, PairSanskrit );
  IndexPair& RefPairIndex       = VectorIndex.Insert( I, PairIndex );
  
  DDUMP( I );
  
  RefPairIndex.Index          = I;
  RefPairIndex.ReservIndex    = RefPairIndex.Index;

  RefPairSanskrit.ID          = ++VectorSanskrit.LastID;
  RefPairSanskrit.Sanskrit    = "Санскрит";
  RefPairSanskrit.Perevod     = "Перевод";
  
  Upp::Sql sql( Session );
  sql.SetStatement( "insert into SANSKRIT ( ID, IZNACHALYNO, PEREVOD, YAZYK ) values ( ?, ?, ?, ? )" );
  sql.SetParam( 0, RefPairSanskrit.ID );
  sql.SetParam( 1, RefPairSanskrit.Sanskrit );
  sql.SetParam( 2, RefPairSanskrit.Perevod );
  sql.SetParam( 3, VectorYazyk[ Yazyk ].Yazyk );
  sql.Run();
  sql.Clear();
  
  int L = VectorIndex.GetCount();
  for ( int i = 0; i < L; ++i ) {
    if ( i != I ) {
      if ( VectorIndex[ i ].Index >= I )
        VectorIndex[ i ].Index = VectorIndex[ i ].Index + 1;
      if ( VectorIndex[ i ].ReservIndex >= I )
        VectorIndex[ i ].ReservIndex = VectorIndex[ i ].ReservIndex + 1;
    }
  }
  ArraySanskritRefresh();
}

PrabhupadaSlovaryPanel::PrabhupadaSlovaryPanel()
{
  CtrlLayout( *this );
  SanskritFilterEdit.Tip( Upp::t_( "Поиск санскрита" ) );
  PerevodFilterEdit.Tip(  Upp::t_( "Поиск перевода" ) );
  SplitterSearch.Horz() << SanskritFilterEdit << PerevodFilterEdit;
  SplitterSearch.SetMin( 0, 1000 );
  SplitterSearch.SetMin( 1, 1000 );
  EditIndicatorRow.NoWantFocus();
  
  PrepareBukvaryPrabhupada();

  ArraySanskrit.AddRowNumColumn( Upp::t_( "Санскрит" ), 50 ).ConvertBy( [&] ( const Upp::Value& V ) { return GetSanskrit( V ); } ).SetSetter( [&] ( const Upp::Value& V, int i ) { SetSanskrit( V, i ); } ).Edit( EditSanskrit );
  ArraySanskrit.AddRowNumColumn( Upp::t_( "Перевод"  ), 50 ).ConvertBy( [&] ( const Upp::Value& V ) { return GetPerevod(  V ); } ).SetSetter( [&] ( const Upp::Value& V, int i ) { SetPerevod(  V, i ); } ).Edit( EditPerevod  );
  const Upp::Font& gf = GetGauraFont();
  GauraFont = gf;
  ArraySanskrit.SetLineCy( gf.GetCy() );
  ArraySanskrit.MultiSelect();
  ArraySanskrit.Inserting();

  Upp::Function< void ( int i ) > ins;
  ins = [&] ( int i ) { ArraySanskritInserter( i ); };
  ArraySanskrit.Inserter << ins;
  
  SanskritFilterEdit.SetFont( GauraFont );
  PerevodFilterEdit.SetFont( GauraFont );

  EditSanskrit.SetFont( GauraFont );
  EditPerevod.SetFont( GauraFont );

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
  VectorSanskrit.LastID = GetVectorSanskritLastID();
}

void PrabhupadaSlovaryPanel::PrepareBukvaryPrabhupada()
{
	Upp::FileIn F( Upp::ConfigFile( "PrabhupadaBukvary.txt" ) );
	Upp::WString W;
	while ( true ) {
		Upp::String S = F.GetLine();
		if ( S.IsVoid() )
		  break;
	  
	  W = S.ToWString();
	  BukvaryPrabhupada[ W[ 0 ] ] = W.GetCount() == 3 ? W[ 2 ] : W[ 0 ];
	}
}

void PrabhupadaSlovaryPanel::IndicatorRow()
{
  int r = ArraySanskrit.GetCursor() + 1;
  EditIndicatorRow.SetText( Upp::AsString( r ) + IndicatorSeparator + Upp::AsString( VectorSanskrit.FilterGetCount ) );
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
  VectorIndex.Clear();

  int i = 0;
  SanskritPair PairSanskrit;
  IndexPair    PairIndex;
  
  while ( sql.Fetch() ) {
    ID          = sql[ 0 ];
    IZNACHALYNO = sql[ 1 ];
    PEREVOD     = sql[ 2 ];
  
    SanskritPair& RefPairSanskrit = VectorSanskrit.At( i, PairSanskrit );
    IndexPair&    RefPairIndex    = VectorIndex.At( i, PairIndex );
  
    RefPairIndex.Index          = i++;
    RefPairIndex.ReservIndex    = RefPairIndex.Index;

    RefPairSanskrit.ID          = ID;
    RefPairSanskrit.Sanskrit    = IZNACHALYNO;
    RefPairSanskrit.Perevod     = PEREVOD;
    
    RemoveDiacritics( RefPairSanskrit.SanskritWD, RefPairSanskrit.Sanskrit );
    RemoveDiacritics( RefPairSanskrit.PerevodWD,  RefPairSanskrit.Perevod );
  }
  sql.Clear();
  SetVectorSanskritFilterGetCount( i );
}

void PrabhupadaSlovaryPanel::RemoveDiacritics( Upp::String& R, const Upp::String& S )
{
  Upp::WString W = S.ToWString(), WR;
  
  int L = W.GetCount();
  for ( int i = 0; i < L; ++i )
    WR.Cat( BukvaryPrabhupada[ W[ i ] ] );
  R = WR.ToString();
}

int PrabhupadaSlovaryPanel::GetVectorSanskritLastID()
{
  Upp::Sql sql( Session );
  sql.SetStatement( "select max( a.ID ) from SANSKRIT a" );
  sql.Run();
  sql.Fetch();
  int R = sql[ 0 ];
  return R;
}

void PrabhupadaSlovaryPanel::SetVectorSanskritFilterGetCount( int d )
{
  VectorSanskrit.FilterGetCount = d;
  ArraySanskrit.Clear();
  ArraySanskrit.SetVirtualCount( VectorSanskrit.FilterGetCount );
  ArraySanskrit.WhenSel();
}

AboutPrabhupadaSlovaryWindow::AboutPrabhupadaSlovaryWindow()
{
  Upp::CtrlLayout( *this );
  Title( Upp::t_( "О программе и помощь!" ) );
  RichTextAbout.Pick( Upp::ParseQTF( Upp::LoadFile( Upp::ConfigFile( "AboutPrabhupadaSlovary.qtf" ) ) )  );
}

void AboutPrabhupadaSlovaryWindow::Paint( Upp::Draw& draw )
{
  draw.DrawRect( GetSize(), Upp::SColorFace() );
  draw.DrawImage( 0, 0, PrabhupadaSlovaryImg::GerbNizhneyNavadvipy() );
}

void PrabhupadaSlovaryPanel::AboutPrabhupadaSlovary()
{
  AboutPrabhupadaSlovaryWindow WindowAboutPrabhupadaSlovary;
  WindowAboutPrabhupadaSlovary.Run();
}

void PrabhupadaSlovaryPanel::PrabhupadaBukvary()
{
  Upp::String S;
  Upp::WString W;
  
  static std::set< int > Set;
  int L = VectorSanskrit.GetCount();
  if ( Upp::PromptOKCancel( Upp::t_( "Собрать полный набор букв?" ) ) ) {
    int SL;
    for ( int i = 0; i < L; ++i ) {
      SanskritPair& sp = VectorSanskrit[ i ];
      S = sp.Sanskrit + sp.Perevod;
      W = S.ToWString();
      SL = W.GetCount();
      for ( int j = 0; j < SL; ++j ) {
        Set.insert( W[ j ] );
      }
    }
  }
  W.Clear();
  for ( std::set< int >::iterator i = Set.begin(); i != Set.end(); ++i ) {
    W.Cat( (*i) );
    W += "\n";
  }
  Upp::BeepInformation();
  Upp::AppendClipboardText( W.ToString() );
}

void PrabhupadaSlovaryPanel::PrepareBar( Upp::Bar& bar )
{
  Upp::Event<> AddSlovo_; // Gate<> для функторов, возвращающих логический тип
  Upp::Function< void( void ) > MarkDeleteSlovo_, Edit_, SmenaYazyka_, SortirovkaUstanovka_, Filter_, RemoveDuplicates_, CopyToClipboard_, DeleteSlova_, AboutPrabhupadaSlovary_,
                                PrabhupadaBukvary_;

  AddSlovo_          =  [&] { AddSlovo(); };
  MarkDeleteSlovo_   << [&] { MarkDeleteSlovo(); };
  Edit_              << [&] { Edit(); };
  Filter_            << [&] { FilterUstanovka(); };
  RemoveDuplicates_  << [&] { RemoveDuplicates(); };
  CopyToClipboard_   << [&] { CopyToClipboard(); };
  DeleteSlova_       << [&] { DeleteSlova(); };
  AboutPrabhupadaSlovary_ << [&] { AboutPrabhupadaSlovary(); };
  PrabhupadaBukvary_ =  [&] { PrabhupadaBukvary(); };
  
  bar.Add( Upp::t_( "Добавить" ), PrabhupadaSlovaryImg::AddSlovo(), AddSlovo_ ).Key( Upp::K_CTRL_INSERT ).Help( Upp::t_( "Добавить новое слово и перевод" ) );
  bar.Add( Upp::t_( "Пометить для удаления" ), PrabhupadaSlovaryImg::MarkDeleteSlovo(), MarkDeleteSlovo_ ).Key( Upp::K_CTRL_DELETE ).Help( Upp::t_( "Пометить слова для удаления" ) );
  bar.Add( Upp::t_( "Редактировать" ), PrabhupadaSlovaryImg::Edit(), Edit_ ).Key( Upp::K_CTRL_E ).Help( Upp::t_( "Редактировать" ) );
  bar.Add( Upp::t_( "Поиск" ), PrabhupadaSlovaryImg::Filter(), Filter_ ).Key( Upp::K_ENTER ).Help( Upp::t_( "Применить введенные слова для поиска" ) );
  bar.Add( Upp::t_( "Удалить дубли" ), PrabhupadaSlovaryImg::RemoveDuplicates(), RemoveDuplicates_ ).Key( Upp::K_ALT | Upp::K_CTRL_D  ).Help( Upp::t_( "Удалить дубли" ) );
  bar.Add( Upp::t_( "Копировать в буфер обмена" ), PrabhupadaSlovaryImg::CopyToClipboard(), CopyToClipboard_ ).Key( Upp::K_CTRL_C ).Help( Upp::t_( "Копировать в буфер обмена" ) );
  bar.Add( Upp::t_( "Удалить помеченные слова" ), PrabhupadaSlovaryImg::DeleteSlova(), DeleteSlova_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_DELETE ).Help( Upp::t_( "Удалить помеченные слова" ) );
  bar.Add( Upp::t_( "О программе" ), PrabhupadaSlovaryImg::Tilaka(), AboutPrabhupadaSlovary_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_A ).Help( Upp::t_( "О программе \"Словарь Шрилы Прабхупады\"" ) );
  bar.Add( Upp::t_( "Собрать набор букв в/nбуфер обмена" ), PrabhupadaSlovaryImg::PrabhupadaBukvary(), PrabhupadaBukvary_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_B ).Help( Upp::t_( "Собрать набор букв в буфер обмена" ) );

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
  Upp::Vector< int > V;
  ArraySanskrit.GetSelIndexes( V );
  Upp::String S;
  int L = V.GetCount();
  for ( int i = 0; i < L; ++i ) {
    S = S + VectorSanskrit[ VectorIndex[ V[ i ] ].Index ].Sanskrit + "\t" +
            VectorSanskrit[ VectorIndex[ V[ i ] ].Index ].Perevod  + "\n";
  }
  Upp::AppendClipboardText( S );
}

void PrabhupadaSlovaryPanel::FilterUstanovka()
{
  FilterSlovary F;
  F.FilterSanskrit = ~SanskritFilterEdit;
  F.FilterPerevod  = ~PerevodFilterEdit;
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
      VectorIndex.ResetIndex();
      break;
    case VidSortirovka::SanskritVozrastanie :
      Upp::Sort( VectorIndex
               , [&] ( IndexPair& a, IndexPair& b ) { return VectorSanskrit[ a.Index ].Sanskrit == VectorSanskrit[ b.Index ].Sanskrit ?
                                                             VectorSanskrit[ a.Index ].Perevod  <  VectorSanskrit[ b.Index ].Perevod :
                                                             VectorSanskrit[ a.Index ].Sanskrit <  VectorSanskrit[ b.Index ].Sanskrit; } );
      break;
    case VidSortirovka::SanskritUbyvanie :
      Upp::Sort( VectorIndex
               , [&] ( IndexPair& a, IndexPair& b ) { return VectorSanskrit[ a.Index ].Sanskrit == VectorSanskrit[ b.Index ].Sanskrit ?
                                                             VectorSanskrit[ a.Index ].Perevod  >  VectorSanskrit[ b.Index ].Perevod :
                                                             VectorSanskrit[ a.Index ].Sanskrit >  VectorSanskrit[ b.Index ].Sanskrit; } );
      break;
    case VidSortirovka::PerevodVozrastanie :
      Upp::Sort( VectorIndex
               , [&] ( IndexPair& a, IndexPair& b ) { return VectorSanskrit[ a.Index ].Perevod  == VectorSanskrit[ b.Index ].Perevod ?
                                                             VectorSanskrit[ a.Index ].Sanskrit <  VectorSanskrit[ b.Index ].Sanskrit :
                                                             VectorSanskrit[ a.Index ].Perevod  <  VectorSanskrit[ b.Index ].Perevod; } );
      break;
    case VidSortirovka::PerevodUbyvanie :
      Upp::Sort( VectorIndex
               , [&] ( IndexPair& a, IndexPair& b ) { return VectorSanskrit[ a.Index ].Perevod  == VectorSanskrit[ b.Index ].Perevod ?
                                                             VectorSanskrit[ a.Index ].Sanskrit >  VectorSanskrit[ b.Index ].Sanskrit :
                                                             VectorSanskrit[ a.Index ].Perevod  >  VectorSanskrit[ b.Index ].Perevod; } );
      break;
    }
    SortDropList.SetIndex( static_cast< int >( s ) );
    VectorIndex.SaveIndexToReserv();
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

void PrabhupadaSlovaryPanel::RemoveDuplicates()
{
  Upp::Sql sql( Session );
  sql.SetStatement( "delete from SANSKRIT where ID = ?" );
  // Сортируем по простому без выкрутасов!
  Upp::Sort( VectorSanskrit, std::less< SanskritPair >() );
  bool NeedBigRefresh = false;
  Upp::RemoveDuplicates< SanskritVector >( VectorSanskrit,
    [&] ( int i ) {
                    sql.SetParam( 0, VectorSanskrit[ VectorIndex[ i ].Index ].ID );
                    sql.Run();
                    VectorIndex.Remove( i );
                    NeedBigRefresh = true;
                  } );

  if ( NeedBigRefresh )
    BigRefresh();
}

void PrabhupadaSlovaryPanel::AddSlovo()
{
  ArraySanskrit.DoInsertBefore();
}

void PrabhupadaSlovaryPanel::MarkDeleteSlovo()
{
  if ( Upp::PromptOKCancel( Upp::t_( "Удалять слово?" ) ) ) {
    Upp::Vector< int > rs;
    ArraySanskrit.GetSelIndexes( rs );
    int L = rs.GetCount();
    for ( int i = 0; i < L; ++i )
      VectorSanskrit[ VectorIndex[ rs[ i ] ].Index ].DeleteCandidat = !VectorSanskrit[ VectorIndex[ rs[ i ] ].Index ].DeleteCandidat;
 }
}

void PrabhupadaSlovaryPanel::DeleteSlova()
{
  Upp::Sql sql( Session );
  sql.SetStatement( "delete from SANSKRIT where ID = ?" );
  if ( Upp::PromptOKCancel( Upp::t_( "Удалять помеченные слова?" ) ) ) {
    bool NeedBigRefresh = false;
    VectorSanskrit.RemoveIf( [&] ( int i ) { return VectorSanskrit[ i ].DeleteCandidat; },
                             [&] ( int i ) {
                                             SanskritPair& sp = VectorSanskrit[ i ];
                                             sql.SetParam( 0, sp.ID );
                                             sql.Run();
                                             VectorIndex.Remove( i );
                                             NeedBigRefresh = true;
                                           } );
    if ( NeedBigRefresh )
      BigRefresh();
  }
}

void PrabhupadaSlovaryPanel::BigRefresh()
{
  VidSortirovka Sortirovka_ = Sortirovka;
  Sortirovka = VidSortirovka::Reset;
  Filter.Reset = true;

  VectorIndex.ResetIndex();

  SetSortirovka( Sortirovka_ );
  SetFilter( Filter );
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
  Title( Upp::t_( "Санскритский словарь Шрилы Прабхупады!" ) ).Sizeable().Zoomable();

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

void IndexVector::SaveIndexToReserv()
{
  int L = GetCount();
  for ( int i = 0; i < L; ++i )
    (*this)[ i ].ReservIndex = (*this)[ i ].Index;
}

void IndexVector::LoadIndexFromReserv()
{
  int L = GetCount();
  for ( int i = 0; i < L; ++i )
    (*this)[ i ].Index = (*this)[ i ].ReservIndex;
}

void IndexVector::ResetIndex()
{
  int L = GetCount();
  for ( int i = 0; i < L; ++i ) {
    (*this)[ i ].Index       = i;
    (*this)[ i ].ReservIndex = i;
  }
}

void PrabhupadaSlovaryPanel::FilterVectorSanskrit()
{
  int ActualIndex = -1, n;

  Upp::String Sanskrit = ~SanskritFilterEdit,
              Perevod  = ~PerevodFilterEdit;
  bool CheckSanskrit = !Sanskrit.IsEmpty(),
       CheckPerevod  = !Perevod.IsEmpty();

  Upp::RegExp RegSanskrit( Sanskrit, Upp::RegExp::UNICODE ),
              RegPerevod(  Perevod,  Upp::RegExp::UNICODE );

  bool NeedSanskrit, NeedPerevod;
  
  int L = VectorSanskrit.GetCount();
  for ( int i = 0; i < L; ++i ) {
    n = VectorIndex[ i ].ReservIndex;
    NeedPerevod = false;
    NeedSanskrit = CheckSanskrit ? RegSanskrit.Match( VectorSanskrit[ n ].SanskritWD ) : true;
    if ( NeedSanskrit )
      NeedPerevod  = CheckPerevod  ? RegPerevod.Match(  VectorSanskrit[ n ].PerevodWD )  : true;
    if ( NeedPerevod )
      VectorIndex[ ++ActualIndex ].Index = n;
  }

  SetVectorSanskritFilterGetCount( ++ActualIndex );
}

void PrabhupadaSlovaryPanel::SetFilter( const FilterSlovary& F )
{
  if ( Filter != F || Filter.Reset == true  ) {
    Filter.Reset = false;

    IfEditOKCancel();

    Filter = F;
    if ( Filter.IsEmpty() ) {
      VectorIndex.LoadIndexFromReserv();
      SetVectorSanskritFilterGetCount( VectorSanskrit.GetCount() );
    } else
      FilterVectorSanskrit();
    
    SanskritFilterEdit.SetText( Filter.FilterSanskrit );
    PerevodFilterEdit.SetText( Filter.FilterPerevod );
  }
}

int YazykVector::FindYazyk( const Upp::String& S )
{
	for( int i = 0; i < GetCount(); ++i )
		if ( (*this)[i].Yazyk == S )
		  return i;
	return -1;
}

void PrabhupadaSlovaryPanel::IfEditOKCancel()
{
  if ( ArraySanskrit.IsEdit() || ArraySanskrit.IsInsert() || ArraySanskrit.IsAppendLine() ) {
    bool Post_ = false;
    if( EditSanskrit.IsModified() || EditPerevod.IsModified() )
      if ( Upp::PromptYesNo( Upp::t_( "Сохранить изменения?" ) ) ) {
        ArraySanskrit.IfEditPost();
        Post_ = true;
      }
    if ( !Post_ )
      ArraySanskrit.IfEditCancel();
  }
}

void PrabhupadaSlovaryPanel::SetYazyk( int y )
{
  if ( Yazyk != y ) {
    Filter.Reset = true;
    VidSortirovka Sortirovka_ = Sortirovka;
    Sortirovka = VidSortirovka::Reset;
    
    IfEditOKCancel();
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
