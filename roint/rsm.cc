/* $id$ */
#include "stdafx.h"
#include "rsm.h"
#include "ro.h"

#include <vector>

#ifndef MAX
#	define MAX(x,y) (((x)>(y))?(x):(y))
#endif

#ifndef MIN
#	define MIN(x,y) (((y)>(x))?(x):(y))
#endif

RO::RSM::RSM() : Object() {
	m_meshes = NULL;
}

RO::RSM::~RSM() {
	if (m_meshes != NULL)
		delete[] m_meshes;
}

bool RO::RSM::Write(std::ostream& s) const {
	writeHeader(s);
	if (m_version.cver.major == 1 && (m_version.cver.minor == 2 || m_version.cver.minor == 3))
		s.write(garbage, 24);
	else
		s.write(garbage, 25);

	m_textures.Write(s);

	for (unsigned int i = 0; i < meshCount; i++)
		m_meshes[i].Write(s);

	return(true);
}

bool RO::RSM::readStream(std::istream& s) {
	readHeader(s);

	if (!checkHeader(RSM_HEADER)) {
		std::cout << "Invalid RSM header (" << magic[0] << magic[1] << magic[2] << magic[3] << ")" << std::endl;
		return(false);
	}
	unsigned int i;

	/*
	if (m_version.cver.major > 3) {
		std::cerr << "We can't handle RSM with major_version over 3. (we got " << (int)m_version.cver.major << ")" << std::endl;
		return(false);
	}
	*/

	// In versions 1.2 and 1.3, ignore 24 bytes. Others, 25 bytes.
	if (m_version.cver.major == 1 && (m_version.cver.minor == 2 || m_version.cver.minor == 3))
		s.read(garbage, 24);
	else
		s.read(garbage, 25);

	// ===== Reading texture names
	m_textures.readStream(s);
	// The old-fashioned way, just for reference...
	//s.read((char*)&textureCount, sizeof(unsigned int));
	//m_textures = new TexName[textureCount];
	//s.read((char*)m_textures, sizeof(TexName) * textureCount);
	// TODO: Convert to UTF8

	// ===== Reading meshes
	Mesh* m;
	std::vector<Mesh*> meshes;
	bool main = true;
	while(!s.eof()) {
		s.get();
		if (s.eof()) break;
		s.unget();

		m = new Mesh;
		if (m->readStream(s, main))
			meshes.push_back(m);
		else {
			delete m;
			break;
		}

		main = false;
	}
	if (meshes.size() == 1)
		meshes[0]->is_only = true;

	m_meshes = NULL;
	meshCount = meshes.size();
	if (meshCount == 0)
		return(true);

	m_meshes = new Mesh[meshCount];
	for (i = 0; i < meshCount; i++) {
		m_meshes[i] = *meshes[i];
	}
	for (i = 0; i < meshCount; i++) {
		delete meshes[i];
	}
	meshes.clear();

	return(true);
}

unsigned int RO::RSM::getMeshCount() const {
	return(meshCount);
}

unsigned int RO::RSM::getTextureCount() const {
	return(m_textures.size());
}

RO::RSM::Mesh& RO::RSM::operator[] (const unsigned int& idx) {
	return(m_meshes[idx]);
}

const RO::RSM::Mesh& RO::RSM::operator[] (const unsigned int& idx) const {
	return(m_meshes[idx]);
}

RO::RSM::Mesh& RO::RSM::getMesh(const unsigned int& idx) {
	return(m_meshes[idx]);
}

const RO::RSM::Mesh& RO::RSM::getMesh(const unsigned int& idx) const {
	return(m_meshes[idx]);
}

char* RO::RSM::getTexture(const unsigned int& idx) {
	return(m_textures[idx]);
}

const char* RO::RSM::getTexture(const unsigned int& idx) const {
	return(m_textures[idx]);
}

void RO::RSM::Dump(std::ostream& out, const std::string& prefix) const {
	unsigned int i;

	std::string mypfx = prefix + "\t";

	out << prefix << "Version " << (short)m_version.cver.major << "." << (short)m_version.cver.minor << std::endl;
	out << prefix << "Garbage ";
	char buf[16];
	for (i = 0; i < 25; i++) {
		sprintf(buf, "0x%x ", garbage[i]);
		out << buf;
	}
	out << std::endl;


	out << prefix << "Textures (" << m_textures.size() << ")" << std::endl;
	for (i = 0; i < (unsigned int)m_textures.size(); i++)
		out << mypfx << m_textures[i] << std::endl;

	out << prefix << "Meshes (" << meshCount << ")" << std::endl;
	for (i = 0; i < meshCount; i++) {
		m_meshes[i].Dump(out, mypfx);
	}
}

RO::RSM::Mesh::Mesh() {
    is_main = true;
    is_only = false;
}

RO::RSM::Mesh::Mesh(const RO::RSM::Mesh& m) {
	*this = m;
}

RO::RSM::Mesh::~Mesh() {
	Clear();
}

void RO::RSM::Mesh::Clear() {
    memset((char*)&header, 0, sizeof(Header));
    memset((char*)&transf, 0, sizeof(Transf));
    textures.Clear();
    vecs.Clear();
    texv.Clear();
    surfaces.Clear();
    frames.Clear();
}

RO::RSM::Mesh& RO::RSM::Mesh::operator = (const RO::RSM::Mesh& m) {
	memcpy((char*)&header, (char*)&m.header, sizeof(Header));
	memcpy((char*)&transf, (char*)&m.transf, sizeof(Transf));
	textures = m.textures;
	vecs = m.vecs;
	texv = m.texv;
	surfaces = m.surfaces;
	frames = m.frames;

	is_main = m.is_main;
	is_only = m.is_only;

	memcpy((char*)&box, (char*)&m.box, sizeof(BoundingBox));

	return(*this);
}

RO::RSM::Mesh& RO::RSM::Mesh::operator = (const RO::RSM::Mesh* m) {
	memcpy((char*)&header, (char*)&m->header, sizeof(Header));
	memcpy((char*)&transf, (char*)&m->transf, sizeof(Transf));
	textures = m->textures;
	vecs = m->vecs;
	texv = m->texv;
	surfaces = m->surfaces;
	frames = m->frames;

	is_main = m->is_main;
	is_only = m->is_only;

	memcpy((char*)&box, (char*)&m->box, sizeof(BoundingBox));

	return(*this);
}

void RO::RSM::Mesh::Write(std::ostream& s) const {
  if (is_main) {
    s.write((char*)&header, sizeof(header));
  }
  else {
    s.write((char*)header.name, 40);
    s.write((char*)header.parent, 40);
  }
  textures.Write(s);
  s.write((char*)&transf, sizeof(Transf));
  vecs.Write(s);
  texv.Write(s);
  surfaces.Write(s);
  frames.Write(s);
}

bool RO::RSM::Mesh::readStream(std::istream& s, bool main) {
	Clear();

	if (s.eof())
	return(false);

	is_main = main;
	// if (main) printf(" M ");

	if (is_main) {
		s.read((char*)&header, sizeof(Header));
	}
	else {
		s.read((char*)header.name, 40);
		s.read((char*)header.parent, 40);
	}

	if (!textures.readStream(s))
		return(false);
	if (textures.size() == 0)
		return(false);

	s.read((char*)&transf, sizeof(Transf));

	if (s.eof()) {
		Clear();
		return(false);
	}
	vecs.readStream(s);
	if (s.eof()) {
		Clear();
		return(false);
	}
	texv.readStream(s);
	if (s.eof()) {
		Clear();
		return(false);
	}
	surfaces.readStream(s);
	if (s.eof()) {
		Clear();
		return(false);
	}
	frames.readStream(s);

	calcBoundingBox();

	return(true);
}

void DumpBox(const RO::RSM::BoundingBox& box, const std::string& prefix = "", std::ostream& out = std::cout) {
	// BOX
	char buf[512];

	out << prefix << "Box" << std::endl;
	sprintf(buf, "\tMax %.2f %.2f %.2f", box.max.v[0], box.max.v[1], box.max.v[2]);
	out << prefix << buf << std::endl;
	sprintf(buf, "\tMin %.2f %.2f %.2f", box.min.v[0], box.min.v[1], box.min.v[2]);
	out << prefix << buf << std::endl;
	sprintf(buf, "\tRange %.2f %.2f %.2f", box.range.v[0], box.range.v[1], box.range.v[2]);
	out << prefix << buf << std::endl;
}

void RO::RSM::Mesh::calcBoundingBox() {
	box.max.v[0] = box.max.v[1] = box.max.v[2] = -999999.0;
	box.min.v[0] = box.min.v[1] = box.min.v[2] = 999999.0;

	int i, j;
	Vec v;

	for (i = 0; i < vecs.size(); i++) {
		v.c.x = vecs[i].c.x;
		v.c.y = vecs[i].c.y;
		v.c.z = vecs[i].c.z;

		for (j = 0; j < 3; j++) {
			if (!is_only) v.v[j] += transf.translate1.v[j] + transf.translate2.v[j];

			box.min.v[j] = MIN(v.v[j], box.min.v[j]);
			box.max.v[j] = MAX(v.v[j], box.max.v[j]);
		}
	}

	for (i = 0; i < 3; i++)
		box.range.v[i] = (box.max.v[i] + box.min.v[i]) / 2.0f;
}

void RO::RSM::Mesh::Dump(std::ostream& out, const std::string& prefix) const {
	char buf[512];

	out << prefix << "Mesh " << header.name << std::endl;
	out << prefix << "Parent " << header.parent << std::endl;

	out << prefix << "TransformMatrix" << std::endl;
	sprintf(buf, "\t%.5f\t%.5f\t%.5f", transf.mat33[0], transf.mat33[1], transf.mat33[2]);
	out << prefix << buf << std::endl;
	sprintf(buf, "\t%.5f\t%.5f\t%.5f", transf.mat33[3], transf.mat33[4], transf.mat33[5]);
	out << prefix << buf << std::endl;
	sprintf(buf, "\t%.5f\t%.5f\t%.5f", transf.mat33[6], transf.mat33[7], transf.mat33[8]);
	out << prefix << buf << std::endl;

	out << prefix;
	sprintf(buf, "Pos1 (%.2f, %.2f, %.2f)\t", transf.translate1.c.x, transf.translate1.c.y, transf.translate1.c.z);
	out << buf;
	sprintf(buf, "Pos2 (%.2f, %.2f, %.2f)", transf.translate2.c.x, transf.translate2.c.y, transf.translate2.c.z);
	out << buf << std::endl;

	sprintf(buf, "Rotation %.2f (%.2f %.2f %.2f)", transf.rot_angle, transf.rot_vector.c.x, transf.rot_vector.c.y, transf.rot_vector.c.z);
	out << prefix << buf << std::endl;
	sprintf(buf, "Scale (%.2f, %.2f, %.2f)", transf.scale.c.x, transf.scale.c.y, transf.scale.c.z);
	out << prefix << buf << std::endl;

	// BOX
	DumpBox(box, prefix, out);

	int v[3], t[3];
	for (int i = 0; i < surfaces.size(); i++) {
		v[0] = surfaces[i].sv[0];
		v[1] = surfaces[i].sv[2];
		v[2] = surfaces[i].sv[1];

		t[0] = surfaces[i].tv[0];
		t[1] = surfaces[i].tv[2];
		t[2] = surfaces[i].tv[1];

		out << prefix << i << "\tv ";
		sprintf(buf, "(%.2f, %.2f, %.2f),", vecs[v[0]].c.x, vecs[v[0]].c.y, vecs[v[0]].c.z);
		out << buf << " ";
		sprintf(buf, "(%.2f, %.2f, %.2f),", vecs[v[1]].c.x, vecs[v[1]].c.y, vecs[v[1]].c.z);
		out << buf << " ";
		sprintf(buf, "(%.2f, %.2f, %.2f)", vecs[v[2]].c.x, vecs[v[2]].c.y, vecs[v[2]].c.z);
		out << buf << std::endl;

		out << prefix << "\tt ";
		sprintf(buf, "(%.2f, %.2f, %.2f),", texv[t[0]].c.x, texv[t[0]].c.y, texv[t[0]].c.z);
		out << buf << " ";
		sprintf(buf, "(%.2f, %.2f, %.2f),", texv[t[1]].c.x, texv[t[1]].c.y, texv[t[1]].c.z);
		out << buf << " ";
		sprintf(buf, "(%.2f, %.2f, %.2f)", texv[t[2]].c.x, texv[t[2]].c.y, texv[t[2]].c.z);
		out << buf << std::endl;
	}
}

