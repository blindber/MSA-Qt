#ifndef VECTORTYPES_H
#define VECTORTYPES_H

template <class T>
class Q2DVector : public QVector< QVector<T> >
{
  public:
    Q2DVector() : QVector< QVector<T> >(){}

    Q2DVector(int rows, int columns) : QVector< QVector<T> >(rows)
    {
      for(int r=0; r<this->size(); r++)
      {
        (*this)[r].resize(columns);
      }
    }
    void mresize(int rows, int columns)
    {
      this->resize(rows);
      for(int r=0; r<this->size(); r++)
      {
        (*this)[r].resize(columns);
      }
    }
    inline Q2DVector &operator<<(const Q2DVector<T> &l)
    { *this += l; return *this; }
    virtual ~Q2DVector() {}
};
//typedef Q2DVector<bool> Q2DBoolVector;
typedef Q2DVector<float> Q2DfloatVector;
typedef Q2DVector<int> Q2DintVector;
//typedef Q2DVector<QString> Q2DQStringVector;


#endif // VECTORTYPES_H
