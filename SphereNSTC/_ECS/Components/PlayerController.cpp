#include "PlayerController.h"

#include "_inputs/InputHandler.h"
#include "Transform.h"
#include "_ECS/GameObject.h"
#include "_utils/Time.h"

PlayerController::PlayerController(float movementSpeed, float rotationSpeed) {
  this->movementSpeed = movementSpeed;
  this->rotationSpeed = rotationSpeed;

  InputHandler::registerPlayerController(this);
}

PlayerController::~PlayerController() {
  InputHandler::forgetPlayerController(this);
}
