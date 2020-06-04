#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <memory>
#include <functional>
#include <stdexcept>
#endif /* __PROGTEST */
using namespace std;

#ifndef __PROGTEST__
class CTimeStamp
{
	private:
		int year = 0, month = 0, day = 0, hour = 0, minute = 0, sec = 0;
  public:
                   CTimeStamp                              ( int               year,
                                                             int               month,
                                                             int               day,
                                                             int               hour,
                                                             int               minute,
                                                             int               sec ){
      this->year = year;
      this->month = month;
      this->day = day;
      this->hour = hour;
      this->minute = minute;
      this->sec = sec;
     }
    	

    int            Compare                                 ( const CTimeStamp & x ) const{
    	if(year < x.year) return -1; else if(year > x.year) return 1;
    	if(month < x.month) return -1; else if(month > x.month) return 1;
    	if(day < x.day) return -1; else if(day > x.day) return 1;
    	if(hour < x.hour) return -1; else if(hour > x.hour) return 1;
    	if(minute < x.minute) return -1; else if(minute > x.minute) return 1;
    	if(sec < x.sec) return -1; else if(sec > x.sec) return 1;
    	return 0;
    }
    friend ostream & operator <<                           ( ostream          & os,
                                                             const CTimeStamp & x ){
      os <<  x.year << "-" << setfill('0') << setw(2) << x.month << "-" << setw(2) << setfill('0') << x.day;
      os << " " << setfill('0') << setw(2) << x.hour << ":" << setfill('0') << setw(2) << x.minute << ":" << setfill('0') << setw(2) <<x.sec;
      return os;
    }

};
//=================================================================================================
class CMailBody
{
  private:
    int            m_Size;
    char         * m_Data;
  public:
                   CMailBody                               ( int               size,
                                                             const char      * data ){
     		m_Size = size;
     		if(size){
					m_Data = new char[size];
					memcpy(m_Data, data, size);
				}
				else{
					m_Data = nullptr;
				}
     	}

     CMailBody(const CMailBody & other){
     	 m_Size = other.m_Size;
     	 m_Data = new char[m_Size];
     }

     CMailBody & operator = (CMailBody other){
     	 swap(other.m_Size, m_Size);
     	 swap(other.m_Data, m_Data);
     	 return *this;
     }

     // copy cons/op=/destructor is correctly implemented in the testing environment
    friend ostream & operator <<                           ( ostream         & os,
                                                             const CMailBody & x )
    {
      return os << "mail body: " << x . m_Size << " B";
    }

    ~CMailBody(){
    	delete [] m_Data;
    }
};
//=================================================================================================
class CAttach
{
  public:
                   CAttach                                 ( int               x )
      : m_X (x),
        m_RefCnt ( 1 )
    {
    }
    void           AddRef                                  ( void ) const 
    { 
      m_RefCnt ++; 
    }
    void           Release                                 ( void ) const 
    { 
      if ( !--m_RefCnt ) 
        delete this; 
    }
  private:
    int            m_X;
    mutable int    m_RefCnt;
                   CAttach                                 ( const CAttach   & x );
    CAttach      & operator =                              ( const CAttach   & x );
                  ~CAttach                                 ( void ) = default;
    friend ostream & operator <<                           ( ostream         & os,
                                                             const CAttach   & x )
    {
      return os << "attachment: " << x . m_X << " B";
    }
};
//=================================================================================================
#endif /* __PROGTEST__, DO NOT remove */



class CMail
{
  private:
  	 string m_from;
  	 CMailBody m_body;
  	 CTimeStamp m_timeStamp;
  	const CAttach* m_attach;

  public:
   CMail ( const CTimeStamp & timeStamp, const string     & from, const CMailBody  & body, const CAttach    * attach ): m_from(from),m_body(body), m_timeStamp(timeStamp),m_attach(attach) {
   	 if(m_attach){
   	 	 m_attach->AddRef();
   	 }
   }

		CMail (const CMail & other):m_from(other.m_from),m_body(other.m_body), m_timeStamp(other.m_timeStamp),m_attach(other.m_attach){ 
			if(m_attach)
				m_attach->AddRef();
		}

		CMail (CMail && other)noexcept : m_from(move(other.m_from)), m_body(move(other.m_body)), m_timeStamp(move(other.m_timeStamp)), m_attach(other.m_attach) {
			other.m_attach = nullptr;	 cout <<111 << endl;
		}

		CMail & operator = ( CMail && other) noexcept {
			if(&other == this)
				return *this;
			swap(m_from, other.m_from);
			swap(m_timeStamp, other.m_timeStamp);
			m_body = move(other.m_body);	
			m_attach = other.m_attach;
			other.m_attach = nullptr;
			return *this;
		}

	 CMail & operator = (const CMail & other){
	 	 cout << 11111 << endl;
	 	 if(&other == this)
	 	 	 return *this;
	 	 if(m_attach)
			 m_attach->Release();
	 	 m_attach = other.m_attach;
	 	 if(m_attach)
			 m_attach->AddRef();
	 	 m_from = other.m_from;
	 	 m_timeStamp = other.m_timeStamp;
	 	 m_body = other.m_body;
	 	 return *this;
	 }

    const string     & From                                ( void ) const{
    	return m_from;
    }
    const CMailBody  & Body                                ( void ) const{
    	return m_body;
    }
    const CTimeStamp & TimeStamp                           ( void ) const{
    	return m_timeStamp;
    }
    const CAttach* Attachment                              ( void ) const{
    	if(m_attach)
				m_attach->AddRef();
    	return m_attach;
    }
    ~CMail(){
    	if(m_attach)
				m_attach->Release();
    }
    friend ostream & operator <<                           ( ostream          & os,
                                                             const CMail      & x ){
      os << x.m_timeStamp << " " << x.m_from << " " << x.m_body;
      if(x.m_attach)
      	os << " + " << *(x. m_attach);
      return os;
    }
};
struct CmpMailTimeLower{
	bool operator ()(const CMail & cmp1, const CMail & cmp2) const{
		return cmp1.TimeStamp().Compare(cmp2.TimeStamp()) < 0;
	}
};
//=================================================================================================
class CMailBox
{
  private:
  	map<string, multiset<CMail, CmpMailTimeLower>> folders;

  public:
    CMailBox (void){
    	folders.insert({"inbox", {}});
    }
    bool Delivery ( const CMail & mail ){
    	multiset <CMail, CmpMailTimeLower> & r = folders["inbox"];
    	r.insert(mail);
    	return true;
    }

    bool NewFolder ( const string & folderName ){
    	pair<map<string, multiset<CMail, CmpMailTimeLower>>::iterator, bool> r;
    	r = folders.insert({folderName, {}});
    	if(!r.second)
    		return false;
    	return true;
    }

    bool MoveMail ( const string  & fromFolder, const string   & toFolder ) noexcept{
    	multiset<CMail, CmpMailTimeLower> newFolder;
    	auto from = folders.find(fromFolder);
    	auto to = folders.find(toFolder);
    	if(to == folders.end() || from == folders.end())
    		return false;
    	auto it1 = from->second.begin();
    	auto it2 = to->second.begin();
    	while(it1 != from->second.end() && it2 != to->second.end()){
    		if(CmpMailTimeLower()(*it1, *it2)){
    				auto it_tmp = it1;
    				++it_tmp;
    				auto er = from->second.extract(it1);
    				it1 = it_tmp;
					newFolder.insert(newFolder.end(), move(er));
				}else{
    				auto it_tmp = it2;
    				++it_tmp;
    				auto er = to->second.extract(it2);
    				it2 = it_tmp;
					newFolder.insert(newFolder.end(), move(er));
				}
			}
			while(it1 != from->second.end()){
				auto it_tmp = it1;
				++it_tmp;
				auto er = from->second.extract(it1);
				it1 = it_tmp;
				newFolder.insert(newFolder.end(), move(er));
			}
			while(it2 != to->second.end()){
				auto it_tmp = it2;
				++it_tmp;
				auto er = to->second.extract(it2);
				it2 = it_tmp;
				newFolder.insert(newFolder.end(), move(er));
			}
			to->second = newFolder;
			return true;

    }

    list<CMail>    ListMail ( const string     & folderName, const CTimeStamp & from,
                                                             const CTimeStamp & to ) const{
    	auto fromF = folders.find(folderName);
    	list<CMail> returnList;
    	if(fromF == folders.end())
    		return returnList;

			CMailBody tmpBody(0, "");
    	CMail fromMail(from,"", tmpBody, nullptr);
    	CMail toMail(to,"", tmpBody, nullptr);
    	auto start = fromF->second.lower_bound(fromMail);
    	auto end = fromF->second.upper_bound(toMail);
    	if(start == end)
    		return returnList;

    	copy(start, end, back_inserter(returnList));
			return returnList;
     }

    set<string>    ListAddr                                ( const CTimeStamp & from,
                                                             const CTimeStamp & to ) const{
			CMailBody tmpBody(0, "");
    	CMail fromMail(from,"", tmpBody, nullptr);
    	CMail toMail(to,"", tmpBody, nullptr);
    	set<string> returnSet;
      for(auto it = folders.begin(); it != folders.end(); ++it){
				auto start = it->second.lower_bound(fromMail);
				auto end = it->second.upper_bound(toMail);
      	for(; start != end; ++start){
      		returnSet.insert(start->From());
      	}
      }
      return returnSet;
		}
};
//=================================================================================================
#ifndef __PROGTEST__
static string showMail ( const list<CMail> & l )
{
  ostringstream oss;
  for ( const auto & x : l )
    oss << x << endl;
  return oss . str ();
}
static string showUsers ( const set<string> & s )
{
  ostringstream oss;
  for ( const auto & x : s )
    oss << x << endl;
  return oss . str ();
}
int main ( void )
{
  list<CMail> mailList;
  set<string> users;
  CAttach   * att;

  CMailBox m0;
  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 15, 24, 13 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 1" ), nullptr ) ) );
  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 15, 26, 23 ), "user2@fit.cvut.cz", CMailBody ( 22, "some different content" ), nullptr ) ) );
  att = new CAttach ( 200 );
  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 11, 23, 43 ), "boss1@fit.cvut.cz", CMailBody ( 14, "urgent message" ), att ) ) );
  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 18, 52, 27 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 2" ), att ) ) );
  att -> Release ();
  att = new CAttach ( 97 );
  assert ( m0 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 16, 12, 48 ), "boss1@fit.cvut.cz", CMailBody ( 24, "even more urgent message" ), att ) ) );
  att -> Release ();

  assert ( showMail ( m0 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B + attachment: 200 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B + attachment: 97 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 200 B\n" );
  assert ( showMail ( m0 . ListMail ( "inbox",
                      CTimeStamp ( 2014, 3, 31, 15, 26, 23 ),
                      CTimeStamp ( 2014, 3, 31, 16, 12, 48 ) ) ) == "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B + attachment: 97 B\n" );
  assert ( showUsers ( m0 . ListAddr ( CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                       CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "boss1@fit.cvut.cz\n"
                        "user1@fit.cvut.cz\n"
                        "user2@fit.cvut.cz\n" );
  assert ( showUsers ( m0 . ListAddr ( CTimeStamp ( 2014, 3, 31, 15, 26, 23 ),
                       CTimeStamp ( 2014, 3, 31, 16, 12, 48 ) ) ) == "boss1@fit.cvut.cz\n"
                        "user2@fit.cvut.cz\n" );

  CMailBox m1;
  assert ( m1 . NewFolder ( "work" ) );
  assert ( m1 . NewFolder ( "spam" ) );
  assert ( !m1 . NewFolder ( "spam" ) );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 15, 24, 13 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 1" ), nullptr ) ) );
  att = new CAttach ( 500 );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 15, 26, 23 ), "user2@fit.cvut.cz", CMailBody ( 22, "some different content" ), att ) ) );
  att -> Release ();
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 11, 23, 43 ), "boss1@fit.cvut.cz", CMailBody ( 14, "urgent message" ), nullptr ) ) );
  att = new CAttach ( 468 );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 18, 52, 27 ), "user1@fit.cvut.cz", CMailBody ( 14, "mail content 2" ), att ) ) );
  att -> Release ();
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 16, 12, 48 ), "boss1@fit.cvut.cz", CMailBody ( 24, "even more urgent message" ), nullptr ) ) );
  assert ( showMail ( m1 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n" );
  assert ( showMail ( m1 . ListMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "" );
  assert ( m1 . MoveMail ( "inbox", "work" ) );
  assert ( showMail ( m1 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "" );
  assert ( showMail ( m1 . ListMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n" );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 19, 24, 13 ), "user2@fit.cvut.cz", CMailBody ( 14, "mail content 4" ), nullptr ) ) );
  att = new CAttach ( 234 );
  assert ( m1 . Delivery ( CMail ( CTimeStamp ( 2014, 3, 31, 13, 26, 23 ), "user3@fit.cvut.cz", CMailBody ( 9, "complains" ), att ) ) );
  att -> Release ();
  assert ( showMail ( m1 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 13:26:23 user3@fit.cvut.cz mail body: 9 B + attachment: 234 B\n"
                        "2014-03-31 19:24:13 user2@fit.cvut.cz mail body: 14 B\n" );
  assert ( showMail ( m1 . ListMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n" );
  assert ( m1 . MoveMail ( "inbox", "work" ) );
  assert ( showMail ( m1 . ListMail ( "inbox",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "" );
  assert ( showMail ( m1 . ListMail ( "work",
                      CTimeStamp ( 2000, 1, 1, 0, 0, 0 ),
                      CTimeStamp ( 2050, 12, 31, 23, 59, 59 ) ) ) == "2014-03-31 11:23:43 boss1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 13:26:23 user3@fit.cvut.cz mail body: 9 B + attachment: 234 B\n"
                        "2014-03-31 15:24:13 user1@fit.cvut.cz mail body: 14 B\n"
                        "2014-03-31 15:26:23 user2@fit.cvut.cz mail body: 22 B + attachment: 500 B\n"
                        "2014-03-31 16:12:48 boss1@fit.cvut.cz mail body: 24 B\n"
                        "2014-03-31 18:52:27 user1@fit.cvut.cz mail body: 14 B + attachment: 468 B\n"
                        "2014-03-31 19:24:13 user2@fit.cvut.cz mail body: 14 B\n" );

  return 0;
}
#endif /* __PROGTEST__ */
