#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

int screen_x = 1152;
int screen_y = 896;


int showCharacterSelection(RenderWindow &window, Sprite &background) {
    Texture yellowTex, greenTex;
    yellowTex.loadFromFile("Data/char22.png");
    greenTex.loadFromFile("Data/char12.png");

    Sprite yellowSprite(yellowTex);
    Sprite greenSprite(greenTex);

    yellowSprite.setPosition(150, 450);
    greenSprite.setPosition(450, 450);

    int selected = 0;
    yellowSprite.setScale(1.3f, 1.3f); // Initially yellow sprite is selected so it is highlighted

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();

            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Right) selected = 1;
                if (event.key.code == Keyboard::Left)  selected = 0;
                if (event.key.code == Keyboard::Enter) return selected;
            }
        }

        if (selected == 0) {
            yellowSprite.setScale(1.3f, 1.3f);
            greenSprite.setScale(1.0f, 1.0f);
        } else {
            yellowSprite.setScale(1.0f, 1.0f);
            greenSprite.setScale(1.3f, 1.3f);
        }

        window.clear();
        window.draw(background);      // <- IMPORTANT FIX
        window.draw(yellowSprite);
        window.draw(greenSprite);
        window.display();
    }

    return 0;
}





void display_level(RenderWindow& window, char**lvl, Texture& bgTex,Sprite& bgSprite,Texture& blockTexture,Sprite& blockSprite, const int height, const int width, const int cell_size)
{
	window.draw(bgSprite);

	for (int i = 0; i < height; i += 1)
	{
		for (int j = 0; j < width; j += 1)
		{

			if (lvl[i][j] == '#')
			{
				blockSprite.setPosition(j * cell_size, i * cell_size);
				window.draw(blockSprite);
			}
		}
	}

}

void player_gravity(char** lvl, float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth)
{
	offset_y = player_y;

	offset_y += velocityY;

	char bottom_left_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x ) / cell_size];
	char bottom_right_down = lvl[(int)(offset_y  + Pheight) / cell_size][(int)(player_x + Pwidth) / cell_size];
	char bottom_mid_down = lvl[(int)(offset_y + Pheight) / cell_size][(int)(player_x + Pwidth / 2) / cell_size];

	if (bottom_left_down == '#' || bottom_mid_down == '#' || bottom_right_down == '#')
	{
		onGround = true;
	}
	else
	{
		player_y = offset_y;
		onGround = false;
	}

	if (!onGround)
	{
		velocityY += gravity;
		if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
	}

	else
	{
		velocityY = 0;
	}
}


int main()

{
	RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP", Style::Resize); 
	
    // Load background texture
    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("Data/bg.png")) {
        return -1; 
    }

    // Background sprite
    Sprite backgroundSprite;
    backgroundSprite.setTexture(backgroundTexture);

    // --- SHOW SELECTION SCREEN ---
    int chosen = showCharacterSelection(window, backgroundSprite);

    // --- LOAD THE SELECTED PLAYER ---
    Texture playerTex;
    if (chosen == 0)
        playerTex.loadFromFile("Data/player2.png");
    else
        playerTex.loadFromFile("Data/player.png");

    Sprite player(playerTex);
    
    window.clear();

	
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	//level specifics
	const int cell_size = 64;
	const int height = 14;
	const int width = 18;
	char** lvl;

	//level and background textures and sprites
	Texture bgTex;
	Sprite bgSprite;
	Texture blockTexture;
	Sprite blockSprite;

	bgTex.loadFromFile("Data/bg.png");
	bgSprite.setTexture(bgTex);
	bgSprite.setPosition(0,0);

	blockTexture.loadFromFile("Data/block1.png");
	blockSprite.setTexture(blockTexture);

	//Music initialisation
	Music lvlMusic;

	lvlMusic.openFromFile("Data/mus.ogg");
	lvlMusic.setVolume(20);
	lvlMusic.play();
	lvlMusic.setLoop(true);
          int PlayerHeight = 98;
	//player data
	float player_x = 64;
         float player_y = 0;

       

         


	float speed = 5;

	const float jumpStrength = -20; // Initial jump velocity
	const float gravity = 1;  // Gravity acceleration

	bool isJumping = false;  // Track if jumping

	bool up_collide = false;
	bool left_collide = false;
	bool right_collide = false;



	bool onGround = false;

	float offset_x = 0;
	float offset_y = 0;
	float velocityY = 0;

	float terminal_Velocity = 20;

	
	int PlayerWidth = 96;

	bool up_button = false;

	char top_left = '\0';
	char top_right = '\0';
	char top_mid = '\0';

	char left_mid = '\0';
	char right_mid = '\0';

	char bottom_left = '\0';
	char bottom_right = '\0';
	char bottom_mid = '\0';

	char bottom_left_down = '\0';
	char bottom_right_down = '\0';
	char bottom_mid_down = '\0';

	char top_right_up = '\0';
	char top_mid_up = '\0';
	char top_left_up = '\0';

	
	player.setScale(3,3);
	player.setPosition(player_x, player_y);


	//creating level array
	lvl = new char* [height];
	for (int i = 0; i < height; i += 1)
	{
		lvl[i] = new char[width];
	}

	// --- CREATE LEVEL ARRAY ---
lvl = new char*[height];
for (int i = 0; i < height; i++)
    lvl[i] = new char[width];

// --- LOAD LEVEL FILE (correct way) ---
ifstream file("Data/level1.txt");

if (!file.is_open()) {
    cout << "ERROR: Could not open level1.txt\n";
}

// Read each row as a full string
string line;
for (int i = 0; i < height; i++)
{
    getline(file, line);

    // If blank line, read again
    if (line.empty()) {
        i--;
        continue;
    }

    // Copy characters into lvl[][]
    for (int j = 0; j < width; j++)
    {
        lvl[i][j] = line[j];
    }
}

file.close();



	Event ev;
	//main loop
	while (window.isOpen())
	{

		while (window.pollEvent(ev))
		{
			if (ev.type == Event::Closed) 
			{
				window.close();
			}

			if (ev.type == Event::KeyPressed)
			{
			}

		}

		//presing escape to close
		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		window.clear();

		display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, height, width, cell_size);
		player_gravity(lvl,offset_y,velocityY,onGround,gravity,terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth);
		player.setPosition(player_x, player_y);
		window.draw(player);

		window.display();
	}

	//stopping music and deleting level array
	lvlMusic.stop();
	for (int i = 0; i < height; i++)
	{
		delete[] lvl[i];
	}
	delete[] lvl;

	return 0;
}

