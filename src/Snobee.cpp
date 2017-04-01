#include "Snobee.h"

Snobee::Snobee(glm::vec2 pos, glm::vec2 size, GLfloat velocity, const Texture& initialSprite, SnobeeType type)
    : GameObject(pos, size, velocity, initialSprite, false, SHAPE_CIRCLE_SMALL), type(type), isMoving(false), isNumb(0)
{
}

//Snobee::move() {
//    if (!isMoving) {
//        switch(this->type) {
//            case GREEN:
//                break;
//            case YELLOW:
//                break;
//            case RED:
//                break;
//        }
//    }
//}

Snobee::~Snobee() {

}

void Snobee::numb(GLboolean isNumb) {
	this->isNumb = isNumb;
}