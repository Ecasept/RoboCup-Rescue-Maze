#include "common.h"
#include "error.h"
#include <Arduino.h>

CardDir toCard(RelDir relDir, CardDir orientation) {
    return static_cast<CardDir>(
        (static_cast<int>(relDir) + static_cast<int>(orientation)) % 4);
}

RelDir toRel(CardDir cardDir, CardDir orientation) {
    return static_cast<RelDir>(
        (static_cast<int>(cardDir) - static_cast<int>(orientation) + 4) % 4);
}

CardDir opposite(CardDir cardDir) {
    switch (cardDir) {
    case CardDir::Up:
        return CardDir::Down;
    case CardDir::Right:
        return CardDir::Left;
    case CardDir::Down:
        return CardDir::Up;
    case CardDir::Left:
        return CardDir::Right;
    }
}

RelDir opposite(RelDir relDir) {
    switch (relDir) {
    case RelDir::Front:
        return RelDir::Back;
    case RelDir::Right:
        return RelDir::Left;
    case RelDir::Back:
        return RelDir::Front;
    case RelDir::Left:
        return RelDir::Right;
    }
}

void shift(int &x, int &y, CardDir dir) {
    switch (dir) {
    case CardDir::Up:
        y--;
        break;
    case CardDir::Right:
        x++;
        break;
    case CardDir::Down:
        y++;
        break;
    case CardDir::Left:
        x--;
        break;
    default:
        raise("Tried to shift values in invalid direction");
    }
}

String dstr(RelDir dir) {
    switch (dir) {
    case RelDir::Front:
        return "Front";
    case RelDir::Right:
        return "Right";
    case RelDir::Back:
        return "Back";
    case RelDir::Left:
        return "Left";
    }
}

String dstr(CardDir dir) {
    switch (dir) {
    case CardDir::Up:
        return "Up";
    case CardDir::Right:
        return "Right";
    case CardDir::Down:
        return "Down";
    case CardDir::Left:
        return "Left";
    }
}

float minimum(float a, float b, float c, float d, float e) {
    float smallest = a;
    if (b<smallest) smallest = b;
    if (c<smallest) smallest = c;
    if (d<smallest) smallest = d;
    if (e<smallest) smallest = e;
    return smallest;
}

bool getTurnDir(int a, int b) {
    int diff = b-a;
    if (diff < -180) return true;
    if (diff < 0) return false;
    if (diff < 180) return true;
    else {return false;}
}

double getDegrees(CardDir robotDir, RelDir relDir) {
    int cardinalOffset = static_cast<int>(robotDir);
    int relativeOffset = static_cast<int>(relDir);

    int goalOffset = (cardinalOffset + relativeOffset) % 4;

    return goalOffset * 90.0;
}

void shiftArray(int array[], int arrLen) {
    for (int i = 1; i < arrLen; i++) {
        array[i-1] = array[i];
    }
}
