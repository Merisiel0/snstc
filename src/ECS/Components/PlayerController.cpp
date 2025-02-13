#include "PlayerController.h"

#include "inputs/InputHandler.h"
#include "Transform.h"
#include "ECS/GameObject.h"
#include "utils/Time.h"

PlayerController::PlayerController(float movementSpeed, float rotationSpeed) {
  this->movementSpeed = movementSpeed;
  this->rotationSpeed = rotationSpeed;

  InputHandler::registerPlayerController(this);
}

PlayerController::~PlayerController() {
  InputHandler::forgetPlayerController(this);
}
