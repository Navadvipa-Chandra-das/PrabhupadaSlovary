#ifndef _PrabhupadaSlovary_PrabhupadaSlovary_h
#define _PrabhupadaSlovary_PrabhupadaSlovary_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#define LAYOUTFILE <PrabhupadaSlovary/PrabhupadaSlovary.lay>
#include <CtrlCore/lay.h>

#define IMAGEFILE <PrabhupadaSlovary/PrabhupadaSlovary.iml>
#include <Draw/iml_header.h>

#include <plugin/sqlite3/Sqlite3.h>

class SanskritPair : Moveable< SanskritPair > {
public:
  int Index; // для фильтрации поиска
  String Sanskrit;
  String Perevod;
  String ToString() const { return Sanskrit + " = " + Perevod; }
};

struct YazykInfo : Moveable< YazykInfo > {
  int ID;
  String Yazyk;
  String YazykSlovo;
  String ToString() const { return AsString( ID ) + " - " + Yazyk + " - " + YazykSlovo; }
};

class YazykVector : public Vector< YazykInfo > {
public:
  FindYazyk( const String& S );
};

class SanskritVector : public Vector< SanskritPair > {
public:
  int DlinaVector = 0;
};

static Font GetGauraFont();

struct NumberToSanskrit : public Convert {
  SanskritVector& VectorSanskrit;
  NumberToSanskrit( SanskritVector& AVectorSanskrit ) : VectorSanskrit( AVectorSanskrit ) {};
  virtual Value  Format( const Value& q ) const;
};

struct NumberToPerevod : public Convert {
  SanskritVector& VectorSanskrit;
  NumberToPerevod( SanskritVector& AVectorSanskrit ) : VectorSanskrit( AVectorSanskrit ) {};
  virtual Value  Format( const Value& q ) const;
};

using PrabhupadaSlovaryPanelParent = WithPrabhupadaSlovaryPanel< ParentCtrl/*TopWindow*/ >;

class PrabhupadaSlovaryPanel : public PrabhupadaSlovaryPanelParent {
public:
  EditString SanskritPoiskEdit;
  EditString PerevodPoiskEdit;
  typedef PrabhupadaSlovaryPanel CLASSNAME;
  PrabhupadaSlovaryPanel();
  void Dobavity();
  void Udality();
  void Udality2();
  void Udality3();
  void SmenaYazyka();
  Sqlite3Session Session;
  YazykVector VectorYazyk;
  SanskritVector VectorSanskrit;
  void PrepareBar( Bar& bar );
  EditString EditSanskrit;
  EditString EditPerevod;
  void PrepareVectorSanskrit();
  void PrepareVectorYazyk();
  DropList YazykDropList;
  virtual void Serialize( Stream& s );
  int Yazyk = -1;
  void SetYazyk( int y );
  NumberToSanskrit FNumberToSanskrit { VectorSanskrit };
  NumberToPerevod  FNumberToPerevod  { VectorSanskrit };
  void IndicatorRow();
  int StrongYazyk = -1;
};

class PrabhupadaSlovaryWindow : public TopWindow {
public:
  PrabhupadaSlovaryPanel PanelPrabhupadaSlovary;
  typedef PrabhupadaSlovaryWindow CLASSNAME;
  PrabhupadaSlovaryWindow();
  virtual void Serialize( Stream& s );
};

#endif
