#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
using namespace std;
#endif /* __PROGTEST__ */

class CBigInt
{
  public:
    // default constructor
    // copying/assignment/destruction
    // int constructor
    // string constructor
    // operator +, any combination {CBigInt/int/string} + {CBigInt/int/string}
    // operator *, any combination {CBigInt/int/string} * {CBigInt/int/string}
    // operator +=, any of {CBigInt/int/string}
    // operator *=, any of {CBigInt/int/string}
    // comparison operators, any combination {CBigInt/int/string} {<,<=,>,>=,==,!=} {CBigInt/int/string}
    // output operator <<
    // input operator >>
  private:
    // todo
};

#ifndef __PROGTEST__
static bool equal ( const CBigInt & x, const char * val )
{
  ostringstream oss;
  oss << x;
  return oss . str () == val;
}
int main ( void )
{
  CBigInt a, b;
  istringstream is;
  a = 10;
  a += 20;
  assert ( equal ( a, "30" ) );
  a *= 5;
  assert ( equal ( a, "150" ) );
  b = a + 3;
  assert ( equal ( b, "153" ) );
  b = a * 7;
  assert ( equal ( b, "1050" ) );
  assert ( equal ( a, "150" ) );

  a = 10;
  a += -20;
  assert ( equal ( a, "-10" ) );
  a *= 5;
  assert ( equal ( a, "-50" ) );
  b = a + 73;
  assert ( equal ( b, "23" ) );
  b = a * -7;
  assert ( equal ( b, "350" ) );
  assert ( equal ( a, "-50" ) );

  a = "12345678901234567890";
  a += "-99999999999999999999";
  assert ( equal ( a, "-87654321098765432109" ) );
  a *= "54321987654321987654";
  assert ( equal ( a, "-4761556948575111126880627366067073182286" ) );
  a *= 0;
  assert ( equal ( a, "0" ) );
  a = 10;
  b = a + "400";
  assert ( equal ( b, "410" ) );
  b = a * "15";
  assert ( equal ( b, "150" ) );
  assert ( equal ( a, "10" ) );

  is . clear ();
  is . str ( " 1234" );
  assert ( is >> b );
  assert ( equal ( b, "1234" ) );
  is . clear ();
  is . str ( " 12 34" );
  assert ( is >> b );
  assert ( equal ( b, "12" ) );
  is . clear ();
  is . str ( "999z" );
  assert ( is >> b );
  assert ( equal ( b, "999" ) );
  is . clear ();
  is . str ( "abcd" );
  assert ( ! ( is >> b ) );
  is . clear ();
  is . str ( "- 758" );
  assert ( ! ( is >> b ) );
  try
  {
    a = "-xyz";
    assert ( "missing an exception" == NULL );
  }
  catch ( const invalid_argument & e )
  {
  }

  a = "73786976294838206464";
  assert ( a < "1361129467683753853853498429727072845824" );
  assert ( a <= "1361129467683753853853498429727072845824" );
  assert ( ! ( a > "1361129467683753853853498429727072845824" ) );
  assert ( ! ( a >= "1361129467683753853853498429727072845824" ) );
  assert ( ! ( a == "1361129467683753853853498429727072845824" ) );
  assert ( a != "1361129467683753853853498429727072845824" );
  assert ( ! ( a < "73786976294838206464" ) );
  assert ( a <= "73786976294838206464" );
  assert ( ! ( a > "73786976294838206464" ) );
  assert ( a >= "73786976294838206464" );
  assert ( a == "73786976294838206464" );
  assert ( ! ( a != "73786976294838206464" ) );
  assert ( a < "73786976294838206465" );
  assert ( a <= "73786976294838206465" );
  assert ( ! ( a > "73786976294838206465" ) );
  assert ( ! ( a >= "73786976294838206465" ) );
  assert ( ! ( a == "73786976294838206465" ) );
  assert ( a != "73786976294838206465" );
  a = "2147483648";
  assert ( ! ( a < -2147483648 ) );
  assert ( ! ( a <= -2147483648 ) );
  assert ( a > -2147483648 );
  assert ( a >= -2147483648 );
  assert ( ! ( a == -2147483648 ) );
  assert ( a != -2147483648 );

  return 0;
}
#endif /* __PROGTEST__ */
