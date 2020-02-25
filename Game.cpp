#include "Game.h"
#include "RectObject.h"
#include <iostream>

Game::Game()
{
	inputDevice = new InputDevice();
}

Game::~Game()
{

}

void Game::init()
{
	player1 = new RectObject(this, new Vertex{ -GATE_SIZE + GATE_OFFSET, 0.0f, 255, 0, 0, 1 }, 0.03f, 0.4f);
	leftGate = new RectObject(this, new Vertex{ -GATE_SIZE, 0.0f, 255, 0, 0, 1 }, 0.01f, 2.0f);
	
	player2 = new RectObject(this, new Vertex{ GATE_SIZE - GATE_OFFSET, 0.0f, 0, 0, 255, 1 }, 0.03f, 0.4f);
	rightGate = new RectObject(this, new Vertex{ GATE_SIZE, 0.0f, 0, 0, 255, 1 }, 0.01f, 2.0f);

	topBorder = new RectObject(this, new Vertex{ 0.0f, BOUND_SIZE / 2, 128, 128, 128, 1 }, 2.0f, BOUND_OFFSET * 2);
	bottomBorder = new RectObject(this, new Vertex{ 0.0f, -BOUND_SIZE / 2, 128, 128, 128, 1 }, 2.0f, BOUND_OFFSET * 2);
	
	ball = new BallObject(this, new Vertex{ 0.0f, 0.0f, 255, 255, 255, 1 }, 30.0f);
	ballDirection = new Vector2{ 1.0, 1.0 };

	BALL_SPEED_X = BALL_SPEED / screenWidth;
	BALL_SPEED_Y = BALL_SPEED / screenHeight;
}

void Game::doFrame()
{
	update();
	render();
}

void Game::update()
{
	if (isGoal)
	{
		if (isBlinking)
		{
			if (currentBlinkTime > BLINK_TIME)
			{
				isBlinking = false;
				isWaiting = true;

				// Reset ball
				ball->setPosition({ 0, 0 });
				BALL_SPEED_X = BALL_SPEED / screenWidth;
				BALL_SPEED_Y = BALL_SPEED / screenHeight;
				currentBallSpeedMultiplierTime = 0;
			}
			else
			{
				currentBlinkTime += deltaTime;
				if (isBlinkingUp)
				{
					if (currentBlinkTime > BLINK_TIME / 2)
					{
						isBlinkingUp = false;
					}
				}
				currentBlinkValue = (isBlinkingUp ? currentBlinkTime : BLINK_TIME - currentBlinkTime) / BLINK_TIME;
			}
			return;
		}
		if (isWaiting)
		{
			currentWaitTime += deltaTime;
			if (currentWaitTime > WAIT_TIME)
			{
				isWaiting = false;
				isGoal = false;
			}
		}
	}
	
	// Player 1 movement
	int moveDirection = inputDevice->KeyIsPressed('W') ? 1
	: (inputDevice->KeyIsPressed('S') ? -1 : 0);
	movePlayer(player1, moveDirection);

	// Player 2 movement
	moveDirection = inputDevice->KeyIsPressed(VK_UP) ? 1
		: (inputDevice->KeyIsPressed(VK_DOWN) ? -1 : 0);
	movePlayer(player2, moveDirection);

	if (isGoal)
	{
		return;
	}

	if (currentBallSpeedMultiplierTime > BALL_SPEED_MULTIPLIER_TIME)
	{
		BALL_SPEED_X *= BALL_SPEED_MULTIPLIER;
		BALL_SPEED_Y *= BALL_SPEED_MULTIPLIER;
		currentBallSpeedMultiplierTime = 0;
	}
	currentBallSpeedMultiplierTime += deltaTime;
	updateBallPosition();
}

void Game::render()
{
	if (isGoal && isBlinking)
	{
		float color[] = { !isLeftGoal ? currentBlinkValue : 0.0f,
			0.0f, isLeftGoal ? currentBlinkValue : 0.0f, 1.0f };
		context->ClearRenderTargetView(rtv, color);
		annotation->BeginEvent(L"BeginDraw");
		annotation->EndEvent();
		swapChain->Present(1, 0);
		return;
	}
	
	float color[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->ClearRenderTargetView(rtv, color);

	annotation->BeginEvent(L"BeginDraw");
	drawObjects();
	annotation->EndEvent();
	
	swapChain->Present(1, 0);
}

void Game::drawObjects()
{
	player1->draw();
	player2->draw();

	leftGate->draw();
	rightGate->draw();

	topBorder->draw();
	bottomBorder->draw();
	
	ball->draw();
}

float Game::checkPlayerPosition(RectObject* player, float newPosY)
{
	if((newPosY - player->getHeight() / 2) < (-BOUND_SIZE + BOUND_OFFSET))
	{
		return -BOUND_SIZE + BOUND_OFFSET + player->getHeight() / 2;
	}
	else if ((newPosY + player->getHeight() / 2) > (BOUND_SIZE - BOUND_OFFSET))
	{
		return BOUND_SIZE - BOUND_OFFSET - player->getHeight() / 2;
	}
	else
	{
		return newPosY;
	}		
}

void Game::updateBallPosition()
{
	Pos newPos = { (ball->getPosition().x + ballDirection->x * BALL_SPEED_X * deltaTime) / 2,
		ball->getPosition().y + ballDirection->y * BALL_SPEED_Y * deltaTime };

	if (newPos.x + (ball->getRadiusX() / 2) > GATE_SIZE)
	{
		std::cout << "Right gate collision\n";
		goal(false);
	}
	if (newPos.x - (ball->getRadiusX() / 2) < -GATE_SIZE)
	{
		std::cout << "Left gate collision\n";
		goal(true);
	}

	checkBallWithBoundCollision(newPos);

	checkBallWithPlayerCollision(player1, newPos);
	checkBallWithPlayerCollision(player2, newPos);
	
	ballDirection->Normalize();
	ball->setPosition({ ball->getPosition().x + ballDirection->x * BALL_SPEED_X * deltaTime,
		ball->getPosition().y + ballDirection->y * BALL_SPEED_Y * deltaTime });
	
	//std::cout << ball->getPosition().x << " " << ball->getPosition().y << "\n";
}

void Game::checkBallWithBoundCollision(Pos newPos)
{
	if (newPos.y + ball->getRadiusY() / 2 > BOUND_SIZE - BOUND_OFFSET
		|| newPos.y - ball->getRadiusY() / 2 < -BOUND_SIZE + BOUND_OFFSET)
	{
		ballDirection->y = -ballDirection->y;
		std::cout << "Bound collision\n";
	}	
}

bool Game::checkPointBetweenSide(float p, float a, float b)
{
	return p > a && p < b;
}

bool Game::checkSidesCollision(float a1, float b1, float a2, float b2)
{
	return checkPointBetweenSide(a1, a2, b2)
		|| checkPointBetweenSide(b1, a2, b2);
}

void Game::checkBallWithPlayerCollision(RectObject* player, Pos newPos)
{
	Pos pLeftTop = player->getLeftTop();
	Pos pRightTop = player->getRightTop();
	Pos pLeftBottom = player->getLeftBottom();
	Pos pRightBottom = player->getRightBottom();
	const float leftX = pLeftTop.x;
	const float rightX = pRightTop.x;
	const float bottomY = pLeftBottom.y;
	const float topY = pLeftTop.y;

	bool sideCollision = checkSidesCollision(newPos.x - ball->getRadiusX() / 2, newPos.x + ball->getRadiusX() / 2, leftX, rightX)
		&& checkSidesCollision(newPos.y - ball->getRadiusY() / 2, newPos.y + ball->getRadiusY() / 2, bottomY, topY);
	// Side collision
	if (sideCollision)
	{
		//std::cout << "Side collision\n";
		if (newPos.y > topY || newPos.y < bottomY)
		{
			ballDirection->y = -ballDirection->y;
		}
		else
		{
			ballDirection->x = -ballDirection->x;
		}
	}

	// Point collision
	if (checkPointWithSphereCollision(newPos, pLeftTop)
		|| checkPointWithSphereCollision(newPos, pRightTop)
		|| checkPointWithSphereCollision(newPos, pLeftBottom)
		|| checkPointWithSphereCollision(newPos, pRightBottom))
	{
		ballDirection->x = -ballDirection->x;
		ballDirection->y = -ballDirection->y;
	}
}

bool Game::checkPointWithSphereCollision(Pos sphere, Pos p)
{
	return (pow(sphere.x - p.x, 2) / pow(ball->getRadiusX() / 2, 2)
		+ pow(sphere.y - p.y, 2) / pow(ball->getRadiusY() / 2, 2)) < 1;
}

void Game::movePlayer(RectObject* player, const int moveDirection)
{
	// Check if player moved
	if (moveDirection != 0)
	{
		float newPositionY = player->getPosition().y + deltaTime * moveDirection * PLAYER_SPEED;
		newPositionY = checkPlayerPosition(player, newPositionY);
		player->setPositionY(newPositionY);
	}
}

void Game::goal(bool leftGoal)
{
	isGoal = true;
	isLeftGoal = leftGoal;

	isBlinking = true;
	isBlinkingUp = true;
	currentBlinkTime = 0;
	currentBlinkValue = 0;

	isWaiting = false;
	currentWaitTime = 0;
}
