#include "Cutscene.h"
#include "CutsceneObject.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

Cutscene::Cutscene(std::string name, GameLoop* gloop)
{
    ticks = -1;
    _gloop = gloop;

    // Load up the cutscene objects
    std::string filename = std::string(".data/cutscenes/") + name;

    // Load all sprites and objects!
    std::string line;
    std::ifstream myfile(filename.c_str());
    if (myfile.is_open())
    {
        std::cout << "Reading cutscene file:\n";




        // Read line by line
        int mode = -1;      // This means that it expects either an 's' or an 'o'
        CutsceneSprite* nCS = NULL;
        CutsceneObject* nCO = NULL;

        // So the mode (either sprite or object mode)
        // allows for the program to read how 1 obj or
        // sprite needs to be built. It's like to show
        // if we're just adding params to a function that
        // may have as many params as we want!!!
        while (std::getline(myfile, line))
        {
            // DEBUG
            std::cout << line << '\n';



            // Check current mode
            if (line.at(0) == '\t')
            {
                // Continue the mode!!! Keep adding params!
                if (mode < 0)
                {
                    // We got an error
                    printf("ERROR!!!!!!!\n\tMode isn\'t set for params to be added.\n\n\n");
                    break;
                }



                // Keep adding params to the object!!!
                switch (mode)
                {
                    // Add params to spr (nCS should be persistent)
                    case 0:
                    {
                        // Add properties!!!!
                        if (line.at(1) == 'p')      // If it's an image, the first thing should be a number, but setting props, put a p!
                        {
                            // Read those params!
                            line.erase(line.begin(), line.begin() + 2);     // Erase the p too so that it's just parsable vals.



                            int tpf;
                            bool repeat;
                            {
                                std::string token;
                                std::istringstream tokenStream(line);
                                int times = 0;
                                while (std::getline(tokenStream, token, ',') &&
                                    times <= 1)
                                {
                                    if (times == 0)     // Ficks per frame!
                                    {
                                        std::istringstream(token) >> tpf;
                                    }
                                    else if (times == 1)    // Repeat?
                                    {
                                        std::istringstream(token) >> repeat;
                                    }

                                    times++;
                                }
                            }

                            // Edit the props!
                            nCS->SetTicksPerFrame(tpf);
                            nCS->SetRepeat(repeat);

                            break;
                        }







                        // Add an image!!

                        // This way we can parse and get the code
                        std::string dims, fname;
                        {
                            std::string token;
                            std::istringstream tokenStream(line);

                            std::getline(tokenStream, token, '\t');     // '\t'
                            std::getline(tokenStream, token, '\t');     // dimensions!
                            dims = token;
                            std::getline(tokenStream, token, '\t');     // Texture fname!!
                            fname = token;
                        }






                        // Read Dimensions
                        int w, h;
                        {
                            std::string token;
                            std::istringstream tokenStream(dims);
                            int times = 0;
                            while (std::getline(tokenStream, token, ',') &&
                                times <= 1)
                            {
                                if (times == 0)     // Width
                                {
                                    std::istringstream(token) >> w;
                                }
                                else if (times == 1)    // Height
                                {
                                    std::istringstream(token) >> h;
                                }

                                times++;
                            }
                        }

                        // The quad!
                        Quad* tempQ = new Quad(w, h, new Texture(std::string(".data/") + fname, STBI_rgb_alpha));
                        nCS->AddImage(tempQ);
                    }
                    break;




                    // Add params to obj
                    // In specific, the functions to tie to there eh!
                    case 1:
                    {
                        // This way we can parse and get the code
                        int startTick, endTick;
                        std::string func, params;
                        {
                            std::string token;
                            std::istringstream tokenStream(line);

                            std::getline(tokenStream, token, '\t');     // '\t'
                            std::getline(tokenStream, token, '\t');     // Starttick!
                            std::istringstream(token) >> startTick;
                            std::getline(tokenStream, token, '\t');     // Endtick!
                            std::istringstream(token) >> endTick;
                            std::getline(tokenStream, token, '\t');     // Function name!
                            func = token;
                            std::getline(tokenStream, token, '\n');     // Parameters!
                            params = token;
                        }

                        // Add extra stuff
                        nCO->RegisterFunction(startTick, endTick, func, params);
                    }
                    break;
                }
            }












            else
            {
                // Set the mode
                mode = -1;
                if (line.at(0) == 's')
                {
                    // Init Sprite!
                    mode = 0;
                    nCS = NULL;




                    // This way we can parse and get the code
                    std::string dims, fname;
                    {
                        std::string token;
                        std::istringstream tokenStream(line);

                        std::getline(tokenStream, token, '\t');     // 's'
                        std::getline(tokenStream, token, '\t');     // dimensions!
                        dims = token;
                        std::getline(tokenStream, token, '\t');     // Texture fname!!
                        fname = token;
                    }






                    // Read Dimensions
                    int w, h;
                    {
                        std::string token;
                        std::istringstream tokenStream(dims);
                        int times = 0;
                        while (std::getline(tokenStream, token, ',') &&
                            times <= 1)
                        {
                            if (times == 0)     // Width
                            {
                                std::istringstream(token) >> w;
                            }
                            else if (times == 1)    // Height
                            {
                                std::istringstream(token) >> h;
                            }

                            times++;
                        }
                    }

                    // The quad!
                    Quad* tempQ = new Quad(w, h, new Texture(std::string(".data/") + fname, STBI_rgb_alpha));


                    // Finish init of the sprite!!!
                    nCS = new CutsceneSprite(tempQ);            //ctor
                    sprites.push_back(nCS);
                }
                else if (line.at(0) == 'o')
                {
                    // Init Object!
                    mode = 1;
                    nCO = NULL;



                    // This way we can parse and get the code
                    int sprId;
                    std::string coords;
                    {
                        std::string token;
                        std::istringstream tokenStream(line);

                        std::getline(tokenStream, token, '\t');     // 'o'
                        std::getline(tokenStream, token, '\t');     // sprite id!
                        std::istringstream(token) >> sprId;
                        std::getline(tokenStream, token, '\t');     // start coords!!
                        coords = token;
                    }






                    // Read starting coordinates.
                    int sx, sy;
                    {
                        std::string token;
                        std::istringstream tokenStream(coords);
                        int times = 0;
                        while (std::getline(tokenStream, token, ',') &&
                            times <= 1)
                        {
                            if (times == 0)     // Start x
                            {
                                std::istringstream(token) >> sx;
                            }
                            else if (times == 1)    // Start y
                            {
                                std::istringstream(token) >> sy;
                            }

                            times++;
                        }
                    }









                    // Finish init!
                    CutsceneSprite* tmpSpr = NULL;
                    if (sprId >= 0)
                        tmpSpr = sprites.at(sprId);

                    nCO = new CutsceneObject(sx, sy, sprId, this);
                    objects.push_back(nCO);
                }
            }





        }

        // We're done!!!!!
        printf("\nCutscene loaded.\n");
        myfile.close();
    }



}

Cutscene::~Cutscene()
{
    // Delete everything!!!!
    for (int i = 0; i < sprites.size(); i++)
    {
        delete sprites.at(i);
    }
    sprites.clear();

    for (int i = 0; i < objects.size(); i++)
    {
        delete objects.at(i);
    }
    objects.clear();
}






void Cutscene::Update()
{
    ticks++;        // Since it's init'd as -1, this should be 0 now! (if first time)

    // Only update and render objects
    for (int i = 0; i < objects.size(); i++)
    {
        objects.at(i)->Update(ticks);
    }
}

void Cutscene::Render()
{
    // Reset trans. matrix
    glLoadIdentity();

    // Only update and render objects
    for (int i = 0; i < objects.size(); i++)
    {
        objects.at(i)->Render(ticks);
    }
}


void Cutscene::Delete(CutsceneObject* thisObj)
{
    objects.erase(std::remove(objects.begin(), objects.end(), thisObj), objects.end());
}

void Cutscene::End(Room* nextRoom)
{
    // Load this next room and then switch to it!!!
    _gloop->SetRoom(nextRoom);
}

CutsceneSprite* Cutscene::GetSpriteByID(int id)
{
    if (id >= 0 && id < sprites.size())
    {
        return sprites.at(id);
    }

    // print out error
    printf("ERROR: sprite id %i doesn't relate to a sprite inside the list! (size=%i)\n", id, sprites.size());
    return NULL;
}









GameLoop* Cutscene::GetGameLoop()
{
    return _gloop;
}
