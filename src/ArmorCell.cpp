#include "ArmorCell.h"
#include "CorpseCell.h"

ArmorCell::ArmorCell() :
    Cell("cell_armor.png")
{
    type_ = Type::Armor;
}

void ArmorCell::onCollision(Cell* other)
{
    if (other->getType() == Type::Thorn || other->getType() == Type::Carnivore)
        other->markForDeletion();
}

CorpseCell* ArmorCell::createCorpse() const
{
    return new CorpseCell(1.0f, localPos_);
}

char ArmorCell::getSymbol()
{
    return 'A';
}
