#ifndef _PrabhupadaSlovary_PrabhupadaSlovary_h
#define _PrabhupadaSlovary_PrabhupadaSlovary_h

#include <CtrlLib/CtrlLib.h>

namespace Upp {

#define LAYOUTFILE <PrabhupadaSlovary/PrabhupadaSlovary.lay>
#include <CtrlCore/lay.h>

#define IMAGEFILE <PrabhupadaSlovary/PrabhupadaSlovary.iml>
#include <Draw/iml_header.h>

}

#include <plugin/sqlite3/Sqlite3.h>
#include "PrabhupadaSlovaryAlgorithm.h"
#include <map>

namespace Prabhupada {

struct IndexPair : Upp::Moveable< IndexPair > {
  int Index;
  int ReservIndex;
  Upp::String ToString() const { return Upp::AsString( ReservIndex ) + " : " + Upp::AsString( Index ); }
  void Serialize( Upp::Stream& s )
  {
    s % Index % ReservIndex;
  };
};

struct IndexVector : Upp::Vector< IndexPair > {
  void SaveIndexToReserv();
  void LoadIndexFromReserv();
  void ResetIndex();
};

struct FilterSlovary : Upp::Moveable< FilterSlovary > {
  Upp::String FilterSanskrit;
  Upp::String FilterPerevod;
  Upp::String ToString() const { return FilterSanskrit + " ; " + FilterPerevod; }
  bool Reset = false;
  void Serialize( Upp::Stream& s )
  {
    int version = 0;
    s / version;
    s % FilterSanskrit % FilterPerevod;
  };
  bool operator == ( const FilterSlovary& F ) {
    return ( FilterSanskrit == F.FilterSanskrit ) && ( FilterPerevod == F.FilterPerevod );
  }
  bool operator != ( const FilterSlovary& F ) {
    return ( ( FilterSanskrit != F.FilterSanskrit ) || ( FilterPerevod != F.FilterPerevod ) );
  }
  bool IsEmpty() const {
    return FilterSanskrit.IsEmpty() && FilterPerevod.IsEmpty();
  }
  void Clear() { FilterSanskrit.Clear(); FilterPerevod.Clear(); Reset = true; }
};

struct PrabhupadaBukva {
  int Bukva;
  int Ves;
  Upp::String ToString() const { return Upp::AsString( Bukva ) + " : " + Upp::AsString( Ves ); }
};

using PrabhupadaBukvary = std::map< int, PrabhupadaBukva >;

inline bool PrabhupaComareLess( const Upp::String& A, const Upp::String& B );
inline bool PrabhupaComareMore( const Upp::String& A, const Upp::String& B );

class SanskritPair : Upp::Moveable< SanskritPair > {
public:
  int         ID;
  bool        DeleteCandidat = false;
  Upp::String Sanskrit;
  Upp::String Perevod;
  // WD meens itout diakritiks symbhol
  Upp::String SanskritWD;
  Upp::String PerevodWD;
  Upp::String ToString() const { return Upp::AsString( ID ) + " : " + Sanskrit.ToString() + " = " + Perevod.ToString(); }
  bool operator == ( const SanskritPair& sp ) {
    return ( Sanskrit == sp.Sanskrit ) && ( Perevod == sp.Perevod );
  }
  bool operator != ( const SanskritPair& sp ) {
    return ( Sanskrit != sp.Sanskrit ) || ( Perevod != sp.Perevod );
  }
};

inline bool operator < ( const SanskritPair& a, const SanskritPair& b );
inline bool operator > ( const SanskritPair& a, const SanskritPair& b );

struct YazykInfo : Upp::Moveable< YazykInfo > {
  int ID;
  Upp::String Yazyk;
  Upp::String YazykSlovo;
  Upp::String ToString() const { return Upp::AsString( ID ) + " - " + Yazyk + " - " + YazykSlovo; }
};

class YazykVector : public Upp::Vector< YazykInfo > {
public:
  int FindYazyk( const Upp::String& S );
};

class SanskritVector : public Upp::Vector< SanskritPair > {
public:
  int FilterGetCount = 0;
  int LastID;
  template < class Condition, class OnRemove >
  void RemoveIf( Condition c, const OnRemove& On_Remove );
};

using PrabhupadaGoToLineParent = Upp::WithPrabhupadaGoToLine< Upp::TopWindow >;

using PrabhupadaSlovaryPanelParent = Upp::WithPrabhupadaSlovaryPanel< Upp::ParentCtrl/*TopWindow*/ >;
using AboutPrabhupadaSlovaryParent = Upp::WithAboutPrabhupadaSlovary< Upp::TopWindow >;

using PrabhupadaTabAboutParent  = Upp::WithPrabhupadaTabAbout< Upp::ParentCtrl >;
using PrabhupadaTabLetterParent = Upp::WithPrabhupadaTabLetter< Upp::ParentCtrl >;

struct PrabhupadaGoToLineDialog : PrabhupadaGoToLineParent {
  typedef PrabhupadaGoToLineDialog CLASSNAME;
  Upp::StringStream& SerialStream;
  PrabhupadaGoToLineDialog( Upp::StringStream& SerialStream_ );
  virtual ~PrabhupadaGoToLineDialog();
  void ButtonGoClick();
  void ButtonCancelClick();
  int RowNum { 0 };
  virtual void Serialize( Upp::Stream& s );
};

struct PrabhupadaTabAboutPanel : PrabhupadaTabAboutParent {
  typedef PrabhupadaTabAboutPanel CLASSNAME;
  PrabhupadaTabAboutPanel();
  virtual void Paint( Upp::Draw& draw );
};

struct PrabhupadaTabLetterPanel : PrabhupadaTabLetterParent {
  typedef PrabhupadaTabLetterPanel CLASSNAME;
  PrabhupadaTabLetterPanel();
};

class AboutPrabhupadaSlovaryWindow : public AboutPrabhupadaSlovaryParent {
public:
  typedef AboutPrabhupadaSlovaryWindow CLASSNAME;
  AboutPrabhupadaSlovaryWindow();
  PrabhupadaTabAboutPanel PanelPrabhupadaTabAbout;
  PrabhupadaTabLetterPanel PanelPrabhupadaTabLetter;
};

enum class VidSortirovka : int { Reset = -1, NotSorted = 0, SanskritVozrastanie, SanskritUbyvanie, PerevodVozrastanie, PerevodUbyvanie };

struct BookmarkInfo : Upp::Moveable< BookmarkInfo > {
  int RowNum { -1 };
  FilterSlovary Filter;
  VidSortirovka Sortirovka;
  int Yazyk;
  Upp::String ToString() const { return Upp::AsString( RowNum ) + " : " + Upp::AsString( Filter ) + " : " + Upp::AsString( static_cast< int >( Sortirovka ) ) +  + " : " + Upp::AsString( Yazyk ); }
  void Serialize( Upp::Stream& s );
  void Clear() { RowNum = -1; };
};

class BookmarkVector : public Upp::Vector< BookmarkInfo > {
public:
  void ClearBookmark();
  BookmarkVector() {
                     BookmarkInfo BI;
                     for ( int i = 0; i < 10; ++i )
                       Add( BI );
                   };
};

class PrabhupadaSlovaryPanel : public PrabhupadaSlovaryPanelParent {
public:
  typedef PrabhupadaSlovaryPanel CLASSNAME;
  Upp::StringStream SerialStream;
  VidSortirovka Sortirovka = VidSortirovka::Reset;
  void SetSortirovka( VidSortirovka s );
  BookmarkVector Bookmark;
  void SetBookmark( int N );
  void GoToBookmark( int N );
  void BigRefresh();
  Upp::EditString SanskritFilterEdit;
  Upp::EditString PerevodFilterEdit;
  Upp::EditIntSpin GauraFontHeightEdit;
  void SetGauraFontHeight( int H );
  PrabhupadaSlovaryPanel();
  Upp::Color ColorInkDelete { Upp::Red() };
  Upp::Color ColorPaperDelete { Upp::White() };
  void PrepareBukvaryPrabhupada();
  Upp::String RemoveDiacritics( const Upp::String& S );
  void AddSlovo();
  void MarkDeleteSlovo();
  void DeleteSlova();
  void PrabhupadaGoToLine();
  void Edit();
  void SmenaYazyka();
  void RemoveDuplicatesSanskrit();
  Upp::Sqlite3Session Session;
  YazykVector VectorYazyk;
  SanskritVector VectorSanskrit;
  IndexVector VectorIndex;
  void SetBookmarkAdd( Upp::Bar& bar );
  void GoToBookmarkAdd( Upp::Bar& bar );
  void OperationsAdd( Upp::Bar& bar );
  void PrepareBar( Upp::Bar& bar );
  Upp::EditString EditSanskrit;
  Upp::EditString EditPerevod;
  void PrepareVectorSanskrit();
  void PrepareVectorYazyk();
  Upp::DropList YazykDropList;
  Upp::DropList SortDropList;
  int GauraFontHeight { 12 };
  void SortirovkaUstanovka();
  virtual void Serialize( Upp::Stream& s );
  int Yazyk = -1;
  void SetYazyk( int y );
  Upp::Font GauraFont;
  void IndicatorRow();
  void SetVectorSanskritFilterGetCount( int d );
  int StrongYazyk = -1;
  FilterSlovary Filter;
  void SetFilter( const FilterSlovary& F );
  void CopyToClipboard();
  void FilterUstanovka();
  void FilterVectorSanskrit();
  void ArraySanskritRefresh();
  Upp::Value GetSanskrit( const Upp::Value& V );
  Upp::Value GetPerevod( const Upp::Value& V );
  void SetSanskrit( const Upp::Value& V, int i );
  void SetPerevod( const Upp::Value& V, int i );
  void AboutPrabhupadaSlovary();
  void IfEditOKCancel();
  void ArraySanskritInserter( int I );
  int GetVectorSanskritLastID();
  void PrabhupadaBukvary();
};

class PrabhupadaSlovaryWindow : public Upp::TopWindow {
public:
  PrabhupadaSlovaryPanel PanelPrabhupadaSlovary;
  typedef PrabhupadaSlovaryWindow CLASSNAME;
  PrabhupadaSlovaryWindow( CommandMap& cm );
  virtual void Serialize( Upp::Stream& s );
  virtual ~PrabhupadaSlovaryWindow();
};

} // namespace Prabhupada

#endif
