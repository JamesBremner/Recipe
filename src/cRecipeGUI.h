class cRecipeGUI
{
public:
    cRecipeGUI();

private:
    wex::gui &fm;
    wex::menu *myFileMenu;
    wex::menu *myModeMenu;

    raven::sim::gui::cFlowerFactory myFactory;

    raven::sim::gui::cVase myVase;

    // std::string mySimReport;
    // std::string myDisplayReport;

    void menus();
    void registerEventHandlers();
    void init();

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
