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

* Add a decision by right clicking on canvas and selecting `decision` from drop down menu
* Connect output of one event handler to input of second by left clicking on first, then right clicking on second


# Recipe Runner

The user can opt to run the recipe by selecting menu item `Mode | Run`

- The decision box on the output from start is highlighted

- A dialog box pops up asking 'Are you ready to start'

- Depending on the user's answer, the next decision box is highlighted and its dialog box pops up


