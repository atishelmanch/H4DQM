#ifndef BRD_H
#define BRD_H

// #include "interface/EventBuilder.hpp"
// #include "interface/DataType.hpp"
#include "interface/StandardIncludes.hpp"
#include "interface/Event.hpp"

typedef ifstream dataType;

#define _MAXBOARDTYPE_ 9

enum BoardTypes_t { _TIME_=1, //Fake board to get timestamps
		    _CAENVX718_=2, //Crate Controller
		    _CAENV1742_=3, //5Gs Digitiser
		    _CAENV513_=4, //I/O Register
		    _CAENV262_=5, //I/O Register
		    _CAENV792_=6, //32ch Charge ADC
		    _CAENV1290_=7, //16ch TDC
		    _CAENV1495PU_=8, //General Purpose I/O. FW 2.1 PatternUnit
		    _CAENV560_=9, //Scaler
		    _UNKWN_=0 };


class Board {

public:

  virtual int Unpack(dataType &stream)=0;
  virtual int Unpack (dataType &stream, Event * event) 
    {
    
      WORD currWord = 0 ;
      WORD boardTrailerValue = *((uint32_t*)"BRDT");
      int nWords = 0 ;
      while (currWord != boardTrailerValue)
        {
          stream.read ((char*)&currWord, WORDSIZE);
          ++nWords ;
        }
      cout << "[Board][Unpack]            | dummy reading of " << nWords << " words\n" ; 
      cout << "[Board][Unpack]            | is last word trailer " 
           << (currWord == boardTrailerValue) << "\n" ;
      stream.seekg (-1 * WORDSIZE, ios::cur) ;

      return -1 ; 
    } ; 

  int GetType(dataType &stream);

};

#endif
