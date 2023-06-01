// based on SFML Game Development book :)

#include "Game.h"

/*******************************************************************************************************************************************************************/

Game::Game()
	: mWindow(sf::VideoMode(800, 600), "Pizza Panic")
	, Player()
	, pView(sf::FloatRect(0.f, 0.f, mWindow.getSize().x, mWindow.getSize().y))
	, mainMenu(m_font, m_menuBackground)
{
	Initialize();
}

/*******************************************************************************************************************************************************************/

void Game::showMainMenu()
{
	while (mWindow.isOpen()){
		sf::Event event;
		while (mWindow.pollEvent(event)){
			mainMenu.handleEvent(event);
			if (event.type == sf::Event::Closed)
				mWindow.close();
		}
		mWindow.clear();
		mWindow.draw(mainMenu);
		mWindow.display();

		if (mainMenu.getSelectedOption() == MainMenu::Option::Jugar || mainMenu.getSelectedOption() == MainMenu::Option::Salir)
			break;
	}
}

/*******************************************************************************************************************************************************************/

void Game::run()
{
	do {
		// Initialize variables
		mainMenu.resetSelectedOption();
		processEvents();
		bites = 0.f;
		quitarVida = 0.f;
		mItem.setTexture(mItemTexture);
		mItem.setPosition(3070.f, 2760.f);
		Player.sprite.setPosition(Player.initialX, Player.initialY);
		mItem.setScale(1.7f, 1.7f);
		chiwis.hitbox.setPosition(3000.f, 700.f);
		manteca.hitbox.setPosition(2366.f, 2800.f);

		pView.reset(sf::FloatRect(0, 0, mWindow.getSize().x, mWindow.getSize().y));
		mWindow.setView(pView);	

		menuMusic.play();
		menuMusic.setLoop(true);
		showMainMenu();
		
		// If Play button is clicked, start the game
		if (mainMenu.getSelectedOption() == MainMenu::Option::Jugar) {

			// Play game music
			menuMusic.stop();
			music.play();
			music.setLoop(true);

			// Keep track of the player's initial position
			sf::Vector2f previousPlayerPos = Player.sprite.getPosition();

			sf::Clock clock;

			sf::Time timeSinceLastUpdate = sf::Time::Zero;
			while (mWindow.isOpen()){
				processEvents();
				timeSinceLastUpdate += clock.restart();
				while (timeSinceLastUpdate > TimePerFrame){
					timeSinceLastUpdate -= TimePerFrame;
					processEvents();
					update(TimePerFrame);
				}
				render();
				// If player dies
				if (bites >= 60) {
					deliveredPizzas = 0;
					mItem.setPosition(3070.f, 2760.f);
					cargandoItem = false;
					music.stop();
					deathSound.play();
					deathSound.setLoop(true);
			
					GameOver.setPosition(pView.getCenter().x - 460.f, pView.getCenter().y - 500.f);
					mWindow.clear();
					mWindow.draw(GameOver);
					mWindow.display();
                    std::this_thread::sleep_for(std::chrono::seconds(2));
					deathSound.stop();
					break;
				}
			}
		}
	} while (mainMenu.getSelectedOption() == MainMenu::Option::Jugar);
}

/*******************************************************************************************************************************************************************/



void Game::processEvents()
// User input
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			Player.handlePlayerInput(event.key.code, true);
			break;
		case sf::Event::KeyReleased:
			Player.handlePlayerInput(event.key.code, false);
			break;
		case sf::Event::Closed:
			mWindow.close();
			break;
		}

	}
}



/*******************************************************************************************************************************************************************/

void Game::update(sf::Time deltaTime)
// Update what happens in game
{
	// Saving player's coordinates on variables
	xPlayer = Player.sprite.getPosition().x;
	yPlayer = Player.sprite.getPosition().y;

	// Sprite Animations 
	if (clock.getElapsedTime().asSeconds() > 0.1f) {
		Player.animate();
		chiwis.animate(xPlayer, yPlayer, chiwisSpeed);
		sheguis.animate(xPlayer, yPlayer, 2000.f, 2100.f);
		soruya.animate(xPlayer, yPlayer, 3000.f, 700.f);
		mindy.animate(xPlayer, yPlayer, 1816.f, 1466.f);
		bella.animate(xPlayer, yPlayer, 85.f, 790.f);
		manteca.animate(xPlayer, yPlayer, 2366, 2800);
		pushi.animate(yPlayer, yPlayer, 85.f, 1950.f);
		muneca.animate(xPlayer, yPlayer, 1500.f, 2415.f);

		clock.restart();
	}

	// Camera size
	pView.setSize(1000.f, 1000.f);

	// Camera follows player, if player is in the limits of the map, the camera locks 
	// in place to the corresponding boundary (camera boundaires)
	SetCameraView();

	mWindow.setView(pView);

	// Update item and player collision boundaries 
	mItemCollider = mItem.getGlobalBounds();
	mItemArrowCollider = mItemArrow.getGlobalBounds();
	mPlayerCollider = Player.sprite.getGlobalBounds();

	// Sprites with hitboxes.
	chiwis.sprite.setPosition(chiwis.hitbox.getPosition().x, chiwis.hitbox.getPosition().y);
	sheguis.sprite.setPosition(sheguis.hitbox.getPosition().x, sheguis.hitbox.getPosition().y);
	soruya.sprite.setPosition(soruya.hitbox.getPosition().x, soruya.hitbox.getPosition().y);
	mindy.sprite.setPosition(mindy.hitbox.getPosition().x, mindy.hitbox.getPosition().y);
	bella.sprite.setPosition(bella.hitbox.getPosition().x, bella.hitbox.getPosition().y);
	manteca.sprite.setPosition(manteca.hitbox.getPosition().x, manteca.hitbox.getPosition().y);
	pushi.sprite.setPosition(pushi.hitbox.getPosition().x, pushi.hitbox.getPosition().y);
	muneca.sprite.setPosition(muneca.hitbox.getPosition().x, muneca.hitbox.getPosition().y);

	// Check item collision
	CheckItemCollision();

	// Arrow
	if (mItem.getPosition().x == 3070.f && mItem.getPosition().y == 2760.f) 
		Arrow(xPlayer, yPlayer, mItem.getPosition().x, mItem.getPosition().y);
	else 
		Arrow(xPlayer, yPlayer, mItemArrow.getPosition().x, mItemArrow.getPosition().y);
	

	// Print text of pizzas
	this->ContadorPizzas(deliveredPizzas, text, m_font2);

	// Check if pet attacks player
	chiwis.checkBites(bites, quitarVida, deliveredPizzas, mPlayerCollider);
	sheguis.checkBites(bites, quitarVida, deliveredPizzas, mPlayerCollider);
	soruya.checkBites(bites, quitarVida, deliveredPizzas, mPlayerCollider);
	bella.checkBites(bites, quitarVida, deliveredPizzas, mPlayerCollider);
	pushi.checkBites(bites, quitarVida, deliveredPizzas, mPlayerCollider);
	muneca.checkBites(bites, quitarVida, deliveredPizzas, mPlayerCollider);
	mindy.checkBites(bites, quitarVida, deliveredPizzas, mPlayerCollider);
	manteca.checkBites(bites, quitarVida, deliveredPizzas, mPlayerCollider);

	// Pets follow player
	chiwis.followPlayer(Player.sprite, chiwisRadio, chiwisSpeed, deltaTime);
	sheguis.followPlayer(xPlayer, yPlayer, 2000.f, 2100.f, deltaTime);
	soruya.followPlayer(xPlayer, yPlayer, 3000.f, 700.f, deltaTime);
	mindy.followPlayer(xPlayer, yPlayer, 1816.f, 1466.f, deltaTime);
	bella.followPlayer(xPlayer, yPlayer, 85.f, 790.f, deltaTime);
	manteca.followPlayer(xPlayer, yPlayer, 2366.f, 2800.f, deltaTime);
	pushi.followPlayer(xPlayer, yPlayer, 85.f, 1950.f, deltaTime);
	muneca.followPlayer(xPlayer, yPlayer, 1500.f, 2415.f, deltaTime);

	// Check for collisions
	Player.checkCollisions(objects);
	chiwis.checkCollisions(objects, deltaTime, chiwisSpeed);
	sheguis.checkCollisions(objects, deltaTime, 250.f);
	soruya.checkCollisions(objects, deltaTime, 250.f);
	mindy.checkCollisions(objects, deltaTime, 250.f);
	bella.checkCollisions(objects, deltaTime, 250.f);
	manteca.checkCollisions(objects, deltaTime, 250.f);
	pushi.checkCollisions(objects, deltaTime, 250.f);
	muneca.checkCollisions(objects, deltaTime, 250.f);

	//Move Player
	Player.move(deltaTime);

	//Player's life
	this->Player.PlayerLife(quitarVida, xPlayer, yPlayer);
	this->Player.PlayerLifeAux(xPlayer, yPlayer);
}

/*******************************************************************************************************************************************************************/

void Game::render()
{
	mWindow.clear();;
	mWindow.draw(background);
	mWindow.draw(objects);
	mWindow.draw(pizzaLogo);
    mWindow.draw(mItem);
	mWindow.draw(Player.sprite);
	mWindow.draw(mItemArrow);

	if (displayItemPrompt) {
		mWindow.draw(prompt);
	}

	mWindow.draw(Player.playerLifeAux);
	mWindow.draw(Player.playerLife);
	mWindow.draw(chiwis.sprite);
	mWindow.draw(sheguis.sprite);
	mWindow.draw(soruya.sprite);
	mWindow.draw(mindy.sprite);
	mWindow.draw(bella.sprite);
	mWindow.draw(manteca.sprite);
	mWindow.draw(pushi.sprite);
	mWindow.draw(muneca.sprite);
	mWindow.draw(arrow);
	mWindow.draw(textBackground);
	mWindow.draw(text);

	mWindow.display();

}
/*******************************************************************************************************************************************************************/

void Game::Initialize()
{
	// Initialize MainMenu 
	if (!m_font.loadFromFile("Fonts\\ka1.ttf"))
		cout << ("Error al cargar el font.");
	
	if (!m_font2.loadFromFile("Fonts\\font2.ttf"))
		cout << ("Error al cargar el font del item.");
	
	if (!m_menuBackground.loadFromFile("Images\\FondoMainMenu.png"))
		cout << ("Error al cargar el fondo del menu.");
	MainMenu mainMenu(m_font, m_menuBackground);

	// Item
	if (!mItemTexture.loadFromFile("Images\\PizzaBox.png"))
		cout << ("Error al cargar el archivo del Item.");
	mItem.setTexture(mItemTexture);
	mItem.setPosition(3070.f, 2760.f);
	mItem.setScale(1.7f, 1.7f);

	// Arrow's item
	if (!mItemArrowTexture.loadFromFile("Images\\PixelArrowRotated.png"))
		cout << ("Error al cargar el archivo del Item.");
	mItemArrow.setTexture(mItemArrowTexture);
	mItemArrow.setPosition(-1000, -1000);
	mItemArrow.setScale(0.3f, 0.3f);

	// Game Over
	if (!GameOverTex.loadFromFile("Images\\GameOver_.png"))
		cout << ("Error al cargar el archivo de game over.");
	GameOver.setTexture(GameOverTex);
	GameOver.setScale(1.5f, 1.5f);

	// Music 
	if (!music.openFromFile("Audios\\GORILLAvsHUMAN.wav"))
		cout << ("Error al cargar el audio.");
	music.setVolume(10.f);

	if (!menuMusic.openFromFile("Audios\\omairi.wav"))
		cout << ("Error al cargar el audio del menu.");
	menuMusic.setVolume(20.f);

	if (!deathSound.openFromFile("Audios\\death.wav"))
		cout << ("Error al cargar el audio de game over.");

	// Tilemap 
	if (!background.load("Images\\Tileset.png", sf::Vector2u(48, 48), false))
		cout << ("Error al cargar el mapa.");
	if (!objects.load("Images\\Tileset.png", sf::Vector2u(48, 48), true))
		cout << ("Error al cargar los objetos del mapa.");


	// Pizza logo in restaurant
	if (!PizzaLogoTex.loadFromFile("Images\\Pizza.png"))
		cout << ("Error al cargar el archivo de la pizza");

	// Player 
	if (!Player.texture.loadFromFile("Images\\Robot.png"))
		cout << ("Error al cargar el archivo.");
	Player.texRect.left = 0;
	Player.texRect.top = 0;
	Player.texRect.width = 320;
	Player.texRect.height = 320;
	Player.sprite.setTexture(Player.texture);
	Player.sprite.setTextureRect(Player.texRect);
	Player.sprite.setPosition(2500.f, 2500.f);
	Player.sprite.setScale(Player.escaleX, Player.escaleY);

	// Item's texture
	if (!mItemTexture.loadFromFile("Images\\PizzaBox.png"))
		cout << ("Error al cargar el archivo del Item.");
	mItem.setTexture(mItemTexture);
	mItem.setPosition(3070.f, 2760.f);
	mItem.setScale(1.7f, 1.7f);

	if (!mItemArrowTexture.loadFromFile("Images\\PixelArrowRotated.png"))
		cout << ("Error al cargar el archivo del Item.");
	mItemArrow.setTexture(mItemArrowTexture);
	mItem.setPosition(-1000, -1000);

	// Chiwis' texture
	if (!chiwis.texture.loadFromFile("Images\\Mascotas.png")) 
		cout << ("Error al cargar la textura de chiwis.");
	chiwis.texRect.left = 0;
	chiwis.texRect.width = 36;
	chiwis.texRect.height = 47;
	chiwis.sprite.setTexture(chiwis.texture);
	chiwis.sprite.setTextureRect(chiwis.texRect);
	chiwis.sprite.setPosition(3000.f, 700.f);
	chiwis.sprite.setScale(1.5f, 1.5f);

	sf::FloatRect chiwisRect = chiwis.sprite.getGlobalBounds();
	float chiwisWidth = chiwisRect.width;
	float chiwisHeight = chiwisRect.height;

	// Sheguis'texture 
	if (!sheguis.texture.loadFromFile("Images\\Mascotas.png")) 
		cout << ("Error al cargar la textura de sheguis.");
	sheguis.texRect.left = 0;
	sheguis.texRect.width = 36;
	sheguis.texRect.height = 47;
	sheguis.sprite.setTexture(sheguis.texture);
	sheguis.sprite.setTextureRect(sheguis.texRect);
	sheguis.sprite.setPosition(2000.f, 2100.f);
	sheguis.sprite.setScale(1.5f, 1.5f);

	sf::FloatRect sheguisRect = sheguis.sprite.getGlobalBounds();
	float sheguisWidth = sheguisRect.width;
	float sheguisHeight = sheguisRect.height;

	// Soruya's texture
	if (!soruya.texture.loadFromFile("Images\\Mascotas.png")) 
		cout << ("Error al cargar la textura de soruya.");
	soruya.texRect.left = 156;
	soruya.texRect.width = 36;
	soruya.texRect.height = 47;
	soruya.sprite.setTexture(soruya.texture);
	soruya.sprite.setTextureRect(soruya.texRect);
	soruya.sprite.setPosition(3000.f, 700.f);
	soruya.sprite.setScale(1.5f, 1.5f);

	sf::FloatRect soruyaRect = soruya.sprite.getGlobalBounds();
	float soruyaWidth = soruyaRect.width;
	float soruyaHeight = soruyaRect.height;

	// Mindy's texture
	if (!mindy.texture.loadFromFile("Images\\Mascotas.png"))
		cout << ("Error al cargar la textura de mindy.");
	mindy.texRect.left = 156;
	mindy.texRect.width = 36;
	mindy.texRect.height = 47;
	mindy.sprite.setTexture(mindy.texture);
	mindy.sprite.setTextureRect(mindy.texRect);
	mindy.sprite.setPosition(1816.f, 1466.f);
	mindy.sprite.setScale(1.5f, 1.5f);

	sf::FloatRect mindyRect = mindy.sprite.getGlobalBounds();
	float mindyWidth = mindyRect.width;
	float mindyHeight = mindyRect.height;

	// Bella's texture
	if (!bella.texture.loadFromFile("Images\\Mascotas.png"))
		cout << ("Error al cargar la textura de bella.");
	bella.texRect.left = 468;
	bella.texRect.width = 36;
	bella.texRect.height = 47;
	bella.sprite.setTexture(bella.texture);
	bella.sprite.setTextureRect(bella.texRect);
	bella.sprite.setPosition(85.f, 790.f);
	bella.sprite.setScale(1.5f, 1.5f);

	sf::FloatRect bellaRect = bella.sprite.getGlobalBounds();
	float bellaWidth = bellaRect.width;
	float bellaHeight = bellaRect.height;

	// Manteca's texture 
	if (!manteca.texture.loadFromFile("Images\\Mascotas.png"))
		cout << ("Error al cargar la textura de manteca.");
	manteca.texRect.left = 312;
	manteca.texRect.width = 36;
	manteca.texRect.height = 47;
	manteca.sprite.setTexture(manteca.texture);
	manteca.sprite.setTextureRect(manteca.texRect);
	manteca.sprite.setPosition(2366.f, 2800.f);
	manteca.sprite.setScale(1.5f, 1.5f);

	sf::FloatRect mantecaRect = manteca.sprite.getGlobalBounds();
	float mantecaWidth = mantecaRect.width;
	float mantecaHeight = mantecaRect.height;

	// Pushi's texture 
	if (!pushi.texture.loadFromFile("Images\\Mascotas.png")) 
		cout << ("Error al cargar la textura de pushi.");
	pushi.texRect.left = 312;
	pushi.texRect.width = 36;
	pushi.texRect.height = 47;
	pushi.sprite.setTexture(pushi.texture);
	pushi.sprite.setTextureRect(pushi.texRect);
	pushi.sprite.setPosition(85.f, 1950.f);
	pushi.sprite.setScale(1.5f, 1.5f);

	sf::FloatRect pushiRect = pushi.sprite.getGlobalBounds();
	float pushiWidth = pushiRect.width;
	float pushiHeight = pushiRect.height;

	// Muneca's texture
	if (!muneca.texture.loadFromFile("Images\\Mascotas.png"))
		cout << ("Error al cargar la textura de muneca.");
	muneca.texRect.left = 468;
	muneca.texRect.width = 36;
	muneca.texRect.height = 47;
	muneca.sprite.setTexture(muneca.texture);
	muneca.sprite.setTextureRect(muneca.texRect);
	muneca.sprite.setPosition(1500.f, 2415.f);
	muneca.sprite.setScale(1.5f, 1.5f);

	sf::FloatRect munecaRect = muneca.sprite.getGlobalBounds();
	float munecaWidth = munecaRect.width;
	float munecaHeight = munecaRect.height;

	// Item's texture 
	prompt.setFont(m_font2);

	// Hitboxes 
	chiwis.setHitbox(3000.f, 700.f, chiwisWidth, chiwisHeight);
	sheguis.setHitbox(2000.f, 2100.f, sheguisWidth, sheguisHeight);
	soruya.setHitbox(3000.f, 700.f, soruyaWidth, soruyaHeight);
	mindy.setHitbox(1816.f, 1466.f, mindyWidth, mindyHeight);
	bella.setHitbox(85.f, 790.f, bellaWidth, bellaHeight);
	manteca.setHitbox(2366.f, 2800.f, mantecaWidth, mantecaHeight);
	pushi.setHitbox(85.f, 1950.f, pushiWidth, pushiHeight);
	muneca.setHitbox(1500.f, 2415.f, munecaWidth, munecaHeight);

	// Creating array and seed
	MakeArray();
	srand(time(NULL));
}

//*******************************************************************************************************************************************************************

void Game::ContadorPizzas(int cantidad_pizzas, sf::Text& texto, sf::Font& fuente) {

	this->textBackground.setPosition(pView.getCenter().x - 450.f, pView.getCenter().y - 450.f);
	this->textBackground.setSize(sf::Vector2f(260.f, 30.f));
	this->textBackground.setFillColor(sf::Color::Blue);
	this->textBackground.setOutlineColor(sf::Color::Black);
	this->textBackground.setOutlineThickness(4.f);
	this->text.setFont(fuente); // "fuente" is the object of the font you want to use.
	this->text.setCharacterSize(20); // adjust the size of the text for our neccesities.
	this->text.setPosition(pView.getCenter().x - 450.f, pView.getCenter().y - 450.f);
	this->text.setString("Pizzas entregadas: " + std::to_string(cantidad_pizzas));
}

void Game::Arrow(float xPlayer, float yPlayer, float xItem, float yItem) {

	float dx = xItem - xPlayer;
	float dy = yItem - yPlayer;
	float angle = atan2(dy, dx) * 180 / 3.141592 - 90; 
	this->arrow.setRotation(angle); 
	float arrowOffset = -50.f; 

	this->arrow.setPointCount(3);
	this->arrow.setPoint(0, sf::Vector2f(xPlayer, yPlayer));
	this->arrow.setPoint(1, sf::Vector2f(xPlayer - 10.f, yPlayer - 10.f));
	this->arrow.setPoint(2, sf::Vector2f(xPlayer + 10.f, yPlayer - 10.f));
	this->arrow.setFillColor(sf::Color::Yellow);
	this->arrow.setOrigin(xPlayer, yPlayer - 20.f);
	this->arrow.setScale(1.f,1.f); 
	this->arrow.setOutlineThickness(2.f);
	this->arrow.setOutlineColor(sf::Color::Black);

	float circleRadius = 80.f; 
	sf::Vector2f circleCenter(xPlayer + 15.f, yPlayer + 20.f);

	float angleRadians = (angle + 90) * 3.141592 / 180; 
	sf::Vector2f arrowPosition(cos(angleRadians) * circleRadius, sin(angleRadians) * circleRadius);

	arrowPosition += circleCenter;

	this->arrow.setPosition(arrowPosition.x, arrowPosition.y);
}

void Game::CheckItemCollision() {
	displayItemPrompt = false;
	if (mPlayerCollider.intersects(mItemCollider) || mPlayerCollider.intersects(mItemArrowCollider)) {

		if (mItem.getPosition().x == 3070.f && mItem.getPosition().y == 2760.f) {
			prompt.setString("Presiona espacio para recoger la pizza!");
		}
		else {
			prompt.setString("Presiona espacio para dejar la pizza!");
		}
		displayItemPrompt = true;
		if (cargandoItem) {
			prompt.setPosition(mItemArrow.getPosition().x - 300, mItemArrow.getPosition().y + 100);
		}
		else {
			prompt.setPosition(mItem.getPosition().x - 300, mItem.getPosition().y + 100);
		}

		if (sf::Keyboard::isKeyPressed(teclaItem)) {
			if (cargandoItem) {
				quitarVida = 0.f;
				bites = 0.f;
				mItemArrow.setPosition(-1000, -1000);
				mItem.setPosition(3070.f, 2760.f);
				mItem.setScale(1.7f, 1.7f);
				cargandoItem = false;
				deliveredPizzas++;
			}
			else {
				cargandoItem = true;
				NuevaPosicion = rand() % 35;
				mItemArrow.setPosition(PosicionesItem[0][NuevaPosicion], PosicionesItem[1][NuevaPosicion]);
				mItemArrow.setScale(0.3f, 0.3f);
				mItem.setPosition(-1000, -1000);
			}
		}
	}
}

void Game::SetCameraView() {
	// Check corners only
	if (xPlayer < camLeftBound && yPlayer < camUpperBound)
		pView.setCenter(camLeftBound, camUpperBound);
	else if (xPlayer > camRightBound && yPlayer < camUpperBound)
		pView.setCenter(camRightBound, camUpperBound);
	else if (xPlayer < camLeftBound && yPlayer > camLowerBound)
		pView.setCenter(camLeftBound, camLowerBound);
	else if (xPlayer > camRightBound && yPlayer > camLowerBound)
		pView.setCenter(camRightBound, camLowerBound);

	// Check sides only
	else if (xPlayer < camLeftBound)
		pView.setCenter(camLeftBound, yPlayer);
	else if (xPlayer > camRightBound)
		pView.setCenter(camRightBound, yPlayer);
	else if (yPlayer > camLowerBound)
		pView.setCenter(xPlayer, camLowerBound);
	else if (yPlayer < camUpperBound)
		pView.setCenter(xPlayer, camUpperBound);
	else
		pView.setCenter(xPlayer, yPlayer);
}




