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

cPanZoom::cPanZoom()
{
    clear();
}
void cPanZoom::clear()
{
    myOff = cxy(0,0);
    myZoom = 0;
    myZoomFactor = 1;
    myPrevPos = cxy(-1,-1);
}
void cPanZoom::zoomfactor()
{
    if (myZoom > 3)
        myZoom = 3;
    if (myZoom < -3)
        myZoom = -3;
    myZoomFactor = 1.0;
    switch (myZoom)
    {
    case -3:
        myZoomFactor = 0.25;
        break;
    case -2:
        myZoomFactor = 0.5;
        break;
    case -1:
        myZoomFactor = 0.75;
        break;
    case 0:
        myZoomFactor = 1.0;
        break;
    case 1:
        myZoomFactor = 1.5;
        break;
    case 2:
        myZoomFactor = 2;
        break;
    case 3:
        myZoomFactor = 2.5;
        break;
    default:
        myZoomFactor = 1;
        break;
    }
}
void cPanZoom::operator()(cxy &xy) const
{
    xy = xy - myOff;
    xy *= myZoomFactor;
}
void cPanZoom::undo(cxy &xy)
{
    //std::cout << " ( "<<myZoomFactor<<" "<< myOff<< " )";
    xy /= myZoomFactor;
    xy += myOff;
}

void cPanZoom::operator()(int &x, int &y) const
{
    cxy xy(x, y);
    (xy);
    x = xy.x;
    y = xy.y;
}

void cPanZoom::zoom(int &s) const
{
    s *= myZoomFactor;
}
void cPanZoom::pan(const wex::sMouse &ms)
{
    // std::cout << "pan ";
    cxy pos(ms.x, ms.y);
    if (myPrevPos.x >= 0)
    {
        myOff = myOff + pos.vect(myPrevPos);
    }
    myPrevPos = cxy(ms.x, ms.y);
}
void cPanZoom::panStop()
{
    myPrevPos.x = -1;
}
bool cPanZoom::isPanning() const
{
    return myPrevPos.x > -1;
}

bool cPanZoom::test()
{
    cPanZoom PZ;
    wex::sMouse ms;
    ms.x = 0;
    ms.y = 0;
    //PZ.pan(ms);
    ms.x = 200;
    //PZ.pan(ms);
    PZ.dec();
    PZ.dec();

    cxy txy = cxy(140,440);
    PZ(txy);
    PZ.undo(txy);

    return false;
}

cRecipeGUI::cRecipeGUI()
    : fm(wex::maker::make())
{
    fm.move({50, 50, 800, 800});
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
            myPanZoom.clear();
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
            save();
        });
    mb.append("File", *myFileMenu);

    myModeMenu = new wex::menu(fm);
    myModeMenu->append(
        "Design",
        [&](const std::string &title)
        {
            myVase.setMode(raven::recipe::cVase::e_mode::design);
        });
    myModeMenu->append(
        "Run",
        [&](const std::string &title)
        {
            runRecipe();
        });
    mb.append("Mode", *myModeMenu);

    myHelpMenu = new wex::menu(fm);
    myHelpMenu->append(
        "Usage",
        [&](const std::string &title)
        {
            wex::msgbox(
                "Recipe\n"
"======\n"
"A recipe is a decision flow chart.\n"
"A recipe contains:\n"
"- one source, labelled start, with no inputs and one output.\n"
"- two sinks, labelled success and failed, with any number of inputs and no output\n"
"- any number of decision boxes with one input and two outputs, labelled yes and no.\n\n"
"The start, success and failure are connected by a network of design boxes  constructed by the user using a subset of the VASE code.\n\n"
"Recipe Editor\n"
"=============\n\n"
"* Add a decision by right clicking on empty canvas and selecting `Decision` from drop down menu\n"
"* Add a pipe bend by right clicking on empty canvas and selecting `PipeBend` from drop down menu\n"
"* Move elements by left clicking on element ( source, sink, decision or pipe bend ) and moving mouse with SHIFT key pressed\n"
"* Connect elements with straight lines left clicking on first, then right clicking on second.\n"
"* Use pipe bends to make connections avoid other elements\n"
"* Edit question in decision with left click, then right clicki, then select `Edit` from pop-up menu\n"
"* Delete decision with left click, then right clicki, then select `Delete selected` from pop-up menu\n"
"* Delete output connections from decision  with left click, then right clicki, then select `Delete connections` from pop-up menu\n"
"* Edit title by left clicking then right clicking on source.\n"
"* Zoom view by rotating mouse wheel\n"
"* Pan view by by left clicking on empty canvaa and moving mouse with SHIFT key pressed\n"
"* `File | New` replaces current recipe with initial recipe contain only a source and two sinks\n"
"* `File | Open` replaces current recipe with saved recipe in selected file\n"
"* `File | Save` saves current recipe to a selected file.  Filename defaults to recipe title\n"
"* `Mode | Run` start the recipe runner\n\n"
"Recipe Runner\n"
"=============\n\n"

"The user can opt to run the recipe by selecting menu item `Mode | Run`\n"

"- The decision box on the output from start is highlighted\n"
"- A dialog box pops up asking ''Are you ready to start''\n"
"- Depending on the user''s answer, the next decision box is highlighted and its dialog box pops up\n"

                      );
        });
    myHelpMenu->append(
        "About",
        [&](const std::string &title)
        {
            wex::msgbox(
                "Recipe Manager\n\n"
                "(c) 2024 James Bremner\n"
                "MIT license\n\n"
                "Produced for Lucas Santos\n"            );
        });
     mb.append("Help", *myHelpMenu);
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
        [&](wex::sMouse &ms)
        {
            if (!ms.shift)
            {
                myPanZoom.panStop();
                return;
            }
            auto *flower = myVase.find(ms.x, ms.y);
            if (flower != nullptr && (!myPanZoom.isPanning()))
                flower->setLocationCenter(ms.x, ms.y);
            else
            {
                myPanZoom.pan(ms);
            }

            fm.update();
        });

    fm.events().mouseWheel(
        [this](int dist)
        {
            if (dist < 0)
                myPanZoom.inc();
            else
                myPanZoom.dec();
            // std::cout << "Zoom " << myPanZoom.myZoom << " ";
            fm.update();
        });
}

void cRecipeGUI::onRightClick()
{
    auto ms = mouseInModel();
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

    menuConfig(clickedflower).popup(ms.x, ms.y);

    return;
}

wex::menu cRecipeGUI::menuConfig(
    raven::recipe::cFlower *clickedflower)
{
    wex::menu m(fm);

    if (clickedflower->getType() == myFactory.Index("Source"))
        m.append("Edit Title",
                 [&](const std::string &title)
                 {
                     rename();
                     fm.update();
                 });
    else if (clickedflower->getType() == myFactory.Index("PipeBend"))
        ;
    else
        m.append("Edit Question",
                 [&](const std::string &title)
                 {
                     rename();
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

    return m;
}
void cRecipeGUI::save()
{
    wex::filebox fb(fm);

    // default to recipe title TID12
    auto *f = myVase.findType(myFactory.Index("Source"));
    if (f)
    {
        auto title = f->getName();
        if (title.length() && (title != "Start"))
            fb.initFile(title + ".recipe");
    }

    auto fn = fb.save();

    if (fn.empty())
        return;
    try
    {
        myVase.Write(fn);
    }
    catch (std::runtime_error &e)
    {
        wex::msgbox(e.what());
    }
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

cxy cRecipeGUI::locatePort(
    int port,
    raven::recipe::cFlower *flower)
{

    cxy offset;
    switch (port)
    {
    case 0:
        flower->entryPortOffset(offset);
        break;
    case 1:
        flower->exitPort1Offset(offset);
        break;
    case 2:
        flower->exitPort2Offset(offset);
        break;
    }
    cxy ret(
        flower->getLocationX(),
        flower->getLocationY());
    ret = ret + offset;
    myPanZoom(ret);
    return ret;
}

void cRecipeGUI::draw(wex::shapes &S)
{
    S.textHeight(12);
    for (raven::recipe::cFlower *flower : myVase)
    {
        if (flower->isSelected())
            S.color(0x0000FF);
        else
            S.color(0);

        flower->draw(S, myPanZoom);

        // draw connections
        S.color(0xFF0000);
        int xep, yep, xdst, ydst;
        auto *dstFlower = flower->getDestination();
        if (dstFlower)
        {
            // flower->exitPort1Offset(xep, yep);
            // myPanZoom(xep, yep);
            // dstFlower->entryPortOffset(xdst, ydst);
            // myPanZoom(xdst, ydst);
            // if (dstFlower->getType() == myFactory.Index("PipeBend"))
            //     S.line(cxy(xep, yep), cxy(xdst, ydst));
            // else
            drawArrow(
                S,
                locatePort(1, flower),
                locatePort(0, dstFlower));
        }
        dstFlower = flower->getDestination2();
        if (dstFlower)
        {
            // flower->exitPort2Offset(xep, yep);
            // myPanZoom(xep, yep);
            // dstFlower->entryPortOffset(xdst, ydst);
            // myPanZoom(xdst, ydst);
            // if (dstFlower->getType() == myFactory.Index("PipeBend"))
            //     S.line(cxy(xep, yep), cxy(xdst, ydst));
            // else
            drawArrow(
                S,
                locatePort(2, flower),
                locatePort(0, dstFlower));
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
    static cxy ms;
    ms = mouseInModel();
    // std::cout << "mouse at " << ms.x << " " << ms.y << "\n";
    wex::menu m(fm);
    for (auto flower : myFactory.dictionary())
    {
        if (raven::recipe::cFlowerFactory::isAllowed(flower.myName))
            m.append(flower.myName,
                     [&](const std::string &title)
                     {
                         if (!myVase.Add(title))
                             return;
                         myVase.getSelected()->setLocationTopLeft(ms.x, ms.y);
                         fm.update();
                     });
    }

    auto mv = fm.getMouseStatus();
    m.popup(mv.x, mv.y);
}

cxy cRecipeGUI::mouseInModel()
{
    auto ms = fm.getMouseStatus();
    cxy mouse(ms.x, ms.y);
    myPanZoom.undo(mouse);  
    std::cout << ms.x <<" " << ms.y <<" -> "<< mouse.x <<" "<< mouse.y << "\n";
    return mouse;  
}

void cRecipeGUI::SelectFlower()
{
    cxy mouse = mouseInModel();
    auto *flower = myVase.find(mouse.x, mouse.y);
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
void cRecipeGUI::runRecipe()
{
    myVase.setMode(raven::recipe::cVase::e_mode::run);
    auto *f = myVase.find("Start");
    if (!f)
    {
        wex::msgbox("Start missing");
        myVase.setMode(raven::recipe::cVase::e_mode::design);
        return;
    }
    f = f->getDestination();
    if (!f)
    {
        wex::msgbox("Start not connected to anything");
        myVase.setMode(raven::recipe::cVase::e_mode::design);
        return;
    }
    while (true)
    {
        myVase.setSelected(f);
        fm.update();

        // have we reached a final destination?
        if (f->getName() == "Failed" ||
            f->getName() == "Success")
            break;

        // prompt user for decision
        wex::msgbox mb(
            fm,
            f->getName(),
            "Decision",
            MB_YESNO);

        // exit decision box according to user's choice
        switch (mb.myReturn)
        {
        case IDYES:
            f = f->getDestination2();
            break;
        case IDNO:
            f = f->getDestination();
            break;
        }

        // travel along pipebends
        while (true)
        {
            if (f->getType() != myFactory.Index("PipeBend"))
                break;
            else
                f = f->getDestination();
        }
    }
}
namespace raven
{
    namespace recipe
    {
        void cFlower::draw(
            wex::shapes &S,
            const cPanZoom &pz)
        {
            cxy tl(
                getLocationX(), getLocationY());
            pz(tl);
            int w = 50;
            pz.zoom(w);

            S.rectangle(tl, cxy(w, w));
            S.textxy(getName(),
                   cxy(tl.x + 10, tl.y + 25));
        }

        void cPipeBend::draw(
            wex::shapes &S,
            const cPanZoom &pz)
        {
            cxy tl(
                getLocationX(), getLocationY());
            pz(tl);
            S.rectangle(tl, cxy(5, 5));
        }

        void cDecision::draw(
            wex::shapes &S,
            const cPanZoom &pz)
        {
            int x = getLocationX();
            int y = getLocationY();
            cxy tl(x, y), ep1, ep2;
            exitPort1Offset(ep1);
            exitPort2Offset(ep2);
            cxy wh(200,50);

            ep1 = ep1 + tl;
            ep2 = ep2 + tl;

            pz(tl);
            pz(ep1);
            pz(ep2);
            pz(wh);

            S.rectangle(tl, wh);
            S.textxy(getName(),
                   cxy(tl.x + 30, tl.y + 5), wh);
            S.textxy("NO", ep1);
            S.textxy("YES", ep2);
        }
    }
}

main()
{
    cPanZoom::test();
    cRecipeGUI theGUI;
    return 0;
}
