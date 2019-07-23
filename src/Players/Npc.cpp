#ifdef __unix__
#include "Npc.h"
#include "Lib/Texture.h"
#include "Quad.h"
#include "defs.h"
#include "Textbox.h"
#elif defined(_WIN32) || defined(WIN32)
#include "../../include/Players/Npc.h"
#include "../../include/Lib/Texture.h"
#include "../../include/Shape/Quad.h"
#include "../../include/defs.h"
#include "../../include/Players/Textbox.h"
#endif

Npc::Npc(int gx, int gy, TestRoom* rm) : Object(gx, gy, rm, false)
{
    image = new Quad(GRID_SIZE, GRID_SIZE, new Texture(std::string(".data/textures/npc_test.png"), STBI_rgb_alpha));

    // Heeeeey!!!
    auto glambda = [&](void)
    {
        OnTextboxFinish();
    };
    tBox = new Textbox(x, y - 100, std::string("m_w1SheepExpedition"), 28, glambda, room);
}

Npc::~Npc()
{
    delete image;
}

void Npc::Update()
{
	if (tBox != NULL)
		tBox->Update();
}

void Npc::Render()
{
	glColor4f(1, 1, 1, 1);

    image->Render(x, y);

	if (tBox != NULL)
	{
		tBox->SetXY(x, y - 100);

		tBox->Render();
		if (tBox->DeleteMe())
		{
			delete tBox;
			tBox = NULL;
		}
	}

	// Reset color!
	glColor4f(1, 1, 1, 1);
}

void Npc::OnTextboxFinish()
{
    printf("Yaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaayyyyyyyyyyyyyyyyyyyyyy!!\n");
}
