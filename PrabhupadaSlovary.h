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

namespace Prabhupada {

struct FilterSlovary : Upp::Moveable< FilterSlovary > {
  Upp::String FilterSanskrit;
  Upp::String FilterPerevod;
  Upp::String ToString() const { return FilterSanskrit + " ; " + FilterPerevod; }
  bool Reset = true;
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

class SanskritPair : Upp::Moveable< SanskritPair > {
public:
  mutable int Index; // для фильтрации поиска
  mutable int ReservIndex; // для сохранения после поиска
  int         ID;
  bool        DeleteCandidat = false;
  Upp::String Sanskrit;
  Upp::String Perevod;
  Upp::String ToString() const { return Upp::AsString( ID ) + " : " + Sanskrit + " = " + Perevod; }
  bool operator == ( const SanskritPair& sp ) {
    return ( Sanskrit == sp.Sanskrit ) && ( Perevod == sp.Perevod );
  }
  bool operator != ( const SanskritPair& sp ) {
    return ( Sanskrit != sp.Sanskrit ) || ( Perevod != sp.Perevod );
  }
};

inline bool operator < ( const SanskritPair& a, const SanskritPair& b );

inline bool operator > ( const SanskritPair& a, const SanskritPair& b );

void ArrayCtrlGetSelIndexes( Upp::ArrayCtrl& A, Upp::Vector< int >& R );

struct YazykInfo : Upp::Moveable< YazykInfo > {
  int ID;
  Upp::String Yazyk;
  Upp::String YazykSlovo;
  Upp::String ToString() const { return Upp::AsString( ID ) + " - " + Yazyk + " - " + YazykSlovo; }
};

class YazykVector : public Upp::Vector< YazykInfo > {
public:
  FindYazyk( const Upp::String& S );
};

class SanskritVector : public Upp::Vector< SanskritPair > {
public:
  int DlinaVector = 0;
  int LastID;
  void SaveIndexToReserv();
  void LoadIndexFromReserv();
  void ResetIndex();
};

static Upp::Font GetGauraFont();

using ColumnGetter = Upp::Function< void ( const Upp::Value&, int ) >;
using ColumnSetter = Upp::Function< Upp::Value ( const Upp::Value& ) >;

using PrabhupadaSlovaryPanelParent = Upp::WithPrabhupadaSlovaryPanel< Upp::ParentCtrl/*TopWindow*/ >;
using AboutPrabhupadaSlovaryParent = Upp::WithAboutPrabhupadaSlovary< Upp::TopWindow >;

class AboutPrabhupadaSlovaryWindow : public AboutPrabhupadaSlovaryParent {
public:
  typedef AboutPrabhupadaSlovaryWindow CLASSNAME;
  AboutPrabhupadaSlovaryWindow();
	virtual void Paint( Upp::Draw& draw );
};

class PrabhupadaSlovaryPanel : public PrabhupadaSlovaryPanelParent {
public:
  typedef PrabhupadaSlovaryPanel CLASSNAME;
  enum class VidSortirovka : int { Reset = -1, NotSorted = 0, SanskritVozrastanie, SanskritUbyvanie, PerevodVozrastanie, PerevodUbyvanie };
  VidSortirovka Sortirovka = VidSortirovka::Reset;
  void SetSortirovka( VidSortirovka s );
  void BigRefresh();
  Upp::EditString SanskritPoiskEdit;
  Upp::EditString PerevodPoiskEdit;
  PrabhupadaSlovaryPanel();
  Upp::Color ColorInkDelete { Upp::Red() };
  Upp::Color ColorPaperDelete { Upp::White() };
  void AddSlovo();
  void MarkDeleteSlovo();
  void DeleteSlovo( int i );
  void DeleteSlova();
  void Edit();
  void SmenaYazyka();
  void RemoveDubli();
  Upp::Sqlite3Session Session;
  YazykVector VectorYazyk;
  SanskritVector VectorSanskrit;
  void PrepareBar( Upp::Bar& bar );
  Upp::EditString EditSanskrit;
  Upp::EditString EditPerevod;
  void PrepareVectorSanskrit();
  void PrepareVectorYazyk();
  Upp::DropList YazykDropList;
  Upp::DropList SortDropList;
  void SortirovkaUstanovka();
  virtual void Serialize( Upp::Stream& s );
  int Yazyk = -1;
  void SetYazyk( int y );
  Upp::Font GauraFont;
  void IndicatorRow();
  void SetVectorSanskritDlinaVector( int d );
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
