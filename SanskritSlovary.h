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

class SanscritVector : public Vector< SanskritPair > {
};

class SanskritSlovaryPanel : public WithSanskritSlovaryPanel< /*ParentCtrl*/TopWindow > {
public:
  EditString SansritEdit;
  EditString PerevodEdit;
  typedef SanskritSlovaryPanel CLASSNAME;
  SanskritSlovaryPanel();
  void Dobavity();
  void Udality();
  void Udality2();
  void Udality3();
  Sqlite3Session sqlite3;
  SanscritVector VectorSanskrit;
  void PrepareBar( Bar& bar );
  EditString EditSanskrit;
  EditString EditPerevod;
  void PrepareVectorSanskrit();
  DropList YazykDropList;
};

class SanskritSlovaryWindow : public TopWindow {
public:
  SanskritSlovaryPanel PanelSanskritSlovary;
  typedef SanskritSlovaryWindow CLASSNAME;
  SanskritSlovaryWindow();
  // Вместо перекрыия виртуальной функции Serialize сделаем свою SerializeApp, так как Serialize сериализует все контролы окна. Это никчему. 
  void SerializeApp( Stream& s );
};

#endif
