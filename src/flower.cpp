#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include "flower.h"

namespace raven
{
    namespace recipe
    {
        // namespace gui
        // {

            /*

              Keep track of last index assigned

            */
            int cFlower::lastIndex = 0;

            const int handlesize = 10;

            /**

              Construct a flower

              Never construct a flower directly.
              Instead call the method cVase::Add( cFlowerFactory::type ).
              This will look after constructing a flower of the correct type
              and adding it to the vase.

              Re-implement this for each specialised flower type

            */
            cFlower::cFlower()
                : myName("unnamed"), myTypeName("flower"), myType(1), myShape("box"), myWidth(50), myHeight(50), IsSelected(false), myDestination(NULL), myDestination2(NULL), myOutputPortCount(1)
            {
                // Assign unique index
                myIndex = lastIndex++;

                setName();
            }
            /**

              Set name

              @param[in] n name

              If n is blank, set to typename_index

            */
            void cFlower::setName(const string &n)
            {
                if (n.length() > 0)
                    myName = n;
                else
                    myType = cFlowerFactory::Index(myTypeName);
            }
            void cFlower::setIndex(int id)
            {
                myIndex = id;
                if (id > lastIndex)
                    lastIndex = id;
            }

            void cFlower::locationExitPort1(int &x, int &y)
            {
                x = myX + 25;
                y = myY + 50;
            }

            /**

              Move flower top left to new location

              @param[in] x new left location
              @param[in] y new top location

            */
            void cFlower::setLocationTopLeft(int x, int y)
            {
                myX = x;
                myY = y;
            }
            /**

              Move flower center to new location

              @param[in] x new center location
              @param[in] y new center location

            */
            void cFlower::setLocationCenter(int x, int y)
            {
                static int gridInc = 5;
                x = (x * gridInc) / gridInc;
                y = (y * gridInc) / gridInc;
                myX = x - myWidth / 2;
                if (myHeight < 0)
                    myY = y - myWidth / 2;
                else
                    myY = y - myHeight / 2;
            }

            /**

              Write data to graphviz dot format file

              @param[in] fp file pointer

              name [shape=circle,pos="25,25"] // [ vase_type='flower' vase_idx=1 ]

            */
            void cFlower::Write(FILE *fp)
            {
                std::string name_no_spaces;
                for (int p = 0; p < myName.length(); p++)
                    if (myName[p] == ' ')
                        name_no_spaces += '_';
                    else
                        name_no_spaces += myName[p];

                fprintf(fp,
                        "%s [shape=%s,pos=\"%d,%d\"] // [ vase_type='%s' vase_idx=%d ",
                        name_no_spaces.c_str(),
                        myShape.c_str(),
                        myX, myY,
                        myTypeName.c_str(),
                        myIndex);

                for (auto &pp : myParam)
                {
                    fprintf(fp,
                            "p_%s %f ",
                            pp.second.name.c_str(), pp.second.value);
                }
                fprintf(fp, "]\n");
            }

            /**

              Write data to graphviz dot format file

              @param[in] fp file pointer

              name1 -> name2 // start_idx=0 end_idx=1

            */
            void cFlower::WritePipe(FILE *fp)
            {
                if (myDestination)
                {
                    fprintf(fp, "%S -> %S // start_idx=%d end_idx=%d\n",
                            (wchar_t *)myName.c_str(),
                            (wchar_t *)myDestination->getName().c_str(),
                            myIndex, myDestination->getIndex());
                }
                if (myDestination2)
                {
                    fprintf(fp, "%S -> %S // start_idx=%d end_idx=%d\n",
                            (wchar_t *)myName.c_str(),
                            (wchar_t *)myDestination2->getName().c_str(),
                            myIndex, myDestination2->getIndex());
                }
            }
            /**

              Read from line graphviz dot file

            */
            void cFlower::Read(const string &line)
            {
                int p;

                p = line.find("pos=") + 5;
                int x = atoi(line.substr(p).c_str());
                p = line.find(",", p) + 1;
                int y = atoi(line.substr(p).c_str());
                setLocationTopLeft(x, y);

                auto no_spaces = line.substr(0, line.find(" "));
                std::string name;
                for (int p = 0; p < no_spaces.length(); p++)
                    if (no_spaces[p] == '_')
                        name += ' ';
                    else
                        name += no_spaces[p];
                setName(name);

                p = line.find("vase_idx=");
                int idx;
                idx = atoi(line.substr(p + 9).c_str());
                setIndex(idx);

                // loop over parameters
                for (auto &param : myParam)
                {
                    // find specified value of parameter in input file
                    string target = "p_" + param.second.name;
                    p = line.find(target);
                    if (p != -1)
                    {
                        p += target.length() + 1;

                        // set the value of the paramete
                        param.second.value = atoi(line.substr(p).c_str());
                        // std::wcout << myName << " " << param.second.name << " = " << param.second.value << std::endl;
                    }
                }
            }

            void cFlower::Connect(cFlower *d)
            {
                if (d == this)
                    return;
                if (!myDestination)
                    myDestination = d;
                else
                    myDestination2 = d;
            }

            /**

              Add parameter

              @param[in] name Name of parameter
              @param[in] description
              @param[in] defaultvalue  Initial value for parameter, default 0.0

              When user selects configure a dialog window pops up
              This shows a list of name value pairs and allows the values to be edited.

              In the constructor for a specialized flower,
              call this method to add a row to the parameter configuration dialog
              for the specialized flower type.

            */
            void cFlower::AddParam(const string &name, const string &description, double defaultvalue)
            {
                sparam param;
                param.name = name;
                param.value = defaultvalue;
                param.description = description;
                myParam.insert(std::pair<string, sparam>(name, param));
            }

            double cFlower::getValue(
                const string &name) const
            {

                auto pit = myParam.find(name);
                if (pit == myParam.end())
                {
                    return -1.0;
                }
                return pit->second.value;
            }
            bool cFlower::IsUnder(int x, int y)
            {
                if (myHeight < 0)
                    return (myX <= x && myY <= y &&
                            myX + myWidth >= x && myY + myWidth >= y);
                else
                    return (myX <= x && myY <= y &&
                            myX + myWidth >= x && myY + myHeight >= y);
            }

            cFlowerDict *cFlowerFactory::myFlowerDict;

            /**

              Construct flower factory

            */
            cFlowerFactory::cFlowerFactory()
            {
                myFlowerDict = new cFlowerDict();
            }

            cFlowerDict &cFlowerFactory::dictionary()
            {
                return *myFlowerDict;
            }
            /** Construct popup menu of flower types that can be created

              @param[in] IDM_CreateGeneric menu ID for the base flower creation popup menu option

              This creates a menu option for the creation each flower type.
              There must be enough room above IDM_CreateGeneric for all the flower types.
            */
            void cFlowerFactory::setMenu(int IDM_CreateGeneric)
            {
                for (auto &t : *myFlowerDict)
                {
                    // TODO: myMenuTask.Append( IDM_CreateGeneric+t.myIndex, t.myName );
                }
            }

            int cFlowerFactory::Index(
                const std::string &flower_type_name)
            {
                return myFlowerDict->Find(flower_type_name);
            }
            int cFlowerFactory::TypeCount()
            {
                return (int)myFlowerDict->Size();
            }

            /**

              Construct a new instance of a specified flower type

              @param[in] flower_type_name the flower type to create

            */
            cFlower *cFlowerFactory::Construct(const std::string &flower_type_name)
            {
                int index = myFlowerDict->Find(flower_type_name);
                if (index < 0)
                {
                    return nullptr;
                }

                return Construct(index);
            }
            cFlower *cFlowerFactory::Construct(
                int flower_type_index)
            {
                switch (flower_type_index)
                {
                case 1:
                    return new cFlower();
                case 5:
                    return new cSource();
                case 7:
                    return new cSink();
                case 11:
                    return new cPipeBend();
                case 12:
                    return new cDecision();

                default:
                    throw std::runtime_error(
                        "Unrecognized flower type " + std::to_string(flower_type_index));
                }
            }

            cSink::cSink()
            {
                myTypeName = "Sink";
                setName();
            }
            cSource::cSource()
            {
                myTypeName = "Source";
                setName();
                AddParam("Steady", "1 for constant rate, 0 for exponential");
                AddParam("Mean", "Mean time between arrivals", 1);
                AddParam("Volume", "Volume of each arrival", 1);

                std::vector<string> vQualityNames;
                // raven::recipe::tern::cQuality::getNames( vQualityNames );
                // for ( auto& q : vQualityNames )
                // {
                //     if( ! q.empty() )
                //         AddParam( q, "" );
                // }
            }
            cPipeBend::cPipeBend()
                : cFlower()
            {
                myTypeName = "PipeBend";
                myType = cFlowerFactory::Index(myTypeName);
                myName = "PB" + std::to_string(myIndex);
                myWidth = 5;
            }
            void cPipeBend::getEntryPort(int &x, int &y)
            {
                x = myX;
                y = myY;
            }
            void cPipeBend::locationExitPort1(int &x, int &y)
            {
                x = myX;
                y = myY;
            }
            cDecision::cDecision()
                : cFlower()
            {
                myTypeName = "Decision";
                myType = cFlowerFactory::Index(myTypeName);
                myName = "Decision" + std::to_string(myIndex);
                myWidth = 200;
                myHeight = 50;
            }
            void cDecision::getEntryPort(int &x, int &y)
            {
                x = myX + 100;
                y = myY;
            }
            void cDecision::locationExitPort1(int &x, int &y)
            {
                x = myX;
                y = myY + 25;
            }
            void cDecision::locationExitPort2(int &x, int &y)
            {
                x = myX + 200;
                y = myY + 25;
            }

            void cFlowerDict::Insert(const std::string &flower_type_name)
            {
                // std::cout << "Flower Dictionary Insert " << myDict.size()+1 <<" "<< flower_type_name << std::endl;
                myDict.insert(
                    cFlowerType(
                        myDict.size() + 1,
                        flower_type_name));
            }
        }
    }
//}
