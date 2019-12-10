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

class SanskritPair : Upp::Moveable< SanskritPair > {
public:
  int Index; // для фильтрации поиска
  Upp::String Sanskrit;
  Upp::String Perevod;
  Upp::String ToString() const { return Sanskrit + " = " + Perevod; }
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
  enum class VidSortirovka : int { SanskritVozrastanie = 0, SanskritUbyvanie, PerevodVozrastanie, PerevodUbyvanie };
  Upp::EditString SanskritPoiskEdit;
  Upp::EditString PerevodPoiskEdit;
  PrabhupadaSlovaryPanel();
  void Dobavity();
  void Udality();
  void Edit();
  void SmenaYazyka();
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
  void Sortirovka();
  virtual void Serialize( Upp::Stream& s );
  int Yazyk = -1;
  void SetYazyk( int y );
  NumberToSanskrit FNumberToSanskrit { VectorSanskrit };
  NumberToPerevod  FNumberToPerevod  { VectorSanskrit };
  void IndicatorRow();
  int StrongYazyk = -1;
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
