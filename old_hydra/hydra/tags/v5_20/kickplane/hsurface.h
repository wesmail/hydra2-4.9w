#ifndef HSURFACE_H
#define HSURFACE_H

#include <TObject.h>
#include <iostream.h>

class HGeomVector;

class HSurface : public TObject {
public:
  HSurface(void) {}
  ~HSurface(void) {}
  virtual void printTo(ostream &out)=0;
  virtual void readFrom(istream &in)=0;
  virtual void calcIntersection(const HGeomVector &r,const HGeomVector &alpha,
				HGeomVector &out) const =0;
  ClassDef(HSurface,0) //Defines an abstract surface for the kick plane
};

class HSurfPlane : public HSurface {
protected:
  Double_t fDx,fDz,fC; //Plane equation: y=fDx*x + fDz*z + fC
public:
  HSurfPlane(void) { setParams(0,0,0); }
  ~HSurfPlane(void) {}
  void calcIntersection(const HGeomVector &r,const HGeomVector &alpha,
			HGeomVector &out) const;
  inline void setParams(const Double_t dx, const Double_t dz, const Double_t c);
  void printTo(ostream &out) { out << fDx << fDz << fC << endl; }
  void readFrom(istream &in) { in >> fDx >> fDz >> fC; }
  ClassDef(HSurfPlane,0) //Defines a plane surface
};

class HSurfXCilinder : public HSurface {
protected:
  Double_t fDx,fDz,fC; // y=fDx*x*x + fDz*z +fC;
public:
  HSurfXCilinder(void) { setParams(0,0,0); }
  ~HSurfXCilinder(void) {}
  void calcIntersection(const HGeomVector &r,const HGeomVector &alpha,
			HGeomVector &out) const;
  inline void setParams(const Double_t dx,const Double_t dz, const Double_t c);
  void printTo(ostream &out) { out << fDx << fDz << fC << endl; }
  void readFrom(istream &in) { in >> fDx >> fDz >> fC; }
  ClassDef(HSurfXCilinder,0)
};

//******* Inlines

void HSurfPlane::setParams(const Double_t dx,const Double_t dz,
			   const Double_t c) {
  fDx=dx; fC=c; fDz=dz;
}

void HSurfXCilinder::setParams(const Double_t dx,const Double_t dz,
			       const Double_t c) {
  fDx=dx; fC=c; fDz=dz;
}

#endif