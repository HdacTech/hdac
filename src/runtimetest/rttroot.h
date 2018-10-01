#ifndef ROOTRTT_H
#define ROOTRTT_H

#include <string>
#include <memory>
#include <vector>

class AbstractRunTimeTest
{
public:
    AbstractRunTimeTest();
    virtual bool chooseMenu() = 0;
    virtual bool runRunTimeTest(int index) = 0;
    virtual const std::string title() const = 0;
    virtual void addChild(std::unique_ptr<AbstractRunTimeTest> &child);
    virtual const AbstractRunTimeTest* parent() const;

    static AbstractRunTimeTest& current();

protected:
    void setParent(AbstractRunTimeTest *parent);

    virtual void changeCurrentTest(AbstractRunTimeTest *test);

protected:
    std::vector<std::unique_ptr<AbstractRunTimeTest>>mChildren;
    AbstractRunTimeTest* mParent = nullptr;

private:
    static AbstractRunTimeTest* sCurrentTest;
};



class RTTRoot : public AbstractRunTimeTest
{
public:
    const std::string title() const;

    bool chooseMenu();

    bool runRunTimeTest(int index);

protected:
    virtual void displayMenu();

    virtual void displaySelfMenu() {

    }

    bool selectChildMenu(int index);

    bool handleEtcMenu(int index);

    bool runDefaultRTT(char indexHead, bool &isContinue);
};

#endif // ROOTRTT_H
