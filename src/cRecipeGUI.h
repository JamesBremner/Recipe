class cPanZoom
{
public:

    cPanZoom();

    void operator()(int &x, int &y) const;
    void operator()(cxy &xy) const;
    void zoom(int& s) const;

    void inc()
    {
        myZoom++;
        zoomfactor();
    }
    void dec()
    {
        myZoom--;
        zoomfactor();
    }

    void pan(const wex::sMouse &ms);
    void panStop();
    bool isPanning() const;

private:
    cxy myOff;
    int myZoom;
    float myZoomFactor;
    cxy myPrevPos;

    void zoomfactor();
};

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

    cPanZoom myPanZoom;

    /// @brief construct menues along top of app window
    void menus();

    /// @brief register code to run on events
    void registerEventHandlers();

    /// @brief clear and init an empty recipe
    void init();

    /// @brief handle a right mouse button click
    void onRightClick();

    /// @brief flower configuration menu
    wex::menu menuConfig(
        raven::recipe::cFlower *clickedFlower);

    /// @brief save recipe to a file
    void save();

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

    /// @brief get panned and zoomed location of a flower's port
    /// @param port
    /// @param flower
    /// @return
    cxy locatePort(
        int port,
        raven::recipe::cFlower *flower);

    /// @brief recipe runner
    void runRecipe();

    /// @brief prompt for flower type and construct selected
    void ConstructFlower();

    /// @brief select flower under mouse cursor
    void SelectFlower();

    /// @brief flower configuration menu
    void config();
};
