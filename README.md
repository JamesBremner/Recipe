# Recipe
A recipe is a decision flow chart.

A recipe contains:

 -  one source, labelled start, with no inputs and one output.

 - two sinks, labelled success and failed, with any number of inputs and no output

- any number of decision boxes with one input and two outputs, labelled yes and no.

The start, success and failure are connected by a network of design boxes  constructed by the user using a subset of the VASE code.

Recipes can be saved, loaded, edited and run.

Here is a simple recipe example with one decision box

![image](https://github.com/user-attachments/assets/f6ddbe9a-aaef-49c1-9fd6-c142706b616a)

# Recipe Editor

* Add a decision by right clicking on empty canvas and selecting `Decision` from drop down menu
* Add a pipe bend by right clicking on empty canvas and selecting `PipeBend` from drop down menu
* Move elements by left clicking on element ( source, sink, decision or pipe bend ) and moving mouse with SHIFT key pressed
* Connect elements with straight lines left clicking on first, then right clicking on second.
* Use pipe bends to make connections avoid other elements

![image](https://github.com/user-attachments/assets/57b71454-fc88-4326-8ccb-3f734d87feac)

* Edit question in decision with left click, then right clicki, then select `Edit` from pop-up menu
* Delete decision with left click, then right clicki, then select `Delete selected` from pop-up menu
* Delete output connections from decision  with left click, then right clicki, then select `Delete connections` from pop-up menu
* Edit title by left clicking then right clicking on source.
* Zoom view by rotating mouse wheel
* Pan view by by left clicking on empty canvaa and moving mouse with SHIFT key pressed
* `File | New` replaces current recipe with initial recipe contain only a source and two sinks
* `File | Open` replaces current recipe with saved recipe in selected file
* `File | Save` saves current recipe to a selected file.  Filename defaults to recipe title
* `Mode | Run` start the recipe runner
* `Help | Usage` displays how to use help
* `Help | About` displays copyright and licence

# Recipe Runner

The user can opt to run the recipe by selecting menu item `Mode | Run`

- The decision box on the output from start is highlighted

- A dialog box pops up asking 'Are you ready to start'

- Depending on the user's answer, the next decision box is highlighted and its dialog box pops up


