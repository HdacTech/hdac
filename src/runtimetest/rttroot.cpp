#include "rttroot.h"

#include <iostream>
#include <boost/lexical_cast.hpp>

using std::string;
using std::cout;
using std::endl;
using std::cin;
using std::unique_ptr;

AbstractRunTimeTest* AbstractRunTimeTest::sCurrentTest = nullptr;

AbstractRunTimeTest::AbstractRunTimeTest() {
    if (sCurrentTest == nullptr)    {
        sCurrentTest = this;
    }
}

void AbstractRunTimeTest::addChild(unique_ptr<AbstractRunTimeTest> &child) {
    child->setParent(this);
    mChildren.push_back(std::move(child));
}

const AbstractRunTimeTest *AbstractRunTimeTest::parent() const {
    return mParent;
}

AbstractRunTimeTest &AbstractRunTimeTest::current() {
    return *sCurrentTest;
}

void AbstractRunTimeTest::setParent(AbstractRunTimeTest *parent) {
    mParent = parent;
}

void AbstractRunTimeTest::changeCurrentTest(AbstractRunTimeTest *test) {
    sCurrentTest = test;
}

const string RTTRoot::title() const {
    return "Root Runtime Test";
}

bool RTTRoot::chooseMenu() {
    displayMenu();

    string in;
    cin >> in;
    bool isQuit = false;
    if (!runDefaultRTT(in[0], isQuit))   {
        int number = boost::lexical_cast<int>(in);
        isQuit = runRunTimeTest(number);
    }
    return isQuit;
}

bool RTTRoot::runRunTimeTest(int index) {
    cout << "You select : " << index << ", " << __FUNCTION__ << endl;

    return true;
}

void RTTRoot::displayMenu() {
    cout << "-------------------------------------" << endl;
    cout << title() << endl;
    cout << "-------------------------------------" << endl;
    displaySelfMenu();
    for (int i = 0; i < mChildren.size(); i++)  {
        cout << static_cast<char>('a'+i) << ". " << mChildren[i]->title() << endl;
    }
    cout << "-------------------------------------" << endl;
    if (mParent) {
        cout << "E. Exit" << endl;
    }
    cout << "Q. Quit" << endl;
    cout << "-------------------------------------" << endl;
}

bool RTTRoot::selectChildMenu(int index) {
    if (index < mChildren.size())   {
        changeCurrentTest(mChildren[index].get());
    }
    return true;
}

bool RTTRoot::handleEtcMenu(int index) {
    if (index == 'Q' - 'A') {
        return false;
    }
    if (index == 'E' - 'A') {
        if (mParent)    {
            changeCurrentTest(mParent);
        }
    }
    return true;
}

bool RTTRoot::runDefaultRTT(char indexHead, bool &isContinue) {
    if (indexHead >= 'A' && indexHead <= 'Z')   {
        isContinue = handleEtcMenu(indexHead - 'A');
        return true;
    }
    else if (indexHead >= 'a' && indexHead <= 'z') {
        isContinue = selectChildMenu(indexHead - 'a');
        return true;
    }

    return false;
}
