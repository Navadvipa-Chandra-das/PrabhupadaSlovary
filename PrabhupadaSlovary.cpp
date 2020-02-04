#include "PrabhupadaSlovary.h"
#include "PrabhupadaSlovaryConsts.h"

#define IMAGECLASS PrabhupadaSlovaryImg
#define IMAGEFILE <PrabhupadaSlovary/PrabhupadaSlovary.iml>
//#include <Draw/iml_source.h>
#include <Draw/iml.h>
#include <plugin/pcre/Pcre.h>
#include <set>

namespace Prabhupada {

PrabhupadaBukvary BukvaryPrabhupada;

template <class Condition, class OnRemove>
void SanskritVector::RemoveIf( Condition c, const OnRemove& On_Remove )
{
  int L = GetCount();
  for( int i = L - 1; i >= 0; --i )
    if( c( i ) ) {
      On_Remove( i );
      Remove( i );
    }
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
  
  RefPairIndex.Index          = I;
  RefPairIndex.ReservIndex    = RefPairIndex.Index;

  RefPairSanskrit.ID          = ++VectorSanskrit.LastID;
  RefPairSanskrit.Sanskrit    = Upp::t_( "Sanskrit" );
  RefPairSanskrit.Perevod     = Upp::t_( "Translation" );
  
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
  SanskritFilterEdit.Tip( Upp::t_( "Search for Sanskrit" ) );
  PerevodFilterEdit.Tip(  Upp::t_( "Search for translation" ) );
  SplitterSearch.Horz() << SanskritFilterEdit << PerevodFilterEdit;
  SplitterSearch.SetMin( 0, 1000 );
  SplitterSearch.SetMin( 1, 1000 );
  
  PrepareBukvaryPrabhupada();

  ArraySanskrit.AddRowNumColumn( Upp::t_( "Sanskrit" ),    50 ).ConvertBy( [&] ( const Upp::Value& V ) { return GetSanskrit( V ); } ).SetSetter( [&] ( const Upp::Value& V, int i ) { SetSanskrit( V, i ); } ).Edit( EditSanskrit );
  ArraySanskrit.AddRowNumColumn( Upp::t_( "Translation" ), 50 ).ConvertBy( [&] ( const Upp::Value& V ) { return GetPerevod(  V ); } ).SetSetter( [&] ( const Upp::Value& V, int i ) { SetPerevod(  V, i ); } ).Edit( EditPerevod  );

  GauraFont.FaceName( "Gaura Times" );

  ArraySanskrit.MultiSelect();
  ArraySanskrit.Inserting();

  Upp::Function< void ( int i ) > ins;
  ins = [&] ( int i ) { ArraySanskritInserter( i ); };
  ArraySanskrit.Inserter << ins;
  
  Upp::Function< void () > ur;
  ur = [&] () { IndicatorRow(); };
  ArraySanskrit.WhenSel << ur;
  
  ToolBarSanskrit.ButtonMinSize( Upp::Size( 24, 24 ) );
  PrepareBar( ToolBarSanskrit );
  PrepareBar( PrabhupadaMenu );
       
  Upp::String db( Upp::ConfigFile( "Sanskrit.db" ) );
  if( !Session.Open( db ) ) {
    Upp::PromptOK( Upp::t_( "Shrila Prabhupada's Sanskrit dictionary database could not be opened!" ) );
  } else {
    #ifdef DEBUG
      Session.LogErrors( true );
    #endif
  }
  PrepareVectorYazyk();
  VectorSanskrit.LastID = GetVectorSanskritLastID();
}

void PrabhupadaSlovaryPanel::PrepareBukvaryPrabhupada()
{
  Upp::FileIn F( Upp::ConfigFile( "PrabhupadaBukvary.txt" ) );
  Upp::WString W;
  PrabhupadaBukva B;
  Upp::ConvertInt CV;
  int V = 0, L;

  Upp::String S;
  while ( true ) {
    S = F.GetLine();
    if ( S.IsVoid() )
      break;
    
    W = S.ToWString();
    
    L = W.GetCount();

    if ( L >= 3 ) {
      B.Bukva = W[ 2 ];
      B.Ves = ++V;
      BukvaryPrabhupada[ W[ 0 ] ] = B;
    } else
      throw "Not correct letter in Bukvary Prabhupada!";
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
  for ( YazykVector::iterator i = VectorYazyk.begin(); i != VectorYazyk.end(); ++i )
    YazykDropList.Add( (*i).YazykSlovo );
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
    
    RefPairSanskrit.SanskritWD = RemoveDiacritics( RefPairSanskrit.Sanskrit );
    RefPairSanskrit.PerevodWD  = RemoveDiacritics( RefPairSanskrit.Perevod );
  }
  sql.Clear();
  SetVectorSanskritFilterGetCount( i );
}

Upp::String PrabhupadaSlovaryPanel::RemoveDiacritics( const Upp::String& S )
{
  Upp::WString W = S.ToWString(), WR;
  
  int L = W.GetCount();
  for ( int i = 0; i < L; ++i )
    WR.Cat( BukvaryPrabhupada[ W[ i ] ].Bukva );
  return WR.ToString();
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
  int C = ArraySanskrit.GetCursor();
  VectorSanskrit.FilterGetCount = d;
  ArraySanskrit.Clear();
  ArraySanskrit.SetVirtualCount( VectorSanskrit.FilterGetCount );
  ArraySanskrit.SetCursor( C );
  ArraySanskrit.WhenSel();
}

PrabhupadaTabAboutPanel::PrabhupadaTabAboutPanel()
{
  Upp::CtrlLayout( *this );
}

void PrabhupadaTabAboutPanel::Paint( Upp::Draw& draw )
{
  draw.DrawRect( GetSize(), Upp::SColorFace() );
  draw.DrawImage( 0, 0, PrabhupadaSlovaryImg::ShrilaPrabhupada() );
  draw.DrawImage( 488, 644, PrabhupadaSlovaryImg::GerbNizhneyNavadvipy() );
}

PrabhupadaTabLetterPanel::PrabhupadaTabLetterPanel()
{
  Upp::CtrlLayout( *this );

  Upp::String Lang = Upp::GetCurrentLanguageString();
  int L = Lang.Find( ' ' );
  if ( L == -1 )
    L = Lang.Find( '.' );
  L = L == -1 ? Lang.GetCount() : L;
  Lang = Lang.Left( L );
  Upp::String F = Upp::ConfigFile( "Text/AboutPrabhupadaSlovary" + Lang + ".qtf" );
  RichTextAbout.Pick( Upp::ParseQTF( Upp::LoadFile( F ) ) );
}

AboutPrabhupadaSlovaryWindow::AboutPrabhupadaSlovaryWindow()
{
  Upp::CtrlLayout( *this );
  
  PrabhupadaTabCtrl.SizePos();
  
  PrabhupadaTabCtrl.Add( PanelPrabhupadaTabAbout,  Upp::t_( "About the program" ) ).Image( PrabhupadaSlovaryImg::Tilaka() );
  PanelPrabhupadaTabAbout.SizePos();
  PrabhupadaTabCtrl.Add( PanelPrabhupadaTabLetter, Upp::t_( "Letter" ) ).Image( PrabhupadaSlovaryImg::Edit() );
  PanelPrabhupadaTabLetter.SizePos();

  Title( Upp::t_( "About the program and help!" ) );
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
  if ( Upp::PromptOKCancel( Upp::t_( "Collect a full set of letters?" ) ) ) {
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

void PrabhupadaGoToLineDialog::Serialize( Upp::Stream& s )
{
  Upp::GuiLock __;
  int version = 0;
  s / version;
  s.Magic( 134766 );

  SerializePlacement( s );
  EditRowNum.Serialize( s );
}

PrabhupadaGoToLineDialog::PrabhupadaGoToLineDialog( Upp::StringStream& SerialStream_ ) :
  SerialStream( SerialStream_ )
{
  CtrlLayout( *this );
  Title( Upp::Ctrl::GetAppName() );

  Upp::Function< void( void ) > ButtonGoClick_, ButtonCancelClick_;

  ButtonGoClick_     << [&] { ButtonGoClick(); };
  ButtonCancelClick_ << [&] { ButtonCancelClick(); };
  
  ToolBarGoRow.Add( Upp::t_( "Go" ), PrabhupadaSlovaryImg::Ok(), ButtonGoClick_ ).Key( Upp::K_ENTER );
  ToolBarGoRow.Add( Upp::t_( "Cancel" ), PrabhupadaSlovaryImg::DeleteSlova(), ButtonCancelClick_ ).Key( Upp::K_ESCAPE );
  
  if ( SerialStream.GetSize() > 0 ) {
    Upp::Function< void( Upp::Stream& ) > Serial;
    Serial = [&] ( Upp::Stream& S ) { Serialize( S ); };
    //SerialStream.SetLoading();
    SerialStream.Seek( 0 );
    Upp::Load( Serial, SerialStream, 0 );
  }
}

PrabhupadaGoToLineDialog::~PrabhupadaGoToLineDialog()
{
  //SerialStream.SetStoring();
  SerialStream.SetSize( 0 );
  Upp::Function< void( Upp::Stream& ) > Serial;
  Serial = [&] ( Upp::Stream& S ) { Serialize( S ); };
  Upp::Store( Serial, SerialStream, 0 );
}

void PrabhupadaGoToLineDialog::ButtonGoClick()
{
  RowNum = EditRowNum.GetData();
  Close();
}

void PrabhupadaGoToLineDialog::ButtonCancelClick()
{
  RowNum = 0;
  Close();
}

void PrabhupadaSlovaryPanel::PrabhupadaGoToLine()
{
  PrabhupadaGoToLineDialog G( SerialStream );
  G.Run();
  if ( G.RowNum != 0 ) {
    ArraySanskrit.SetCursor( G.RowNum - 1 );
    ArraySanskrit.WhenSel();
  }
}

void PrabhupadaSlovaryPanel::SetBookmark( int N )
{
  BookmarkInfo BI;
  
  BI.RowNum     = ArraySanskrit.GetCursor();
  BI.Filter     = Filter;
  BI.Sortirovka = Sortirovka;
  BI.Yazyk      = Yazyk;
  
  Bookmark[ N ] = BI;
}

void PrabhupadaSlovaryPanel::GoToBookmark( int N )
{
  BookmarkInfo BI = Bookmark[ N ];

  if ( BI.RowNum != -1 ) {
    SetYazyk( BI.Yazyk );
    SetSortirovka( BI.Sortirovka );
    SetFilter( BI.Filter );
    
    ArraySanskrit.SetCursor( BI.RowNum );
    ArraySanskrit.WhenSel();
  }
}

void BookmarkVector::ClearBookmark()
{
  int L = GetCount();
  for ( int i = 0; i < L; ++i )
    (*this)[ i ].Clear();
}

void PrabhupadaSlovaryPanel::OperationsAdd( Upp::Bar& bar )
{
  Upp::Event<> AddSlovo_; // Gate<> для функторов, возвращающих логический тип
  Upp::Function< void( void ) > MarkDeleteSlovo_, Edit_, Filter_, RemoveDuplicates_, CopyToClipboard_, DeleteSlova_,
                                PrabhupadaBukvary_, PrabhupadaGoToLine_, AboutPrabhupadaSlovary_;

  AddSlovo_               =  [&] { AddSlovo(); };
  MarkDeleteSlovo_        << [&] { MarkDeleteSlovo(); };
  Edit_                   << [&] { Edit(); };
  Filter_                 << [&] { FilterUstanovka(); };
  RemoveDuplicates_       << [&] { RemoveDuplicatesSanskrit(); };
  CopyToClipboard_        << [&] { CopyToClipboard(); };
  DeleteSlova_            << [&] { DeleteSlova(); };
  PrabhupadaBukvary_      =  [&] { PrabhupadaBukvary(); };
  PrabhupadaGoToLine_     << [&] { PrabhupadaGoToLine(); };
  AboutPrabhupadaSlovary_ << [&] { AboutPrabhupadaSlovary(); };

  bar.Add( Upp::t_( "Add" ), PrabhupadaSlovaryImg::AddSlovo(), AddSlovo_ ).Key( Upp::K_CTRL_INSERT ).Help( Upp::t_( "Add a new word and translation" ) );
  bar.Add( Upp::t_( "Mark for deletion" ), PrabhupadaSlovaryImg::MarkDeleteSlovo(), MarkDeleteSlovo_ ).Key( Upp::K_CTRL_DELETE ).Help( Upp::t_( "Mark words for deletion" ) );
  bar.Add( Upp::t_( "Edit" ), PrabhupadaSlovaryImg::Edit(), Edit_ ).Key( Upp::K_F2 ).Help( Upp::t_( "Edit" ) );
  bar.Add( Upp::t_( "Search" ), PrabhupadaSlovaryImg::Filter(), Filter_ ).Key( Upp::K_ENTER ).Help( Upp::t_( "Apply the entered words to the search" ) );
  bar.Add( Upp::t_( "To remove duplicates" ), PrabhupadaSlovaryImg::RemoveDuplicatesSanskrit(), RemoveDuplicates_ ).Key( Upp::K_ALT | Upp::K_CTRL_D  ).Help( Upp::t_( "To remove duplicates" ) );
  bar.Add( Upp::t_( "Copy to clipboard" ), PrabhupadaSlovaryImg::CopyToClipboard(), CopyToClipboard_ ).Key( Upp::K_CTRL_C ).Help( Upp::t_( "Copy to clipboard" ) );
  bar.Add( Upp::t_( "Delete marked words" ), PrabhupadaSlovaryImg::DeleteSlova(), DeleteSlova_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_DELETE ).Help( Upp::t_( "Delete marked words" ) );
  bar.Add( Upp::t_( "Collect a set of letters\nto the clipboard" ), PrabhupadaSlovaryImg::PrabhupadaBukvary(), PrabhupadaBukvary_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_B ).Help( Upp::t_( "Collect a set of letters to the clipboard" ) );
  bar.Add( Upp::t_( "Go to the row of number" ), PrabhupadaSlovaryImg::PrabhupadaGoToLine(), PrabhupadaGoToLine_ ).Key( Upp::K_CTRL_L ).Help( Upp::t_( "Go to the row of number" ) );
  bar.Add( Upp::t_( "About the program" ), PrabhupadaSlovaryImg::Tilaka(), AboutPrabhupadaSlovary_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_A ).Help( Upp::t_( "Sobre o programa \"Dicionário de Shrila Prabhupada\"" ) );
}

void PrabhupadaSlovaryPanel::SetBookmarkAdd( Upp::Bar& bar )
{
  Upp::Function< void() > SetBookmark0_, SetBookmark1_, SetBookmark2_, SetBookmark3_, SetBookmark4_, SetBookmark5_, SetBookmark6_, SetBookmark7_, SetBookmark8_, SetBookmark9_;

  SetBookmark0_ =  [&] { SetBookmark( 0 ); };
  SetBookmark1_ =  [&] { SetBookmark( 1 ); };
  SetBookmark2_ =  [&] { SetBookmark( 2 ); };
  SetBookmark3_ =  [&] { SetBookmark( 3 ); };
  SetBookmark4_ =  [&] { SetBookmark( 4 ); };
  SetBookmark5_ =  [&] { SetBookmark( 5 ); };
  SetBookmark6_ =  [&] { SetBookmark( 6 ); };
  SetBookmark7_ =  [&] { SetBookmark( 7 ); };
  SetBookmark8_ =  [&] { SetBookmark( 8 ); };
  SetBookmark9_ =  [&] { SetBookmark( 9 ); };

  Upp::String SB = Upp::t_( "Set bookmark" ), S;

  S = SB + " &0"; bar.Add( ~S, SetBookmark0_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_0 ).Help( ~S );
  S = SB + " &1"; bar.Add( ~S, SetBookmark1_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_1 ).Help( ~S );
  S = SB + " &2"; bar.Add( ~S, SetBookmark2_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_2 ).Help( ~S );
  S = SB + " &3"; bar.Add( ~S, SetBookmark3_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_3 ).Help( ~S );
  S = SB + " &4"; bar.Add( ~S, SetBookmark4_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_4 ).Help( ~S );
  S = SB + " &5"; bar.Add( ~S, SetBookmark5_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_5 ).Help( ~S );
  S = SB + " &6"; bar.Add( ~S, SetBookmark6_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_6 ).Help( ~S );
  S = SB + " &7"; bar.Add( ~S, SetBookmark7_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_7 ).Help( ~S );
  S = SB + " &8"; bar.Add( ~S, SetBookmark8_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_8 ).Help( ~S );
  S = SB + " &9"; bar.Add( ~S, SetBookmark9_ ).Key( Upp::K_SHIFT | Upp::K_CTRL_9 ).Help( ~S );
}

void PrabhupadaSlovaryPanel::GoToBookmarkAdd( Upp::Bar& bar )
{
  Upp::Function< void() > GoToBookmark0_, GoToBookmark1_, GoToBookmark2_, GoToBookmark3_, GoToBookmark4_, GoToBookmark5_, GoToBookmark6_, GoToBookmark7_, GoToBookmark8_, GoToBookmark9_;

  GoToBookmark0_ =  [&] { GoToBookmark( 0 ); };
  GoToBookmark1_ =  [&] { GoToBookmark( 1 ); };
  GoToBookmark2_ =  [&] { GoToBookmark( 2 ); };
  GoToBookmark3_ =  [&] { GoToBookmark( 3 ); };
  GoToBookmark4_ =  [&] { GoToBookmark( 4 ); };
  GoToBookmark5_ =  [&] { GoToBookmark( 5 ); };
  GoToBookmark6_ =  [&] { GoToBookmark( 6 ); };
  GoToBookmark7_ =  [&] { GoToBookmark( 7 ); };
  GoToBookmark8_ =  [&] { GoToBookmark( 8 ); };
  GoToBookmark9_ =  [&] { GoToBookmark( 9 ); };

  Upp::String GB = Upp::t_( "Go to bookmark" ), S;

  S = GB + " &0"; bar.Add( ~S, GoToBookmark0_ ).Key( Upp::K_CTRL_0 ).Help( ~S );
  S = GB + " &1"; bar.Add( ~S, GoToBookmark1_ ).Key( Upp::K_CTRL_1 ).Help( ~S );
  S = GB + " &2"; bar.Add( ~S, GoToBookmark2_ ).Key( Upp::K_CTRL_2 ).Help( ~S );
  S = GB + " &3"; bar.Add( ~S, GoToBookmark3_ ).Key( Upp::K_CTRL_3 ).Help( ~S );
  S = GB + " &4"; bar.Add( ~S, GoToBookmark4_ ).Key( Upp::K_CTRL_4 ).Help( ~S );
  S = GB + " &5"; bar.Add( ~S, GoToBookmark5_ ).Key( Upp::K_CTRL_5 ).Help( ~S );
  S = GB + " &6"; bar.Add( ~S, GoToBookmark6_ ).Key( Upp::K_CTRL_6 ).Help( ~S );
  S = GB + " &7"; bar.Add( ~S, GoToBookmark7_ ).Key( Upp::K_CTRL_7 ).Help( ~S );
  S = GB + " &8"; bar.Add( ~S, GoToBookmark8_ ).Key( Upp::K_CTRL_8 ).Help( ~S );
  S = GB + " &9"; bar.Add( ~S, GoToBookmark9_ ).Key( Upp::K_CTRL_9 ).Help( ~S );
}

void PrabhupadaSlovaryPanel::PrepareBar( Upp::Bar& bar )
{
  
  if ( bar.IsToolBar() ) {
    OperationsAdd( bar );
  } else {
    bar.Sub( Upp::t_( "&Operations" ), [=] ( Upp::Bar& bar ) { OperationsAdd( bar ); } );
    bar.Sub( Upp::t_( "&Set bookmarks" ), [=] ( Upp::Bar& bar ) { SetBookmarkAdd( bar ); } );
    bar.Sub( Upp::t_( "&Go to bookmarks" ), [=] ( Upp::Bar& bar ) { GoToBookmarkAdd( bar ); } );
  }
  
  if ( bar.IsToolBar() ) {
    Upp::Function< void() >  GauraFontHeight_, SortirovkaUstanovka_, SmenaYazyka_;
    
    YazykDropList.Tip( Upp::t_( "Language" ) );
    YazykDropList.DropWidthZ( 148 );
    YazykDropList.NoWantFocus();
    
    SmenaYazyka_          << [&] { SmenaYazyka(); };
    SortirovkaUstanovka_  << [&] { SortirovkaUstanovka(); };
    // WhenPush в отличие от WhenAction даёт существенно другое поведение DropList!
    // Когда WhenPush - похоже на кнопку и событие не возникает при выборе из выпадающего списка
    // когда WhenAction - на кнопку не похоже, сразу выпадает список и событие возникает при
    // выборе из списка
    YazykDropList.WhenAction = SmenaYazyka_;
    
    SortDropList.Add( Upp::t_( "Without sorting" ) );
    SortDropList.Add( Upp::t_( "Sanskrit ascending" ) );
    SortDropList.Add( Upp::t_( "Sanskrit descending" ) );
    SortDropList.Add( Upp::t_( "Translation ascending" ) );
    SortDropList.Add( Upp::t_( "Translation descending" ) );
    SortDropList.Tip( Upp::t_( "Sort" ) );
    SortDropList.DropWidthZ( 200 );
    SortDropList.NoWantFocus();
    SortDropList.WhenPush = SortirovkaUstanovka_;
    
    bar.Separator();
    bar.Add( YazykDropList, 138 );
    bar.Separator();
    bar.Add( SortDropList, 170 );
    bar.Separator();
    
    GauraFontHeightEdit.MinMax( GauraFontHeightMin, GauraFontHeightMax );
    GauraFontHeight_ << [&] { SetGauraFontHeight( ~GauraFontHeightEdit ); };
    GauraFontHeightEdit.WhenAction = GauraFontHeight_;
    GauraFontHeightEdit.NoWantFocus();
    GauraFontHeightEdit.SetData( GauraFontHeightMin );
    GauraFontHeightEdit.WhenAction();
    GauraFontHeightEdit.Tip( Upp::t_( "Gaura Times font height" ) );
    bar.Add( GauraFontHeightEdit, 46 );
  }
}

void PrabhupadaSlovaryPanel::SetGauraFontHeight( int H )
{
  if ( H < GauraFontHeightMin || H > GauraFontHeightMax )
    return;
  if ( GauraFontHeight != H ) {
    GauraFontHeight = H;
    GauraFont.Height( GauraFontHeight );
    ArraySanskrit.SetLineCy( GauraFont.GetCy() );

    SanskritFilterEdit.SetFont( GauraFont );
    PerevodFilterEdit.SetFont( GauraFont );
  
    EditSanskrit.SetFont( GauraFont );
    EditPerevod.SetFont( GauraFont );
    
    if ( GauraFontHeightEdit.GetData() != GauraFontHeight )
      GauraFontHeightEdit.SetData( GauraFontHeight );
  }
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
                                                             PrabhupaComareLess( VectorSanskrit[ a.Index ].Perevod,  VectorSanskrit[ b.Index ].Perevod ) :
                                                             PrabhupaComareLess( VectorSanskrit[ a.Index ].Sanskrit, VectorSanskrit[ b.Index ].Sanskrit ); } );
      break;
    case VidSortirovka::SanskritUbyvanie :
      Upp::Sort( VectorIndex
               , [&] ( IndexPair& a, IndexPair& b ) { return VectorSanskrit[ a.Index ].Sanskrit == VectorSanskrit[ b.Index ].Sanskrit ?
                                                             PrabhupaComareMore( VectorSanskrit[ a.Index ].Perevod,  VectorSanskrit[ b.Index ].Perevod ) :
                                                             PrabhupaComareMore( VectorSanskrit[ a.Index ].Sanskrit, VectorSanskrit[ b.Index ].Sanskrit ); } );
      break;
    case VidSortirovka::PerevodVozrastanie :
      Upp::Sort( VectorIndex
               , [&] ( IndexPair& a, IndexPair& b ) { return VectorSanskrit[ a.Index ].Perevod  == VectorSanskrit[ b.Index ].Perevod ?
                                                             PrabhupaComareLess( VectorSanskrit[ a.Index ].Sanskrit, VectorSanskrit[ b.Index ].Sanskrit ) :
                                                             PrabhupaComareLess( VectorSanskrit[ a.Index ].Perevod,  VectorSanskrit[ b.Index ].Perevod ); } );
      break;
    case VidSortirovka::PerevodUbyvanie :
      Upp::Sort( VectorIndex
               , [&] ( IndexPair& a, IndexPair& b ) { return VectorSanskrit[ a.Index ].Perevod  == VectorSanskrit[ b.Index ].Perevod ?
                                                             PrabhupaComareMore( VectorSanskrit[ a.Index ].Sanskrit, VectorSanskrit[ b.Index ].Sanskrit ) :
                                                             PrabhupaComareMore( VectorSanskrit[ a.Index ].Perevod,  VectorSanskrit[ b.Index ].Perevod ); } );
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

inline bool PrabhupaComareLess( const Upp::String& A, const Upp::String& B )
{
  Upp::WString AW = A.ToWString(),
               BW = B.ToWString();

  int AWL = AW.GetCount(),
      BWL = BW.GetCount();

  int L = Upp::min( AWL, BWL );
  
  int AWV, BWV;
  for ( int i = 0; i < L; ++i ) {
    AWV = BukvaryPrabhupada[ AW[ i ] ].Ves;
    BWV = BukvaryPrabhupada[ BW[ i ] ].Ves;
    if ( AWV == BWV ) {
      continue;
    } else if ( AWV > BWV ) {
      return false;
    } else if ( AWV < BWV ) {
      return true;
    }
  }
    
  if ( AWL < BWL )
    return true;
  
  return false;
}

inline bool PrabhupaComareMore( const Upp::String& A, const Upp::String& B )
{
  Upp::WString AW = A.ToWString(),
               BW = B.ToWString();

  int AWL = AW.GetCount(),
      BWL = BW.GetCount();

  int L = Upp::min( AWL, BWL );
  
  int AWV, BWV;
  for ( int i = 0; i < L; ++i ) {
    AWV = BukvaryPrabhupada[ AW[ i ] ].Ves;
    BWV = BukvaryPrabhupada[ BW[ i ] ].Ves;
    if ( AWV == BWV ) {
      continue;
    } else if ( AWV < BWV ) {
      return false;
    } else if ( AWV > BWV ) {
      return true;
    }
  }
    
  if ( AWL < BWL )
    return true;

  return false;
}

inline bool operator < ( const SanskritPair& a, const SanskritPair& b )
{
  return a.Sanskrit == b.Sanskrit ? a.Perevod  <  b.Perevod : a.Sanskrit <  b.Sanskrit;
}

inline bool operator > ( const SanskritPair& a, const SanskritPair& b )
{
  return a.Sanskrit == b.Sanskrit ? a.Perevod  >  b.Perevod : a.Sanskrit >  b.Sanskrit;
}

void PrabhupadaSlovaryPanel::RemoveDuplicatesSanskrit()
{
  Upp::Sql sql( Session );
  sql.SetStatement( "delete from SANSKRIT where ID = ?" );
  VectorIndex.ResetIndex();
  // Сортируем по простому без выкрутасов! Но сортировка обязательна для єтого алгоритма!
  Upp::Sort( VectorSanskrit, std::less< SanskritPair >() );
  bool NeedBigRefresh = false;
  Prabhupada::RemoveDuplicates< SanskritVector >( VectorSanskrit,
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
  if ( Upp::PromptOKCancel( Upp::t_( "Delete a word?" ) ) ) {
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
  if ( Upp::PromptOKCancel( Upp::t_( "To remove the tagged words?" ) ) ) {
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
  ArraySanskrit.DoEdit();
}

PrabhupadaSlovaryWindow::PrabhupadaSlovaryWindow( CommandMap& cm )
{
  Upp::Function< void( Upp::Stream& ) > Serial;
  Serial = [&] ( Upp::Stream& S ) { Serialize( S ); };

  Add( PanelPrabhupadaSlovary );
  PanelPrabhupadaSlovary.SizePos();
  Upp::Rect( 0, 0, 600, 600 );
  Title( Upp::t_( "Shrila Prabhupada's Sanskrit dictionary!" ) ).Sizeable().Zoomable();

  int i, &StrongYazyk = PanelPrabhupadaSlovary.StrongYazyk;
  VidSortirovka InitSortirovka = VidSortirovka::Reset;

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
    InitSortirovka = VidSortirovka::SanskritVozrastanie;
  }

  if ( StrongYazyk != -1 )
    PanelPrabhupadaSlovary.SetYazyk( StrongYazyk );

  if ( InitSortirovka != VidSortirovka::Reset )
    PanelPrabhupadaSlovary.SetSortirovka( InitSortirovka );
  
  FilterSlovary F = PanelPrabhupadaSlovary.Filter;
  PanelPrabhupadaSlovary.Filter.Clear();
  PanelPrabhupadaSlovary.SetFilter( F );
}

PrabhupadaSlovaryWindow::~PrabhupadaSlovaryWindow()
{
  Upp::Function< void( Upp::Stream& ) > Serial;
  Serial = [&] ( Upp::Stream& S ) { Serialize( S ); };
  Upp::StoreToFile( Serial, Prabhupada::PrabhupadaSlovaryIniFile, 0 );
}

void BookmarkInfo::Serialize( Upp::Stream& s )
{
  int Sortirovka_;
  if ( s.IsStoring() )
    Sortirovka_ = static_cast< int >( Sortirovka );

  s % RowNum % Filter % Sortirovka_ % Yazyk;

  if ( s.IsLoading() )
    Sortirovka = static_cast< VidSortirovka >( Sortirovka_ );
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

  Upp::String S;
  
  if ( s.IsStoring() )
    S = SerialStream.GetResult();
 
  s % S;
 
  if ( s.IsLoading() )
     SerialStream.Open( S );

  ArraySanskrit.SerializeHeader( s );
  ArraySanskrit.SerializeSettings( s );
  SplitterSearch.Serialize( s );
  Filter.Serialize( s );
  Bookmark.Serialize( s );
  
  int LastYazyk, LastCursor, LastSortirovka, LastGauraFontHeight;
  if ( s.IsStoring() ) {
    LastCursor          = ArraySanskrit.GetCursor();
    LastYazyk           = Yazyk;
    LastSortirovka      = static_cast< int >( Sortirovka );
    LastGauraFontHeight = GauraFontHeight;
  }
  s % LastCursor % LastYazyk % LastSortirovka % LastGauraFontHeight;
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
    SetGauraFontHeight( LastGauraFontHeight );
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

  Upp::String Sanskrit = Filter.FilterSanskrit,
              Perevod  = Filter.FilterPerevod;

  Sanskrit = RemoveDiacritics( Sanskrit );
  Perevod  = RemoveDiacritics( Perevod );
              
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
    if ( Filter.Reset ) {
      VectorIndex.ResetIndex();
      Filter.Reset = false;
    }

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
      if ( Upp::PromptYesNo( Upp::t_( "Save changes?" ) ) ) {
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
