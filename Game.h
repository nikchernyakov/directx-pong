#pragma once
#include <d3d11_1.h>
#include "InputDevice.h"
#include "RectObject.h"
#include "BallObject.h"
#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

class Game
{
public:
	Game();
	~Game();

	void init();
	
	float deltaTime = 0;
	float currentTime = 0;

	int screenWidth = 0;
	int screenHeight = 0;

	HWND hWnd;

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* rtv;
	ID3DUserDefinedAnnotation* annotation;

	InputDevice* inputDevice;
	
	void doFrame();
	
	const float PLAYER_SPEED = 2.0f;
	const float PLAYER_OFFSET_POSITION_X = 0.05f;
	
	const float BOUND_SIZE = 1.0f;
	const float BOUND_OFFSET = 0.05f;
	
	const float GATE_SIZE = 0.5f;
	const float GATE_OFFSET = 0.05f;
	
	const float BALL_SPEED = 500.0f;
	const float BALL_SPEED_MULTIPLIER = 1.1f;
	const float BALL_SPEED_MULTIPLIER_TIME = 3.0f;
	float BALL_SPEED_X;
	float BALL_SPEED_Y;
	
	const float BLINK_TIME = 2.0f;
	const float WAIT_TIME = 2.0f;

	float checkPlayerPosition(RectObject* player, float newPosY);
	void updateBallPosition();
	void checkBallWithBoundCollision(Pos newPos);
	bool checkPointBetweenSide(float p, float a, float b);
	bool checkSidesCollision(float a1, float b1, float a2, float b2);
	void checkBallWithPlayerCollision(RectObject* player, Pos newPos);
	bool checkPointWithSphereCollision(Pos sphere, Pos p);
	void movePlayer(RectObject* player, int moveDirection);
	void goal(bool isLeftGoal);
	
private:
	RectObject* player1;
	RectObject* player2;
	RectObject* topBorder;
	RectObject* bottomBorder;
	RectObject* leftGate;
	RectObject* rightGate;
	BallObject* ball;

	Vector2* ballDirection;

	bool isGoal = false;
	bool isLeftGoal;
	bool isBlinking;
	bool isBlinkingUp;
	bool isWaiting;
	float currentBlinkTime;
	float currentBlinkValue;
	float currentWaitTime;

	float currentBallSpeedMultiplierTime;
	
	void update();
	void render();
	void drawObjects();
};

