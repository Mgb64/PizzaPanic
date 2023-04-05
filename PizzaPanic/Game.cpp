// basado en el libro SFML Game Development :)

#include <iostream>
#include <cmath>
#include "Game.h"
//#include "TileMap.h"


using std::cout;


#define TimePerFrame sf::seconds(1.f / 60.f)


Game::Game()
	: mWindow(sf::VideoMode(800, 600), "Pizza Panic")
	, mTexture()
	, mPlayer()
	, pView(sf::FloatRect(0.f, 0.f, mWindow.getSize().x, mWindow.getSize().y))
{
	// Player
	if (!mTexture.loadFromFile("C:\\Users\\Manuel\\Pictures\\Images\\robot-idle.gif"))
	{
		// Handle loading error
		cout << ("Error al cargar el archivo.");
	}
	mPlayer.setTexture(mTexture);
	mPlayer.setPosition(700.f, 700.f);
	mPlayer.setScale(0.3f, 0.3f);
	this->HitBoxPlayer();
	this->HitBoxChiwis();
	this->HitBoxSheguis();
	this->HitBoxSoruya();
	this->HitBoxMindy();

	// Tilemap
	if (!background.load("C:\\Users\\Manuel\\Pictures\\Images\\Background.png", sf::Vector2u(48, 48), level, 70, 70))
	{
		// Handle loading error
		cout << ("Error al cargar el mapa.");
	}

	// Houses, trees, ...
	if (!objects.load("C:\\Users\\Manuel\\Pictures\\Images\\Objects.png", sf::Vector2u(48, 48), objectDisplay, 70, 70))
	{
		// Handle loading error
		cout << ("Error al cargar los objetos del mapa.");
	}

	// Music
	if (!music.openFromFile("C:\\Users\\Manuel\\Music\\Audios")) // canci�n de prueba nom�s
	{
		// Handle loading error
		cout << ("Error al cargar el audio.");
	}
	music.play();
	music.setVolume(20.f);

}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		processEvents();
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;
			processEvents();
			update(TimePerFrame);
		}
		render();
	}
}

void Game::processEvents()
// User input
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;
		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;
		case sf::Event::Closed:
			mWindow.close();
			break;
		}
	}
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	if (key == sf::Keyboard::W)
		mIsMovingUp = isPressed;
	else if (key == sf::Keyboard::S)
		mIsMovingDown = isPressed;
	else if (key == sf::Keyboard::A)
		mIsMovingLeft = isPressed;
	else if (key == sf::Keyboard::D)
		mIsMovingRight = isPressed;
}

void Game::update(sf::Time deltaTime)
// Update what happens in game
{
	sf::Vector2f movement(0.f, 0.f);
	

	if (mIsMovingUp)
		movement.y -= PlayerSpeed;
	if (mIsMovingDown)
		movement.y += PlayerSpeed;
	if (mIsMovingLeft)
		movement.x -= PlayerSpeed;
	if (mIsMovingRight)
		movement.x += PlayerSpeed;

	// Camera follows player
	pView.setCenter(mPlayer.getPosition());
	//pView.setSize(900.f, 900.f);
	pView.setSize(1300.f, 1300.f);
	mWindow.setView(pView);

	// Calculate the distance between the enemy and the player
	float dxchiwis = mPlayer.getPosition().x - hitboxchiwis.getPosition().x;
	float dychiwis = mPlayer.getPosition().y - hitboxchiwis.getPosition().y;
	float distancechiwis = sqrt(pow(dxchiwis,2) + pow(dychiwis,2));

	float dxsheguis = mPlayer.getPosition().x - hitboxsheguis.getPosition().x;
	float dysheguis = mPlayer.getPosition().y - hitboxsheguis.getPosition().y;
	float distancesheguis = sqrt(pow(dxsheguis, 2) + pow(dysheguis, 2));

	float dxsoruya = mPlayer.getPosition().x - hitboxsoruya.getPosition().x;
	float dysoruya = mPlayer.getPosition().y - hitboxsoruya.getPosition().y;
	float distancesoruya = sqrt(pow(dxsoruya, 2) + pow(dysoruya, 2));

	float dxmindy = mPlayer.getPosition().x - hitboxmindy.getPosition().x;
	float dymindy = mPlayer.getPosition().y - hitboxmindy.getPosition().y;
	float distancemindy = sqrt(pow(dxmindy, 2) + pow(dymindy, 2));

	// Calculate the unit vector from the enemy to the player
	sf::Vector2f unitVectorchiwis(dxchiwis / distancechiwis, dychiwis / distancechiwis);
	sf::Vector2f unitVectorsheguis(dxsheguis / distancesheguis, dysheguis / distancesheguis);
	sf::Vector2f unitVectorsoruya(dxsoruya / distancesoruya, dysoruya / distancesoruya);
	sf::Vector2f unitVectormindy(dxmindy / distancemindy, dymindy / distancemindy);

	// Calculate the velocity vector
	sf::Vector2f velocitychiwis = unitVectorchiwis * (ChiwisSpeed);
	sf::Vector2f velocitysheguis = unitVectorsheguis * (SheguisSpeed);
	sf::Vector2f velocitysoruya = unitVectorsoruya * (SoruyaSpeed);
	sf::Vector2f velocitymindy = unitVectormindy * (MindySpeed);

	mPlayer.move(movement * deltaTime.asSeconds());
	hitboxplayer.move(movement * deltaTime.asSeconds());
	hitboxchiwis.move(velocitychiwis * deltaTime.asSeconds());
	hitboxsheguis.move(velocitysheguis * deltaTime.asSeconds());
	hitboxsoruya.move(velocitysoruya * deltaTime.asSeconds());
	hitboxmindy.move(velocitymindy * deltaTime.asSeconds());

	
}

void Game::HitBoxPlayer()
{
	this->hitboxplayer.setPosition(720.f, 725.f);
	this->hitboxplayer.setSize(sf::Vector2f(55.f, 75.f));
	this->hitboxplayer.setFillColor(sf::Color::Transparent);
	this->hitboxplayer.setOutlineColor(sf::Color::Red);
	this->hitboxplayer.setOutlineThickness(6.f);
}

void Game::HitBoxChiwis()
{
	this->hitboxchiwis.setPosition(500.f, 500.f);
	this->hitboxchiwis.setSize(sf::Vector2f(55.f, 75.f));
	this->hitboxchiwis.setFillColor(sf::Color::Transparent);
	this->hitboxchiwis.setOutlineColor(sf::Color::Cyan);
	this->hitboxchiwis.setOutlineThickness(6.f);
}

void Game::HitBoxSheguis()
{
	this->hitboxsheguis.setPosition(500.f, 1000.f);
	this->hitboxsheguis.setSize(sf::Vector2f(55.f, 75.f));
	this->hitboxsheguis.setFillColor(sf::Color::Transparent);
	this->hitboxsheguis.setOutlineColor(sf::Color::White);
	this->hitboxsheguis.setOutlineThickness(6.f);
}

void Game::HitBoxSoruya()
{
	this->hitboxsoruya.setPosition(1000.f, 1000.f);
	this->hitboxsoruya.setSize(sf::Vector2f(55.f, 75.f));
	this->hitboxsoruya.setFillColor(sf::Color::Transparent);
	this->hitboxsoruya.setOutlineColor(sf::Color::Black);
	this->hitboxsoruya.setOutlineThickness(6.f);
}

void Game::HitBoxMindy()
{
	this->hitboxmindy.setPosition(150.f, 200.f);
	this->hitboxmindy.setSize(sf::Vector2f(55.f, 75.f));
	this->hitboxmindy.setFillColor(sf::Color::Transparent);
	this->hitboxmindy.setOutlineColor(sf::Color::Red);
	this->hitboxmindy.setOutlineThickness(6.f);
}



void Game::render()
{
	mWindow.clear();
	mWindow.draw(background);
	mWindow.draw(objects);
	mWindow.draw(mPlayer);
	mWindow.draw(hitboxplayer);
	mWindow.draw(hitboxchiwis);
	mWindow.draw(hitboxsheguis);
	mWindow.draw(hitboxsoruya);
	mWindow.draw(hitboxmindy);
	mWindow.display();

}


