#pragma once

class Player {
	short health;
public:
	Player();
	short getHealth() const { return health; }
	void alter_health(short change) { health += change; }
};