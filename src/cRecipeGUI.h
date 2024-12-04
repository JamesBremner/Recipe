/// @brief User interface for the recipe application
class cRecipeGUI
{
public:
    cRecipeGUI();

private:

    // windex widgets
    wex::gui &fm;
    wex::menu *myFileMenu;
    wex::menu *myModeMenu;

    // flower construction factory
    raven::recipe::cFlowerFactory myFactory;

    // flower container
    raven::recipe::cVase myVase;

    /// @brief construct menues along top of app window
    void menus();

    /// @brief register code to run on events
    void registerEventHandlers();

    /// @brief clear and init an empty recipe
    void init();

    /// @brief handle a right mouse button click
    void onRightClick();

    /// @brief edit the question in a decision
    void rename();

    /// draw recipe on app canves
    void draw(wex::shapes &S);

    /// @brief  draw connect with arrow at destination
    /// @param S drawing context
    /// @param exit source exit port location
    /// @param entry destination entry port location
    void drawArrow(
        wex::shapes &S,
        const cxy &exit,
        const cxy &entry);

    /// @brief recipe runner
    void runRecipe();

    /// @brief prompt for flower type and construct selected
    void ConstructFlower();

    /// @brief select flower under mouse cursor
    void SelectFlower();

    /// @brief flower configuration menu
    void config();
};
