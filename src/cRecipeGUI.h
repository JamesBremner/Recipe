class cRecipeGUI
{
public:
    cRecipeGUI();

private:
    wex::gui &fm;
    wex::menu *myFileMenu;
    wex::menu *mySimMenu;
    wex::plot::plot &myPlot;
    wex::plot::trace &myPlotTrace;

    raven::sim::gui::cFlowerFactory myFactory;

    raven::sim::gui::cVase myVase;

    std::string mySimReport;
    std::string myDisplayReport;

    void menus();
    void registerEventHandlers();
    void onRightClick();
    void rename();

    void draw(wex::shapes &S);
    void drawArrow(
        wex::shapes &S,
        const cxy &exit,
        const cxy &entry);

    void simulate();

    void ConstructFlower();
    void SelectFlower();
    void config();
};
