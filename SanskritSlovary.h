#ifndef _SanskritSlovary_SanskritSlovary_h
#define _SanskritSlovary_SanskritSlovary_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#define LAYOUTFILE <SanskritSlovary/SanskritSlovary.lay>
#include <CtrlCore/lay.h>

#define IMAGEFILE <SanskritSlovary/SanskritSlovary.iml>
#include <Draw/iml_header.h>

#include <plugin/sqlite3/Sqlite3.h>

class SanskritPair : Moveable< SanskritPair > {
public:
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
};

class SanskritVector : public Vector< SanskritPair > {
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

using SanskritSlovaryPanelParent = WithSanskritSlovaryPanel< TopWindow >;

class SanskritSlovaryPanel : public SanskritSlovaryPanelParent {
public:
  EditString SanskritPoiskEdit;
  EditString PerevodPoiskEdit;
  typedef SanskritSlovaryPanel CLASSNAME;
  SanskritSlovaryPanel();
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
};

class SanskritSlovaryWindow : public TopWindow {
public:
  SanskritSlovaryPanel PanelSanskritSlovary;
  typedef SanskritSlovaryWindow CLASSNAME;
  SanskritSlovaryWindow();
  virtual void Serialize( Stream& s );
};

#endif
