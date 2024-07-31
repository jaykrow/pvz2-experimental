#pragma once
#include "GameObject.h"
#include "Graphics.h"

class RealObject : public GameObject, public Renderable
{
public:
	SexyVector3 m_position;
	int m_teamFlags;
	int m_realObjectFlags;

	virtual void UpdatePosition(SexyVector3& newPos) {};
	virtual void QueueDraw(Graphics* g) {};
	virtual void Draw(Graphics* g) {};
	virtual bool HasShadow() {};
	virtual void Function17() {};
	virtual bool CanSink() {};
	virtual void UpdateSinkDistance(bool noOverride) {}; // override a deeper sink distance with a more shallow one
	virtual Rect GetHitRect() {};
	virtual void Function21() {};
	virtual void Function22() {};
	virtual int GetRenderOrder() {};
	virtual FPoint GetGridExtents() {};
	virtual void Function25() {};
	virtual void Function26() {};
	virtual void RenderAnimRigs(Graphics* g) {};
	virtual void RenderShadows(Graphics* g) {};
	virtual void Update() {}; // runs every frame
	virtual void UpdateGroundClipRect(Graphics* graphics) {}; // uses to clip the ground for ground effects
	virtual float GetMaxTideLoweredPercent() {};
	virtual float GetSinkDistance() {};
};

static_assert(offsetof(RealObject, m_position) == 0x14);