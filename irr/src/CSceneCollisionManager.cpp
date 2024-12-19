// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSceneCollisionManager.h"
#include "ICameraSceneNode.h"
#include "SViewFrustum.h"

#include "irrMath.h"
#include "vector3d.h"

namespace irr
{
namespace scene
{

//! constructor
CSceneCollisionManager::CSceneCollisionManager(ISceneManager *smanager, video::IVideoDriver *driver) :
		SceneManager(smanager), Driver(driver)
{
	if (Driver)
		Driver->grab();
}

//! destructor
CSceneCollisionManager::~CSceneCollisionManager()
{
	if (Driver)
		Driver->drop();
}

core::line3d<f32> getRayFromScreenCoordinates(
		const core::vector2df &pos, const SViewFrustum *f, bool orthogonal)
{
	core::line3d<f32> ln(0, 0, 0, 0, 0, 0);

	core::vector3df farLeftUp = f->getFarLeftUp();
	core::vector3df lefttoright = f->getFarRightUp() - farLeftUp;
	core::vector3df uptodown = f->getFarLeftDown() - farLeftUp;

	if (orthogonal)
		ln.start = f->cameraPosition + (lefttoright * (pos.X - 0.5f)) + (uptodown * (pos.Y - 0.5f));
	else
		ln.start = f->cameraPosition;

	ln.end = farLeftUp + (lefttoright * pos.X) + (uptodown * pos.Y);

	return ln;
}

//! Returns a 3d ray which would go through the 2d screen coordinates.
core::line3d<f32> CSceneCollisionManager::getRayFromScreenCoordinates(
		const core::position2d<s32> &pos, const ICameraSceneNode *camera)
{
	core::line3d<f32> ln(0, 0, 0, 0, 0, 0);

	if (!SceneManager)
		return ln;

	if (!camera)
		camera = SceneManager->getActiveCamera();

	if (!camera)
		return ln;

	const core::rect<s32> &viewPort = Driver->getViewPort();
	f32 dx = pos.X / (f32)viewPort.getWidth();
	f32 dy = pos.Y / (f32)viewPort.getHeight();

	return irr::scene::getRayFromScreenCoordinates(core::vector2df(dx, dy),
			camera->getViewFrustum(), camera->isOrthogonal());
}


} // end namespace scene
} // end namespace irr
