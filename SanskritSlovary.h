#ifndef _SanskritSlovary_SanskritSlovary_h
#define _SanskritSlovary_SanskritSlovary_h

#include <CtrlLib/CtrlLib.h>

using namespace Upp;

#define LAYOUTFILE <SanskritSlovary/SanskritSlovary.lay>
#include <CtrlCore/lay.h>

#define IMAGEFILE <SanskritSlovary/SanskritSlovary.iml>
#include <Draw/iml_header.h>

class SanskritSlovaryPanel : public WithSanskritSlovaryPanel< ParentCtrl > {
public:
  EditString SansritEdit;
  EditString PerevodEdit;
  typedef SanskritSlovaryPanel CLASSNAME;
  SanskritSlovaryPanel();
  void Dobavity();
  void Udality();
};

class SanskritSlovaryWindow : public TopWindow {
public:
  SanskritSlovaryPanel PanelSanskritSlovary;
  typedef SanskritSlovaryWindow CLASSNAME;
  SanskritSlovaryWindow();
};

#endif
