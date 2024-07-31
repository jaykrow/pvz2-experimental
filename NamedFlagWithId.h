#pragma once
class NamedFlagWithId
{
public:
	SexyString namedFlag;
	int id;

	NamedFlagWithId(SexyString namedFlag, int id) : namedFlag(namedFlag), id(id) {};
};

