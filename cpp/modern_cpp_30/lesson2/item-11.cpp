#include <stdlib.h>

class Bitmap {};

class Widget {
 public:
 Widget& operator=(const Widget& rhs)
 {
  delete pb;
  pb = new Bitmap(*rhs.pb);
  return *this;
 }


 private:
  Bitmap *pb;
};

int main()
{
  Widget  *obj = new Widget();
  return 0;
}
