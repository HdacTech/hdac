
#include <string>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <storage/txdb.h>
#include <memory>
#include "rttroot.h"
#include "blocktreedbrtt.h"
#include "epowrtt.h"
#include "chainrtt.h"

using std::unique_ptr;
using std::string;

void doRunTimeTest()
{
    unique_ptr<AbstractRunTimeTest> rttFirst(new RTTRoot);
    unique_ptr<AbstractRunTimeTest> blockTreeDBTest(new BlockTreeDBRTT);
    unique_ptr<AbstractRunTimeTest> epowTest(new EpowDBRTT);
    unique_ptr<AbstractRunTimeTest> chainTest(new ChainRTT);
    //IRunTimeTest &root = *rttFirst;
    rttFirst->addChild(blockTreeDBTest);
    rttFirst->addChild(epowTest);
    rttFirst->addChild(chainTest);

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
