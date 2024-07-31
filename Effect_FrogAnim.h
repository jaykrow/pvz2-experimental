#pragma once

#include "Effect_PopAnim.h"
#include "Plant.h"
#include "memUtils.h"

const float WAIT_BETWEEN_JUMPS = 3.0;
const float JUMP_DURATION = 1.5;

class Effect_FrogAnim : public Effect_PopAnim
{
public:
	bool animLooped = false;
	bool animFinished = false;
	bool m_unPausedInState = false;
	bool relocatedPlant = false;
	float m_lastPauseTime = 0.0;
	float m_lastUnpausedTime = 0.0;

	DECLARE_STATIC_RT_CLASS_MEMBERS(Effect_FrogAnim)

	RT_CLASS_BUILD_SYMBOLS() {
		RtClass* rtClass = GetRTClass();
		rtClass->SetRClass(rclass);
		rclass->m_rtclass = rtClass;

		Reflection::RClassRef* parentRef = builder->GetClassRef("Effect_PopAnim");
		Reflection::RClass* parent = parentRef->GetRClass();

		// I think I'll need to register new properties for saves
		
		REGISTER_STANDARD_PROPERTY(builder, rclass, Effect_FrogAnim, animLooped);
		REGISTER_STANDARD_PROPERTY(builder, rclass, Effect_FrogAnim, animFinished);
		REGISTER_STANDARD_PROPERTY(builder, rclass, Effect_FrogAnim, m_unPausedInState);
		REGISTER_STANDARD_PROPERTY(builder, rclass, Effect_FrogAnim, relocatedPlant);
		REGISTER_STANDARD_PROPERTY(builder, rclass, Effect_FrogAnim, m_lastPauseTime);
		REGISTER_STANDARD_PROPERTY(builder, rclass, Effect_FrogAnim, m_lastUnpausedTime);

		builder->SetRClassWithParent(rclass, parent, 0);

		buildEventCallbacks(builder, (void*) rclass);
	}

	DEFINE_RT_CLASS_CONSTRUCT_FUNCTION(Effect_FrogAnim, 0xE46DD8);

	DEFINE_RT_CLASS_GET_CLASS_FUNCTION(Effect_FrogAnim, 0xE45E40);

	DEFINE_RT_CLASS_REGISTER_CLASS_FUNCTION(Effect_FrogAnim);

	static void buildEventCallbacks(Reflection::CRefManualSymbolBuilder* builder, void* rtClass);

	bool isJumping();
};

void FrogAnimOnStopOrLoop(Effect_FrogAnim* r0_0);
