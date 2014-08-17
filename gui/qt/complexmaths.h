#ifndef COMPLEXMATHS_H
#define COMPLEXMATHS_H

class ComplexMaths
{
public:
  ComplexMaths();
  void cxInvert(double R, double I, double &Rres, double &Ires);
  void cxDivide(double Rnum, double Inum, double Rden, double Iden, double &Rres, double &Ires);
};

#endif // COMPLEXMATHS_H
