class cRecipeGUI
{
public:
    cRecipeGUI();

private:
    wex::gui &fm;
    wex::menu *myFileMenu;
    wex::menu *myModeMenu;

    raven::recipe::cFlowerFactory myFactory;

    raven::recipe::cVase myVase;

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

    void runRecipe();

    void ConstructFlower();
    void SelectFlower();
    void config();
};
