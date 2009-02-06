/* $Id$ */
#include "stdafx.h"

#include "gl_object.h"

GLObject::GLObject() {
	x=y=z=0.0f;
	xrot=yrot=zrot=0.0f;
	m_useTexture = false; 
	m_visible = true;
}

void GLObject::setVisible(bool v) {
	m_visible = v;
}

void GLObject::AfterDraw() {
	if (m_useTexture)
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

bool GLObject::isVisible() const {
	return(m_visible);
}

void GLObject::BeforeDraw() {
	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(xrot, 1, 0, 0);
	glRotatef(yrot, 0, 1, 0);
	glRotatef(zrot, 0, 0, 1);
	if (m_useTexture) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_textureId);
	}
}

void GLObject::Render(long tickdelay) {
	m_tickdelay = tickdelay;
	BeforeDraw();
	Draw();
	AfterDraw();
}

void GLObject::setPos(const float& x, const float& y, const float& z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

void GLObject::setRot(const float& x, const float& y, const float& z) {
	this->xrot = x;
	this->yrot = y;
	this->zrot = z;
}

void GLObject::setRotX(const float& x) {
	xrot = x;
}

void GLObject::setRotY(const float& y) {
	yrot = y;
}

void GLObject::setRotZ(const float& z) {
	zrot = z;
}

void GLObject::setTexture(const unsigned int& textureId) {
	m_textureId = textureId;
}

void GLObject::useTexture(bool b) {
	m_useTexture = b;
}