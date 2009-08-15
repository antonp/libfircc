#ifndef _BASEDEFS_H_
#define _BASEDEFS_H_

#ifndef NULL
  #define NULL 0
#endif

namespace firc
{

#ifdef WINDOWS_32
  typedef long long				int64;
  typedef unsigned long long	uint64;
  typedef int					int32;
  typedef unsigned int			uint32;
  typedef short					int16;
  typedef unsigned short		uint16;
  typedef char					int8;
  typedef unsigned char			uint8;

  typedef uint32				bool32;
  typedef uint8					bool8;
  
  typedef int32					word;
  typedef uint32				uword;
#endif

#ifdef TARGET_UBUNTU64
  typedef long long				int64;
  typedef unsigned long long	uint64;
  typedef int					int32;
  typedef unsigned int			uint32;
  typedef short					int16;
  typedef unsigned short		uint16;
  typedef char					int8;
  typedef unsigned char			uint8;

  typedef uint32				bool32;
  typedef uint8					bool8;
  
  typedef int64					word;
  typedef uint64				uword;
#endif

//#define MGE_TRUE 1
//#define MGE_FALSE 0
enum
{
	FALSE = 0,
	TRUE = 1
};

#ifdef FIRC_UNICODE
  #define DCHAR unsigned short
//  typedef unsigned short		DCHAR;
#else
  #define DCHAR char
//  typedef char					DCHAR;
#endif

enum Result
{
	RES_OK 				= 0,
	RES_FAILED,
	RES_INVALID_PARAMETER,
	RES_NOTFOUND,
	RES_OUTOFBOUNDS,
	RES_INTERNALERROR,
	RES_MEMALLOC_FAILED,
	RES_CONNECTION_CLOSED,
	RES_BUFFER_TOO_SMALL
};

} // namespace mge

#define FIRC_CLEANUP_LABEL mgeCleanUpLabel1:
#define FIRC_GOTO_CLEANUP_LABEL goto mgeCleanUpLabel1
#define FIRC_GOTO_CLEANUP_LABEL_IF(expr) if ( expr ) { MGE_GOTO_CLEANUP_LABEL; }
#define FIRC_REQUIRE(expr, resVar) if ( !(expr) ) { resVar = mge::RES_INVALID_PARAMETER; MGE_GOTO_CLEANUP_LABEL; }
#define FIRC_REQUIRE_DIRECT(expr) if ( !(expr) ) { return mge::RES_INVALID_PARAMETER; }

#define FIRC_DELETE(obj) if ( obj != NULL ) { delete obj; obj = NULL; }
#define FIRC_DELETE_ARRAY(obj) if ( obj != NULL ) { delete[] obj; obj = NULL; }

#endif // _BASEDEFS_H_

