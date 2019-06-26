#include "Npc.h"

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

}

void Npc::Render()
{
    image->Render(x, y);
}

void Npc::OnTextboxFinish()
{
    printf("Yaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaayyyyyyyyyyyyyyyyyyyyyy!!\n");
}
