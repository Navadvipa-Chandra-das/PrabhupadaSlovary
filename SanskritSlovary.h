#ifndef _SanskritSlovary_SanskritSlovary_h
#define _SanskritSlovary_SanskritSlovary_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#define LAYOUTFILE <SanskritSlovary/SanskritSlovary.lay>
#include <CtrlCore/lay.h>

#define IMAGEFILE <SanskritSlovary/SanskritSlovary.iml>
#include <Draw/iml_header.h>

#include <plugin/sqlite3/Sqlite3.h>

static Font GetGauraFont();

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

class SanscritVector : public Vector< SanskritPair > {
};

using SanskritSlovaryPanelParent = WithSanskritSlovaryPanel< TopWindow >;

class SanskritSlovaryPanel : public SanskritSlovaryPanelParent {
public:
  EditString SansritEdit;
  EditString PerevodEdit;
  typedef SanskritSlovaryPanel CLASSNAME;
  SanskritSlovaryPanel();
  void Dobavity();
  void Udality();
  void Udality2();
  void Udality3();
  Sqlite3Session Session;
  YazykVector VectorYazyk;
  SanscritVector VectorSanskrit;
  void PrepareBar( Bar& bar );
  EditString EditSanskrit;
  EditString EditPerevod;
  void PrepareVectorSanskrit();
  void PrepareVectorYazyk();
  DropList YazykDropList;
  virtual void Serialize( Stream& s );
  int Yazyk = -1;
  void SetYazyk( int y );
};

class SanskritSlovaryWindow : public TopWindow {
public:
  SanskritSlovaryPanel PanelSanskritSlovary;
  typedef SanskritSlovaryWindow CLASSNAME;
  SanskritSlovaryWindow();
  virtual void Serialize( Stream& s );
};

#endif
