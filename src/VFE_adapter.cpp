/*  Data format
 *  -----------
 *   First 3 words: header, containing the timestamp
 *     information (i.e. the number of the 160 MHz clocks
 *     from the StartDAQ() command):
 *     timestamp = (t5<<56) + (t4<<42) + (t3<<28) + (t2<<14) + t1;
 *   Subsequent words: _nsamples for each of the 5 VFE channels
 *   Then concatenate header and samples for each VFE adapter
 *   read by the board.
 *
 *           32 ... 16 ...  0 -> bits
 *   words
 *           VFE_adapter_1
 *     1     [10...0][  t1  ] \
 *     2     [  t3  ][  t2  ]  |-> header
 *     3     [  t5  ][  t4  ] /
 *     4     [  --  ][ch0_s0] \
 *     5     [ch2_s0][ch1_s0]  |
 *     6     [ch4_s0][ch3_s0]  |
 *     7     [  --  ][ch0_s1]  |-> samples
 *     8     [ch2_s1][ch1_s1]  |
 *     9     [ch4_s1][ch3_s1]  |
 *    ...       ...    ...    ...
 *           VFE_adapter_N
 *    M+1    [10...0][  t1  ] \
 *    M+2    [  t3  ][  t2  ]  |-> header
 *    ...       ...    ...    ...
 */

#include "interface/VFE_adapter.hpp"
#include <bitset>

using namespace std ;

void Record_Sample(int, float, int);

int VFE_adapter::Unpack (dataType &stream, Event * event, boardHeader &bH)
{
    unsigned int nSamples_ = (bH.boardSize - 4) /3 ; //move to hpp like dig1742Words_
    int sampleValue ;
    unsigned int timestamp [5] ;
    short t = 2 ; //to set timestamp place
    for (unsigned int i = 0; i< 4; ++i) //this loop covers header
    {
        stream.read ((char*)&digRawData_, sizeof (digRawData_)) ;
        /* if (i==0)
        {
            long word_zero = digRawData_ & 0xFFFFFFF; //if there is a word 0, guessing based on size estimate (628 with samples taking 3 words means 4 word header not 3)
        } */
        if (i==1)
        {
            int dt_type = digRawData_>>16 & 0xF; // [10...0]
            timestamp [0] = digRawData_ ;
        }
        else if (i==2 or i==3)
        {
            timestamp [t] = digRawData_>>16 ;
            timestamp [t-1] = digRawData_ ;
            t +=2 ;
        }
    }
    for (int iSample = 0; iSample < nSamples_; ++ iSample)
    {
        for (short line = 0; line <3; ++ line)
        {
            stream.read ((char*)&digRawSample_, sizeof (digRawSample_)) ;

            if (line == 0)
            {
                int unknown = digRawSample_>>16 ; // [ -- ]
                channelId_ = 0 ;
                sampleValue = digRawSample_ ;
                Record_Sample(channelId_, sampleValue, iSample);
            }
            else
            {
                channelId_ = 2 * line ;
                sampleValue = digRawSample_ >>16 ;
                Record_Sample(channelId_, sampleValue, iSample);
                channelId_ = (2 * line) -1 ;
                sampleValue = digRawSample_ ;
                Record_Sample(channelId_, sampleValue, iSample);
            }
        }
    }
    return 0 ;
}

void Record_Sample (int channelId_, float samplevalue, int iSample)
{
/* block comment as not needed until storing data
    digiData aDigiSample ;
    //aDigiSample.board = bH.boardID ;
    aDigiSample.channel = channelId_ ;
    //aDigiSample.group = groupId_ ;
    //aDigiSample.frequency = frequency_ ;
    aDigiSample.sampleIndex = iSample ;
    aDigiSample.sampleValue = samplevalue ; this is for storage if using digidata type */

    cout << "VFE_adapter unpack       | sample " << iSample<< " channel: "<< channelId_ << " value: " << samplevalue << "\n" ;

    //event->digiValues.push_back (aDigiSample) ;
}
    
