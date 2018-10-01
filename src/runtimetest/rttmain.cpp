
#include <string>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <storage/txdb.h>
#include <memory>
#include "rttroot.h"
#include "blocktreedbrtt.h"

using std::unique_ptr;
using std::string;



void doRunTimeTest()
{
    unique_ptr<AbstractRunTimeTest> rttFirst(new RTTRoot);
    unique_ptr<AbstractRunTimeTest> blockTreeDBTest(new BlockTreeDBRTT);
    //IRunTimeTest &root = *rttFirst;
    rttFirst->addChild(blockTreeDBTest);

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
