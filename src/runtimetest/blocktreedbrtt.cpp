#include "blocktreedbrtt.h"
#include <iostream>
#include <storage/txdb.h>
#include <core/main.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;

const string BlockTreeDBRTT::title() const {
    return "BlockTreeDB Runtime Test";
}

bool BlockTreeDBRTT::runRunTimeTest(int index) {
    cout << "You select : " << index << ", " << __FUNCTION__ << endl;
    cout << endl;

    switch (index) {
    case 1:
        int readFlagIndex = chooseReadFlagMenu();
        handleReadFlag(readFlagIndex);
        //cout << "result : " << ((result == true) ? "ON" : "OFF") << endl;
        break;
    }

    return true;
}

void BlockTreeDBRTT::displaySelfMenu() {
    cout << "1. ReadFlag" << endl;
}

int BlockTreeDBRTT::chooseReadFlagMenu() {
    cout << "-------------------------------------" << endl;
    cout << title() << "- ReadFlag" << endl;
    cout << "-------------------------------------" << endl;
    cout << "1. addressindex" << endl;
    cout << "2. spentindex" << endl;
    cout << "3. timestampindex" << endl;
    cout << "4. txindex" << endl;
    cout << "-------------------------------------" << endl;

    int readFlagIndex;
    cin >> readFlagIndex;
    return readFlagIndex;
}

void BlockTreeDBRTT::handleReadFlag(int readFlagIndex) {
    bool flag;
    switch(readFlagIndex) {
    case 1:
        pblocktree->ReadFlag("addressindex", flag);
        break;
    case 2:
        pblocktree->ReadFlag("spentindex", flag);
        break;
    case 3:
        pblocktree->ReadFlag("timestampindex", flag);
        break;
    case 4:
        pblocktree->ReadFlag("txindex", flag);
        break;
    }
    cout << "result : " << ((flag == true) ? "ON" : "OFF") << endl;
}
