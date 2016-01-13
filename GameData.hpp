#pragma once

class Data {
	bool playFirst;
	bool orderDecided;
public:
	Data();
	~Data();
	bool getPlayFirst() const { return playFirst; }
	bool getorderDecided() const { return orderDecided; }
	void setPlayFirst(bool first) { playFirst = first; }
	void setOrderDecided(bool decided) { orderDecided = decided; }

};