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
  void Clear() { FilterSanskrit.Clear(); FilterPerevod.Clear(); }
};

class SanskritPair : Upp::Moveable< SanskritPair > {
public:
  int Index; // для фильтрации поиска
  int IndexReserv; // для сохранения после поиска
  Upp::String Sanskrit;
  Upp::String Perevod;
  Upp::String ToString() const { return Sanskrit + " = " + Perevod; }
  bool operator == ( const SanskritPair& sp ) {
    return ( Sanskrit == sp.Sanskrit ) && ( Perevod == sp.Perevod );
  }
  bool operator != ( const SanskritPair& sp ) {
    return ( Sanskrit != sp.Sanskrit ) || ( Perevod != sp.Perevod );
  }
};

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

struct NumberToSanskrit : public Upp::Convert {
  SanskritVector& VectorSanskrit;
  NumberToSanskrit( SanskritVector& AVectorSanskrit ) : VectorSanskrit( AVectorSanskrit ) {};
  virtual Upp::Value  Format( const Upp::Value& q ) const;
};

struct NumberToPerevod : public Upp::Convert {
  SanskritVector& VectorSanskrit;
  NumberToPerevod( SanskritVector& AVectorSanskrit ) : VectorSanskrit( AVectorSanskrit ) {};
  virtual Upp::Value  Format( const Upp::Value& q ) const;
};

using PrabhupadaSlovaryPanelParent = Upp::WithPrabhupadaSlovaryPanel< Upp::ParentCtrl/*TopWindow*/ >;

class PrabhupadaSlovaryPanel : public PrabhupadaSlovaryPanelParent {
public:
  typedef PrabhupadaSlovaryPanel CLASSNAME;
  enum class VidSortirovka : int { Reset = -1, NotSorted = 0, SanskritVozrastanie, SanskritUbyvanie, PerevodVozrastanie, PerevodUbyvanie };
  VidSortirovka Sortirovka = VidSortirovka::Reset;
  void SetSortirovka( VidSortirovka s );
  Upp::EditString SanskritPoiskEdit;
  Upp::EditString PerevodPoiskEdit;
  PrabhupadaSlovaryPanel();
  void Dobavity();
  void Udality();
  void Edit();
  void SmenaYazyka();
  void UdalityDubli();
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
  NumberToSanskrit FNumberToSanskrit { VectorSanskrit };
  NumberToPerevod  FNumberToPerevod  { VectorSanskrit };
  void IndicatorRow();
  void SetVectorSanskritDlinaVector( int d );
  int StrongYazyk = -1;
  FilterSlovary Filter;
  void SetFilter( const FilterSlovary& F );
  void CopyToClipboard();
  void FilterUstanovka();
  void FilterVectorSanskrit();
  void ArraySanskritRefresh();
};

class PrabhupadaSlovaryWindow : public Upp::TopWindow {
public:
  PrabhupadaSlovaryPanel PanelPrabhupadaSlovary;
  typedef PrabhupadaSlovaryWindow CLASSNAME;
  PrabhupadaSlovaryWindow();
  virtual void Serialize( Upp::Stream& s );
};

} // namespace Prabhupada

#endif
