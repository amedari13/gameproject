#include "level.h"
#include <Box2D\Box2D.h>

#include <iostream>
#include <random>

#include "Game.h"

#include <filesystem>

#pragma comment(lib,"Box2D.lib")

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"gdi32.lib")    

#pragma comment(lib,"sfml-graphics-s-d.lib")
#pragma comment(lib,"sfml-window-s-d.lib")
#pragma comment(lib,"sfml-system-s-d.lib")


std::string search_file(std::string file)
{
	for (int index = 3; index-- > 0; file = "..\\" + file)
	{
		if (std::filesystem::exists(file))
			break;
	}
	return file;
}


int main()
{
	std::shared_ptr<Object> player;
	b2Body* playerBody;

	std::vector<std::shared_ptr<Object>> coins;
	std::vector<b2Body*> coinBody;

	std::vector<std::shared_ptr<Object>> enemies;
	std::vector<b2Body*> enemyBody;
	
	srand(time(NULL));
	
	Level lvl;

	if (!lvl.LoadFromFile(search_file("platformer.tmx")))
	{
		std::cout << "cannot load .tmx" << std::endl;
		return -1;
	}


	b2Vec2 gravity(0.0f, 1.0f);
	b2World world(gravity);

	sf::Vector2i tileSize = lvl.GetTileSize();
	 
	std::vector<std::shared_ptr<Object>> blocks = lvl.GetObjects("block");
	for(auto block: blocks)
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_staticBody;
		bodyDef.position.Set(
			block->rect.left + tileSize.x / 2 * (block->rect.width / tileSize.x - 1),
			block->rect.top + tileSize.y / 2 * (block->rect.height / tileSize.y - 1));
		b2Body* body = world.CreateBody(&bodyDef);
		b2PolygonShape shape;
		shape.SetAsBox(block->rect.width / 2, block->rect.height / 2);
		body->CreateFixture(&shape, 1.0f);
	}

	coins = lvl.GetObjects("coin");
	for(auto coin: coins)
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(
			coin->rect.left + tileSize.x / 2 * (coin->rect.width / tileSize.x - 1),
			coin->rect.top + tileSize.y / 2 * (coin->rect.height / tileSize.y - 1));
		bodyDef.fixedRotation = true;
		b2Body* body = world.CreateBody(&bodyDef);
		b2PolygonShape shape;
		shape.SetAsBox(coin->rect.width / 2, coin->rect.height / 2);
		body->CreateFixture(&shape, 1.0f);
		coinBody.push_back(body);
	}

	enemies = lvl.GetObjects("enemy");
	for (auto enemy: enemies)
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(
			enemy->rect.left +
			tileSize.x / 2 * (enemy->rect.width / tileSize.x - 1),
			enemy->rect.top + tileSize.y / 2 * (enemy->rect.height / tileSize.y - 1));
		bodyDef.fixedRotation = true;
		b2Body* body = world.CreateBody(&bodyDef);
		b2PolygonShape shape; 
		shape.SetAsBox(enemy->rect.width / 2, enemy->rect.height / 2);
		body->CreateFixture(&shape, 1.0f);
		enemyBody.push_back(body);
	}


	player = lvl.GetObject("player");
	assert(player);

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(player->rect.left, player->rect.top);
	bodyDef.fixedRotation = true;
	playerBody = world.CreateBody(&bodyDef);
	b2PolygonShape shape; shape.SetAsBox(player->rect.width / 2, player->rect.height / 2);
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f; fixtureDef.friction = 0.6f;
	fixtureDef.restitution=0.2f;//добавила прыгучесть и поменяла силу трения
	playerBody->CreateFixture(&fixtureDef);
	
	sf::Vector2i screenSize(800, 600);

	sf::RenderWindow window;
	window.create(sf::VideoMode(screenSize.x, screenSize.y), "Game");


	sf::View view;
	view.reset(sf::FloatRect(0.0f, 0.0f, screenSize.x, screenSize.y));
	view.setViewport(sf::FloatRect(0.0f, 0.0f, 2.0f, 2.0f));

	while (window.isOpen())
	{
		sf::Event evt;

		while (window.pollEvent(evt))
		{
			switch (evt.type)
			{
			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::KeyPressed:

				if(evt.key.code == sf::Keyboard::W && playerBody->GetLinearVelocity().x > 0
					&&playerBody->GetLinearVelocity().y == 0)
					playerBody->SetLinearVelocity(b2Vec2(7.0f, -15.0f)); //по диагонали вправо

				if (evt.key.code == sf::Keyboard::W && playerBody->GetLinearVelocity().x < 0
					&& playerBody->GetLinearVelocity().y == 0)
					playerBody->SetLinearVelocity(b2Vec2(-7.0f, -15.0f)); //по диагонали влево

				if (evt.key.code == sf::Keyboard::S)
					playerBody->SetLinearVelocity(b2Vec2(0.0f, 20.0f)); //вниз

				if ((evt.key.code == sf::Keyboard::W || evt.key.code == sf::Keyboard::Space)
					&& playerBody->GetLinearVelocity().y == 0) 
					playerBody->SetLinearVelocity(b2Vec2(0.0f, -15.0f)); //вверх
				
				if (evt.key.code == sf::Keyboard::D) 
					playerBody->SetLinearVelocity(b2Vec2(7.0f, 0.0f)); //вправо
				
				if (evt.key.code == sf::Keyboard::A) 
					playerBody->SetLinearVelocity(b2Vec2(-7.0f, 0.0f));//влево
								
				break;
			}
		}

		world.Step(1.0f / 60.0f, 1, 1);

		bool stop = false;
		for (b2ContactEdge* ce = playerBody->GetContactList(); ce && !stop; ce = ce->next)
		{
			b2Contact* c = ce->contact;

			for (int i = 0; i < coinBody.size(); i++)
				if (c->GetFixtureA() == coinBody[i]->GetFixtureList())
				{
					coinBody[i]->DestroyFixture(coinBody[i]->GetFixtureList());
					coins.erase(coins.begin() + i);
					coinBody.erase(coinBody.begin() + i);
					stop = true; 
					break;
				}

			for (int i = 0; i < enemyBody.size(); i++)
				if (c->GetFixtureA() == enemyBody[i]->GetFixtureList())
				{
					if (playerBody->GetPosition().y < enemyBody[i]->GetPosition().y)
					{
						playerBody->SetLinearVelocity(b2Vec2(0.0f, -10.0f));

						enemyBody[i]->DestroyFixture(enemyBody[i]->GetFixtureList());
						enemies.erase(enemies.begin() + i);
						enemyBody.erase(enemyBody.begin() + i);
						stop = true;
						break;
					}
					else
					{
						int tmp = (playerBody->GetPosition().x < enemyBody[i]->GetPosition().x)
							? -1 : 1;
						playerBody->SetLinearVelocity(b2Vec2(10.0f * tmp, 0.0f));
					}
				}
		}

		for (int i = 0; i < enemyBody.size(); i++)
		{
			if (enemyBody[i]->GetLinearVelocity() == b2Vec2_zero)
			{
				int tmp = (rand() % 2 == 1) ? 1 : -1;
				enemyBody[i]->SetLinearVelocity(b2Vec2(5.0f * tmp, 0.0f));
			}
		}

		b2Vec2 pos = playerBody->GetPosition();
		view.setCenter(pos.x + screenSize.x / 4, pos.y + screenSize.y / 4);
		window.setView(view);

		player->sprite.setPosition(pos.x, pos.y);

		for (int i = 0; i < coins.size(); i++)
			coins[i]->sprite.setPosition(coinBody[i]->GetPosition().x, coinBody[i]->GetPosition().y);

		for (int i = 0; i < enemies.size(); i++)
			enemies[i]->sprite.setPosition(enemyBody[i]->GetPosition().x, enemyBody[i]->GetPosition().y);

		window.clear();

		lvl.Draw(window);

		window.draw(player->sprite);

		for (int i = 0; i < coins.size(); i++)
			window.draw(coins[i]->sprite);

		for (int i = 0; i < enemies.size(); i++)
			window.draw(enemies[i]->sprite);

		window.display();
	}

	return 0;
}


