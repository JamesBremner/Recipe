#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "cxy.h"
#include <wex.h>
#include <inputbox.h>
#include "flower.h"
#include "cVase.h"
#include "cRecipeGUI.h"

cRecipeGUI::cRecipeGUI()
    : fm(wex::maker::make())
{
    fm.move({50, 50, 1000, 500});
    fm.text("Recipe");

    menus();
    registerEventHandlers();

    init();

    fm.show();
    fm.run();
}

void cRecipeGUI::init()
{
    if (!myVase.Add("Source"))
        return;
    auto *f = myVase.getSelected();
    f->setLocationTopLeft(300, 20);
    f->setName("Start");
    if (!myVase.Add("Sink"))
        return;
    f = myVase.getSelected();
    f->setLocationTopLeft(100, 400);
    f->setName("Failed");
    if (!myVase.Add("Sink"))
        return;
    f = myVase.getSelected();
    f->setLocationTopLeft(500, 400);
    f->setName("Success");
}

void cRecipeGUI::menus()
{
    wex::menubar mb(fm);

    myFileMenu = new wex::menu(fm);
    myFileMenu->append(
        "New",
        [&](const std::string &title)
        {
            myVase.clear();
            init();
            fm.update();
        });
    myFileMenu->append(
        "Open",
        [&](const std::string &title)
        {
            wex::filebox fb(fm);
            auto fn = fb.open();
            if (fn.empty())
                return;
            myVase.Read(fn);
            fm.update();
        });
    myFileMenu->append(
        "Save",
        [&](const std::string &title)
        {
            wex::filebox fb(fm);
            auto fn = fb.save();
            if (fn.empty())
                return;
            myVase.Write(fn);
        });
    mb.append("File", *myFileMenu);

    myModeMenu = new wex::menu(fm);
    myModeMenu->append(
        "Design",
        [&](const std::string &title)
        {
            myVase.setMode(raven::sim::gui::cVase::e_mode::design);
        });
    myModeMenu->append(
        "Run",
        [&](const std::string &title)
        {
            startRun();
        });
    mb.append("Mode", *myModeMenu);
}

void cRecipeGUI::registerEventHandlers()
{
    fm.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            draw(S);
        });

    fm.events().click(
        [&]()
        {
            SelectFlower();
            fm.update();
        });

    fm.events().clickRight(
        [&]()
        {
            onRightClick();
            fm.update();
        });

    fm.events().mouseMove(
        [&](wex::sMouse &m)
        {
            // if (!m.left)
            //     return;
            if (!m.shift)
                return;
            if (!myVase.IsSelected())
                return;
            myVase.getSelected()->setLocationCenter(m.x, m.y);
            fm.update();
        });
}

void cRecipeGUI::onRightClick()
{
    auto ms = fm.getMouseStatus();
    auto *clickedflower = myVase.find(ms.x, ms.y);
    if (clickedflower == nullptr)
    {
        // clicked on empty canvas, construct new flower there
        ConstructFlower();
        return;
    }
    auto *selectedflower = myVase.getSelected();
    if (selectedflower)
        if (clickedflower != selectedflower)
        {
            // clicked on flower other than selected
            // connected selected flower to clicked flower
            selectedflower->Connect(clickedflower);
            return;
        }

    // clicked on selected flower

    wex::menu m(fm);
    m.append("Rename",
             [&](const std::string &title)
             {
                 rename();
                 fm.update();
             });
    m.append("Configure",
             [&](const std::string &title)
             {
                 config();
                 fm.update();
             });
    m.append("Delete selected",
             [&](const std::string &title)
             {
                 myVase.Delete();
                 fm.update();
             });
    m.append("Delete outputs",
             [&](const std::string &title)
             {
                 myVase.getSelected()->deleteOutputConnections();
                 fm.update();
             });

    m.popup(ms.x, ms.y);
    return;
}

void cRecipeGUI::rename()
{
    // prompt user to change the name of the selected flower
    auto *selectedflower = myVase.getSelected();
    wex::inputbox ib(fm);
    ib.gridWidth(400);
    ib.add("Name", selectedflower->getName());
    ib.showModal();

    // rename the flower with value entered into inputbox
    selectedflower->setName(ib.value("Name"));
}

void cRecipeGUI::draw(wex::shapes &S)
{
    S.textHeight(12);
    for (raven::sim::gui::cFlower *flower : myVase)
    {
        if (flower->isSelected())
            S.color(0x0000FF);
        else
            S.color(0);

        flower->draw(S);

        auto *dstFlower = flower->getDestination();
        if (dstFlower)
        {
            S.color(0xFF0000);
            int xep, yep, xdst, ydst;
            flower->locationExitPort1(xep, yep);
            dstFlower->getEntryPort(xdst, ydst);
            drawArrow(S, cxy(xep, yep), cxy(xdst, ydst));
        }
        dstFlower = flower->getDestination2();
        if (dstFlower)
        {
            S.color(0xFF0000);
            cxy entryPort;
            int xep, yep;
            flower->locationExitPort2(xep, yep);
            entryPort.x = dstFlower->getLocationX() + 25;
            entryPort.y = dstFlower->getLocationY();
            drawArrow(S, cxy(xep, yep), entryPort);
        }
    }
}

void cRecipeGUI::drawArrow(
    wex::shapes &S,
    const cxy &exit,
    const cxy &entry)
{
    const int nWidth = 10; // width (in pixels) of the full base of the arrowhead

    cxy arrow[5];
    cxy vecLine;
    cxy baseLine;

    // set base and tip
    arrow[4] = exit;
    arrow[0] = entry;

    // build line vector in arrow head
    vecLine = exit.vect(entry);
    vecLine.zoom(nWidth / sqrt(exit.dist2(entry)));

    // build vector between arrow tips - normal to the line
    baseLine.x = -vecLine.y;
    baseLine.y = vecLine.x;

    arrow[1] = arrow[0] - vecLine;
    arrow[2] = arrow[1] + baseLine;
    arrow[3] = arrow[1] - baseLine;

    S.line(arrow[4], arrow[0]);
    S.line(arrow[0], arrow[2]);
    S.line(arrow[0], arrow[3]);
}

void cRecipeGUI::ConstructFlower()
{
    static wex::sMouse ms;
    ms = fm.getMouseStatus();
    // std::cout << "mouse at " << ms.x << " " << ms.y << "\n";
    if (!myVase.Add("Decision"))
        return;
    myVase.getSelected()->setLocationTopLeft(ms.x, ms.y);
    fm.update();
}

void cRecipeGUI::SelectFlower()
{
    auto ms = fm.getMouseStatus();
    auto *flower = myVase.find(ms.x, ms.y);
    if (flower == nullptr)
        return;

    myVase.setSelected(flower);
}

void cRecipeGUI::config()
{
    wex::inputbox ib(fm);
    ib.text("Configure " +
            myVase.getSelected()->getName());

    for (auto &prm : myVase.getSelected()->myParam)
    {
        ib.add(
            prm.second.name,
            std::to_string(prm.second.value));
    }

    ib.showModal();

    for (auto &prm : myVase.getSelected()->myParam)
    {
        prm.second.value = atof(
            ib.value(prm.second.name).c_str());
    }
}
void cRecipeGUI::startRun()
{
    myVase.setMode(raven::sim::gui::cVase::e_mode::run);
    auto *f = myVase.find("Start");
    if (!f)
    {
        wex::msgbox("Start missing");
        myVase.setMode(raven::sim::gui::cVase::e_mode::design);
        return;
    }
    f->getDestination();
    if (!f)
    {
        wex::msgbox("Start not connected to anything");
        myVase.setMode(raven::sim::gui::cVase::e_mode::design);
        return;
    }
    while (true)
    {
        myVase.setSelected(f);
        fm.update();
        if (f->getName() == "Failed" ||
            f->getName() == "Success")
            break;
        wex::msgbox mb(
            fm,
            f->getName(),
            "Decision",
            MB_YESNO);
        switch (mb.myReturn)
        {
        case IDYES:
            f = f->getDestination2();
            break;
        case IDNO:
            f = f->getDestination();
            break;
        }
    }
}
namespace raven
{
    namespace sim
    {
        namespace gui
        {

            void cFlower::draw(wex::shapes &S)
            {
                S.rectangle({getLocationX(), getLocationY(), 50, 50});
                S.text(getName(),
                       {getLocationX() + 10, getLocationY() + 25});
            }

            void cDecision::draw(wex::shapes &S)
            {
                int x = getLocationX();
                int y = getLocationY();
                S.rectangle({x, y, 200, 50});
                S.text(getName(),
                       {x + 30, y + 15});
                int xp, yp;
                locationExitPort1(xp, yp);
                S.text("NO", {xp + 3, yp});
                locationExitPort2(xp, yp);
                S.text("YES", {xp - 20, yp});
            }
        }
    }
}

main()
{
    cRecipeGUI theGUI;
    return 0;
}
