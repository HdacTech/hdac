
#include <string>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <storage/txdb.h>
#include <memory>
#include "rttroot.h"
#include "blocktreedbrtt.h"
#include "epowrtt.h"

using std::unique_ptr;
using std::string;



void doRunTimeTest()
{
    unique_ptr<AbstractRunTimeTest> rttFirst(new RTTRoot);
    unique_ptr<AbstractRunTimeTest> blockTreeDBTest(new BlockTreeDBRTT);
    unique_ptr<AbstractRunTimeTest> epowTest(new EpowDBRTT);
    //IRunTimeTest &root = *rttFirst;
    rttFirst->addChild(blockTreeDBTest);
    rttFirst->addChild(epowTest);

    do {
        try {
            AbstractRunTimeTest &root = AbstractRunTimeTest::current();
            if (root.chooseMenu() == false)  {
                break;
            }
        } catch (boost::thread_interrupted) {
            break;
        }
    } while (true);
}
